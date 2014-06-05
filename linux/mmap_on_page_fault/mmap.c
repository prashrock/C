/* 
 * Copyright (c) 2010 by Brocade Communications Systems, Inc.
 * All rights reserved.
 *
 * Module name: mmap.c
 *
 * Description:
 * mmap library routines.
 * mmap.c -- memory mapping for the cos char module
 */
#include <linux/stat.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/shm.h>
#include <linux/errno.h>
#include <linux/mman.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/ncp_fs.h>
#include <asm/uaccess.h>
#include <asm/system.h>


#include <linux/module.h>

#include <linux/mm.h>		/* everything */
#include <linux/errno.h>	/* error codes */
#include <asm/pgtable.h>

#include "cosmmap.h"		/* local definitions */
#include "usr_cosmmap.h"       /* common usr/kernl */	
#ifndef MMAPLIB_DEBUG
#define MMAPLIB_DEBUG 0
#endif

int cos_mmap_page_cnt = 0 ;

/*
 * open and close: just keep track of how many times the device is
 * mapped, to avoid releasing it.
 */

void cos_vma_open(struct vm_area_struct *vma)
{
	struct cos_dev *dev = vma->vm_private_data;

	dev->vmas++;
}

void cos_vma_close(struct vm_area_struct *vma)
{
	struct cos_dev *dev = vma->vm_private_data;

	dev->vmas--;
}

/*
 * The nopage method: the core of the file. It retrieves the
 * page required from the cos device and returns it to the
 * user. The count for the page must be incremented, because
 * it is automatically decremented at page unmap.
 *
 * For this reason, "order" must be zero. Otherwise, only the first
 * page has its count incremented, and the allocating module must
 * release it as a whole block. Therefore, it isn't possible to map
 * pages from a multipage block: when they are unmapped, their count
 * is individually decreased, and would drop to 0.
 */

extern struct cosmdata *cosmdata ;
extern struct cosmdatakva *cosmdatakva ;
extern int gresult;
//struct page *cos_vma_nopage(struct vm_area_struct *vma,
 //                               unsigned long address, int *type)
static int cos_vma_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	unsigned long offset;
	struct cos_dev *ptr, *dev = vma->vm_private_data;
	struct page *page = NULL ;
	void *pageptr = NULL; /* default to "missing" */
	int index = 0;
	unsigned long address = (unsigned long )vmf->virtual_address;

	#if(MMAPLIB_DEBUG) 
		printk(KERN_INFO "%s) va (%p) address(%lu) \n", __FUNCTION__, vmf->virtual_address, address ) ;
		printk(KERN_INFO "%s) address (%lu) vm_start(%lu) vm_pgoff(%lu) PAGE_SHIFT(%d) dev_size(%d) \n",
			__FUNCTION__, (unsigned long) vmf->virtual_address ,vma->vm_start, vma->vm_pgoff, PAGE_SHIFT, dev->size );
	#endif

	offset = (address - vma->vm_start) + (vma->vm_pgoff << PAGE_SHIFT);

	#if(MMAPLIB_DEBUG)
		printk(KERN_INFO "%s) offset (%lu) devsz (%d) \n", __FUNCTION__, offset, dev->size );
	#endif

	if (offset >= dev->size) goto out; /* out of range */

	#if(MMAPLIB_DEBUG)
		printk(KERN_INFO "%s) offset(%lu) dev->qset (%d)\n", __FUNCTION__, offset, dev->qset );
	#endif

	/*
	 * Now retrieve the cos device from the list,then the page.
	 * If the device has holes, the process receives a SIGBUS when
	 * accessing the hole.
	 */
	offset >>= PAGE_SHIFT; /* offset is a number of pages */
	for (ptr = dev; ptr && offset >= dev->qset;) 
	{
		ptr = ptr->next;
		offset -= dev->qset;
	}
	if (ptr && ptr->data) pageptr = ptr->data[offset];
	if (!pageptr) goto out; /* hole or end-of-file */
	/*
	 * lets capture uva in kernel mmap device structure
	 * this will be used for mapping/verification etc.
	 */
	ptr->uva[offset] = (unsigned long )vmf->virtual_address ;
	page = virt_to_page(pageptr);

	/* got it, now increment the count */
	get_page(page);
	
	/* write mmap kernel info in shared memory mdata */
	cos_mmap_page_cnt++;
	if(cos_mmap_page_cnt == KRNL_DUMP_DATA_CNT)
	{
		/*
		 * assign metadata to cosmdata/cosmdatakva and kernel library 
		 * uses this.
		 * cosmdatakva holds kva for user space mmap address.
		 */
		cosmdata =   (struct cosmdata *) ptr->data[KRNL_MMAP_MDATA_PAGE_OFFSET]  ;
		cosmdatakva =   (struct cosmdatakva *) ptr->data[KRNL_MMAP_MDATAKVA_PAGE_OFFSET] ;
		/*
		 * all kva are part of meta data
		 */
		#if(MMAPLIB_DEBUG)
			printk(KERN_INFO "%s) Updating meta data with kva \n", __FUNCTION__ );
			printk(KERN_INFO "%s) cosmdatakva(%x) \n", __FUNCTION__, cosmdatakva );
			printk(KERN_INFO "%s) cosmdata(%x) \n", __FUNCTION__, cosmdata );
		#endif
		for(index=0; index<MAX_FREE_BUF; index++) 
		{
			//cosmdata->kva[index] = (void *)ptr->data[index];
			#if(MMAPLIB_DEBUG)
			//	 printk(KERN_INFO "%s) index (%d) kva (%p)\n", __FUNCTION__, index, cosmdata->kva[index] );
				 printk(KERN_INFO "%s) index (%d) kva (%p)\n", __FUNCTION__, index, cosmdatakva->kva[index] );
			#endif
			cosmdatakva->kva[index] = (void *)ptr->data[index];
		}
	}
  out:
	vmf->page = page ;
	return  0;
}



struct vm_operations_struct cos_vm_ops = {
	.open =     cos_vma_open,
	.close =    cos_vma_close,
	//.nopage =   cos_vma_nopage, // for kernel 2.6.14
	.fault =   cos_vma_fault,
};


int cos_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct inode *inode ;
	inode  = file->f_path.dentry->d_inode;

	printk(KERN_INFO "%s) L2MMAP");
	/* refuse to map if order is not 0 */
	if (cos_devices[iminor(inode)].order)
		return -ENODEV;

	/* don't do anything here: "nopage" will set up page table entries */
	vma->vm_ops = &cos_vm_ops;
	vma->vm_flags |= VM_RESERVED;
	vma->vm_private_data = file->private_data;

	#if(MMAPLIB_DEBUG)
		 printk(KERN_INFO "%s) vma->start(%lu) vma->end (%lu)  pgoff(%lu) \n",
			__FUNCTION__, vma->vm_start, vma->vm_end, vma->vm_pgoff  );
	#endif

	cos_vma_open(vma);
	return 0;
}

