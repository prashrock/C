
/*
 * Copyright (c) 2010 by Brocade Communications Systems, Inc.
 * All rights reserved.
 *
 * Module name: main.c
 *
 * Description:
 * mmap library routines (kernel infrastructure code).
 * this file contains kernel code for cos mmap and implemented 
 * as char device driver, module is inserted at run time and memory
 * is allocated at init time but passed to user space code with 
 * fault/no-page mechanism. This works fine for mapping RAM into to user
 * space.
 * TODO LIST
 * - Have fixed major number - Done
 * - create /dev/<device> at boot time with mknod and same is used in mmap driver. - Done
 * - library for L2/L3 will be added on top of this infrastructure code. - Done
 * - add ioctl for different mode. -TBD
 * - change it to run time allocation instead for fix MAXQSETINIT. - TBD
 * - To enhance it to work witj l3 /l2 / multicast. -TBD
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/cdev.h>
#include <linux/slab.h>		/* kmalloc() */
#include <linux/vmalloc.h>		/* vmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/aio.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#include <asm/ioctls.h>
#include <linux/sockios.h>
#include "cosmmap.h"		/* local definitions */
#include "usr_cosmmap.h" 	/* library header files */


int cos_major =   COS_MAJOR;
int fib_cos_major =   FIB_COS_MAJOR;
int cos_devs =    COS_DEVS;	/* number of bare cos_mmap devices */
int cos_qset =    COS_QSET;
int cos_order =   COS_ORDER;
void *gptr = (void *)NULL ;

module_param(cos_major, int, 0);
module_param(fib_cos_major, int, 0);
module_param(cos_devs, int, 0);
module_param(cos_qset, int, 0);
module_param(cos_order, int, 0);
MODULE_AUTHOR("Anmol Kumar");
MODULE_LICENSE("Dual BSD/GPL");

struct cos_dev *cos_devices; /* allocated in cos_init */
struct cos_dev *fib_cos_devices; /* allocated in cos_init */
static int *vmalloc_area;

int cos_trim(struct cos_dev *dev);
void cos_cleanup(void);
#define MAX_REGISTER_FNC	COS_MAX_REGION+1
#ifndef MMAPLIB_DEBUG
#define MMAPLIB_DEBUG 0
#endif
struct lcl_op {
	void  *gval ;
	void (*pfnc)(void *ptr, unsigned long val);
};
struct lcl_op register_op[MAX_REGISTER_FNC];
extern struct cosmdata *cosmdata;

ssize_t fib_cos_write (struct file *filp, const char __user *buf1, size_t count,
                loff_t *f_pos)
{
	return 0;
}
/*
 * code to simulate driver timeout, user space code can
 * issue write to perform action for driver.
 * - get_buf, perform buf  manipulation (none), return it
 * - allocate new buf (learn event), put buf so user can use it.
 */
ssize_t cos_write (struct file *filp, const char __user *buf1, size_t count,
                loff_t *f_pos)
{

#if 0
	int ret = 0 ;
	char *buf = NULL ;
	int index = 0;
	int region_id = 1;
	int tret = 0;

	for (index = 1; index < 18+1; index++) 
	{
		if((index %3 ) == 1 )
			region_id = index ;
		else
			continue;
		ret = cos_mmap_get_buf(region_id, &buf );	
		printk(KERN_INFO "%s %d) region (%d) ret(%d) buffer (%x) region(%x) \n", __FUNCTION__, __LINE__, region_id,  ret, buf, cosmdata->region[region_id-1]);
		if( ret == COSMMAPSUC) 
		{
			printk(KERN_INFO "%s %d: get_buf ret(%d) buf(%p) region (%d)  bufinfo [%x %x %x %x %x %x %x %x %x %x %x]\n\n",
				__FUNCTION__, __LINE__, ret, buf, region_id, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10]);
			ret = cos_mmap_return_buf(region_id, buf ) ;
			printk(KERN_INFO "%s %d: return buf(%d) buf(%p) region %d\n" ,
				__FUNCTION__, __LINE__, ret, buf, region_id ) ;
			buf = NULL ;
		}
	}

#endif
	return 0;
}

int cos_open (struct inode *inode, struct file *filp)
{
	struct cos_dev *dev; /* device information */

	/*  Find the device */
	dev = container_of(inode->i_cdev, struct cos_dev, cdev);

    	/* now trim to 0 the length of the device if open was write-only */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (down_interruptible (&dev->sem))
			return -ERESTARTSYS;
		cos_trim(dev); /* ignore errors */
		up (&dev->sem);
	}

	/* and use filp->private_data to point to the device data */
	filp->private_data = dev;

	return 0;          /* success */
}

int fib_cos_trim(struct cos_dev *dev)
{
	struct cos_dev *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	if (dev->vmas) /* don't trim: there are active mappings */
		return -EBUSY;

	for (dptr = dev; dptr; dptr = next) { /* all the list items */
		if (dptr->data) {
			/* This code frees a whole quantum-set */
			for (i = 0; i < qset; i++)
				if (dptr->data[i])
					free_pages((unsigned long)(dptr->data[i]),
							dptr->order);

		}
		next=dptr->next;
		if (dptr != dev) kfree(dptr); /* all of them but the first */
	}
	dev->size = 0;
	dev->qset = cos_qset;
	dev->order = cos_order;
	dev->next = NULL;
	return 0;
}


int fib_cos_open (struct inode *inode, struct file *filp)
{
	struct cos_dev *dev; /* device information */

	/*  Find the device */
	dev = container_of(inode->i_cdev, struct cos_dev, cdev);

    	/* now trim to 0 the length of the device if open was write-only */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (down_interruptible (&dev->sem))
			return -ERESTARTSYS;
		fib_cos_trim(dev); /* ignore errors */
		up (&dev->sem);
	}

	/* and use filp->private_data to point to the device data */
	filp->private_data = dev;

	return 0;          /* success */
}

int fib_cos_release (struct inode *inode, struct file *filp)
{
	return 0;
}

int cos_release (struct inode *inode, struct file *filp)
{
	return 0;
}

int cos_mmap_register_fnc(void (*fnc)(void *ptr, unsigned long val), void *ptr, int region)
{
	if(region < 1 || region > (MAX_REGISTER_FNC-1))
		return 0;

	if(fnc != NULL)
	{
		register_op[region].pfnc = fnc;
		register_op[region].gval =  ptr;
		return 1;	
	}
	return 0;
}

EXPORT_SYMBOL(cos_mmap_register_fnc) ;

long cos_unioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int count = 0;
	int data = 0 ;

	if (copy_from_user(&data, (int __user *)arg, sizeof(int )))
		return -EFAULT;

	if(cmd != SIOCDEVPRIVATE)
		return -EINVAL;

	if(data < 1 || data > (MAX_REGISTER_FNC-1))
		return 0;

	if(register_op[data].pfnc)
		register_op[data].pfnc( register_op[data].gval, data) ;

	return put_user(count, (int __user *)arg);
}


long fib_cos_unioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
#if 0
	int count = 0;
	int data = 0 ;

	if (copy_from_user(&data, (int __user *)arg, sizeof(int )))
		return -EFAULT;

	if(cmd != SIOCDEVPRIVATE)
		return -EINVAL;

	if(data < 1 || data > (MAX_REGISTER_FNC-1))
		return 0;

	if(register_op[data].pfnc)
		register_op[data].pfnc( register_op[data].gval, data) ;

	return put_user(count, (int __user *)arg);
#endif 
	return 0;
}



/*
 * Follow the list 
 */
struct cos_dev *cos_follow(struct cos_dev *dev, int n)
{
	while (n--) {
		if (!dev->next) {
			dev->next = kmalloc(sizeof(struct cos_dev), GFP_KERNEL);
			memset(dev->next, 0, sizeof(struct cos_dev));
		}
		dev = dev->next;
		continue;
	}
	return dev;
}

 
/*
 * open and close: just keep track of how many times the device is
 * mapped, to avoid releasing it.
 */

void fib_cos_vma_open(struct vm_area_struct *vma)
{
    struct cos_dev *dev = vma->vm_private_data;

    dev->vmas++;
}

void fib_cos_vma_close(struct vm_area_struct *vma)
{
    struct cos_dev *dev = vma->vm_private_data;

    dev->vmas--;
}

int fib_cos_vma_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	return 0;
}

struct vm_operations_struct fib_cos_vm_ops = { 
    .open =     fib_cos_vma_open,
    .close =    fib_cos_vma_close,
    //.nopage =   cos_vma_nopage, // for kernel 2.6.14
    .fault =   fib_cos_vma_fault,
};



/*
 * Mmap *is* available, but confined in a different file
 */
extern int cos_mmap(struct file *filp, struct vm_area_struct *vma);

// helper function, mmap's the kmalloc'd area which is physically contiguous
int mmap_kmem(struct file *filp, struct vm_area_struct *vma)
{
    int ret;  
    long length = vma->vm_end - vma->vm_start;  
    unsigned long start = vma->vm_start;  
    char *vmalloc_area_ptr = (char *)vmalloc_area;  
    unsigned long pfn;  

    /* check length - do not allow larger mappings than the number of 
       pages allocated */  
	printk(KERN_INFO "%d L3MMAP", __LINE__);
    if (length > MAX_FREE_BUF * PAGE_SIZE)  
	{
	printk(KERN_INFO "%d L3MMAP", __LINE__);
        return -EIO;  
	}

	vma->vm_flags |= (VM_MAYSHARE);

    /* loop over all pages, map it page individually */  
    while (length > 0) {   
        pfn = vmalloc_to_pfn(vmalloc_area_ptr);  
        if ((ret = remap_pfn_range(vma, start, pfn, PAGE_SIZE,
                        PAGE_SHARED)) < 0) {
            printk(KERN_ERR "Error - Remapping for vm_area 0x%x not successful by bk_mmap\n", vma);
            printk(KERN_INFO "Error - Remapping for vm_area 0x%x not successful by bk_mmap\n", vma);
            return ret;  
        }
        start += PAGE_SIZE;  
        vmalloc_area_ptr += PAGE_SIZE;  
        length -= PAGE_SIZE;  
    }   
	vma->vm_ops = &fib_cos_vm_ops;
    return 0;  

}

int fib_cos_mmap(struct file *file, struct vm_area_struct *vma)
{
    struct inode *inode ;
    inode  = file->f_path.dentry->d_inode;

    int uaddr;

	printk(KERN_INFO "%d L3MMAP", __LINE__);
    if (vma->vm_pgoff == 0) {   
	printk(KERN_INFO "%d L3MMAP", __LINE__);
        uaddr = mmap_kmem(file, vma);  
    }
	printk(KERN_INFO "vma->vm_start = 0x%x vma->vm_end = 0x%x\n",(unsigned int)vma->vm_start, (unsigned int) vma->vm_end);
	printk(KERN_INFO "%d L3MMAP", __LINE__);

    return 0;
}

/*
 * The fops
 */

struct file_operations cos_fops = {
	.owner =     THIS_MODULE,
	.mmap =	     cos_mmap,
	.open =	     cos_open,
	.write =     cos_write,
	.release =   cos_release,
	.unlocked_ioctl =	cos_unioctl,
};

struct file_operations fib_cos_fops = {
	.owner =     THIS_MODULE,
	.mmap =	     fib_cos_mmap,
	.open =	     fib_cos_open,
	.write =     fib_cos_write,
	.release =   fib_cos_release,
};


int cos_trim(struct cos_dev *dev)
{
	struct cos_dev *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	if (dev->vmas) /* don't trim: there are active mappings */
		return -EBUSY;

	for (dptr = dev; dptr; dptr = next) { /* all the list items */
		if (dptr->data) {
			/* This code frees a whole quantum-set */
			for (i = 0; i < qset; i++)
				if (dptr->data[i])
					free_pages((unsigned long)(dptr->data[i]),
							dptr->order);

		}
		next=dptr->next;
		if (dptr != dev) kfree(dptr); /* all of them but the first */
	}
	dev->size = 0;
	dev->qset = cos_qset;
	dev->order = cos_order;
	dev->next = NULL;
	return 0;
}


static void cos_setup_cdev(struct cos_dev *dev, int index)
{
	int err, devno = MKDEV(cos_major, index);
    
	cdev_init(&dev->cdev, &cos_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &cos_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding cos%d", err, index);
}


static void fib_cos_setup_cdev(struct cos_dev *dev, int index)
{
	int err, devno = MKDEV(fib_cos_major, index);
	printk(KERN_INFO "devno %d adding index %d", devno, index);
    
	cdev_init(&dev->cdev, &fib_cos_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &fib_cos_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding cos%d", err, index);
}


/*
 * Finally, the module stuff
 */
int cos_mmap_init(void)
{
	int result, i, j;
	dev_t dev = MKDEV(cos_major, 0);
	struct cos_dev *dptr;
	
	#if(MMAPLIB_DEBUG)
		 printk(KERN_INFO "%s) Entering cos_init ...... \n", __FUNCTION__ ) ;
	#endif
	
	/*
	 * Register your major, and accept a dynamic number.
	 */
	if (cos_major) 
	{
		result = register_chrdev_region(dev, cos_devs, "cosmmap");
		#if(MMAPLIB_DEBUG)
			 printk(KERN_INFO "cos result  <%d> major <%d>  \n", result, cos_major );
		#endif
	}
	else 
	{
		result = alloc_chrdev_region(&dev, 0, cos_devs, "cosmmap");
		cos_major = MAJOR(dev);
		#if(MMAPLIB_DEBUG)
			 printk(KERN_INFO "cos result/major  <%d>/<%d> \n", result, cos_major );
		#endif
	}
	if (result < 0)
		return result;

	memset(&register_op[0], 0, sizeof(struct lcl_op)*MAX_REGISTER_FNC);
	
	/* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	cos_devices = kmalloc(cos_devs*sizeof (struct cos_dev), GFP_KERNEL);
	if (!cos_devices) 
	{
		result = -ENOMEM;
		goto fail_malloc;
	}
	memset(cos_devices, 0, cos_devs*sizeof (struct cos_dev));
	for (i = 0; i < cos_devs; i++) 
	{
		cos_devices[i].order = cos_order;
		cos_devices[i].qset = cos_qset;
		sema_init (&cos_devices[i].sem, 1);
		cos_setup_cdev(cos_devices + i, i);
		dptr = cos_devices;
		dptr->qset = MAXQSETINIT;
		dptr->order = 0;
		memset(&dptr->data[0], 0, dptr->qset * sizeof(void *));
		#if(MMAPLIB_DEBUG)
			 printk(KERN_INFO "%s) Allocation device starts QSET (%d) memset sz (%d)  \n", __FUNCTION__, dptr->qset, (dptr->qset * sizeof( void *)) );
		#endif

		for(j=0 ; j < dptr->qset; j++) 
		{
			#if(MMAPLIB_DEBUG)
				 printk(KERN_INFO "%s) Trying to allocating pages to data(%d) dptr->data(%p)\n"
					, __FUNCTION__, j, dptr->data[j] );
			#endif
			if (!dptr->data[j]) 
			{
				dptr->data[j] =
					(void *)__get_free_pages(GFP_KERNEL, dptr->order);

				#if(MMAPLIB_DEBUG)
					 printk(KERN_INFO "%s) allocating pages to data(%d) ptr(%p) \n",
						 __FUNCTION__, j, dptr->data[j] );
				#endif
				if (!dptr->data[j])
					printk(KERN_INFO "%s) unable to allocate memory for data -page \n", __FUNCTION__ );
				memset(dptr->data[j], 0, PAGE_SIZE << dptr->order);
				dptr->size  += PAGE_SIZE;
			}
		}
	}


	return 0; /* succeed */

  fail_malloc:
	unregister_chrdev_region(dev, cos_devs);
	return result;
}



void cos_mmap_cleanup(void)
{
	int i;
	
	for (i = 0; i < cos_devs; i++) {
		cdev_del(&cos_devices[i].cdev);
		cos_trim(cos_devices + i);
	}

	kfree(cos_devices);
	unregister_chrdev_region(MKDEV (cos_major, 0), cos_devs);
}

/*
 * Finally, the module stuff
 */
int fib_cos_mmap_init(void)
{
	int result, i, j;
	dev_t dev = MKDEV(fib_cos_major, 0);
	struct cos_dev *dptr;
	
//	#if(MMAPLIB_DEBUG)
		 printk(KERN_INFO "%s) Entering cos_init ...... \n", __FUNCTION__ ) ;
//	#endif
	
	/*
	 * Register your major, and accept a dynamic number.
	 */
	if (fib_cos_major) 
	{
		result = register_chrdev_region(dev, cos_devs, "fibcosmmap");
//		#if(MMAPLIB_DEBUG)
			 printk(KERN_INFO "cos result  <%d> major <%d>  \n", result, fib_cos_major );
//		#endif
	}
	else 
	{
		result = alloc_chrdev_region(&dev, 0, cos_devs, "fibcosmmap");
		fib_cos_major = MAJOR(dev);
		//#if(MMAPLIB_DEBUG)
			 printk(KERN_INFO "cos result/major  <%d>/<%d> \n", result, fib_cos_major );
		//#endif
	}
	if (result < 0)
		return result;

	//memset(&register_op[0], 0, sizeof(struct lcl_op)*MAX_REGISTER_FNC);
	
	/* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	fib_cos_devices = kmalloc(cos_devs*sizeof (struct cos_dev), GFP_KERNEL);
	if (!fib_cos_devices) 
	{
		result = -ENOMEM;
		goto fail_malloc;
	}
	memset(fib_cos_devices, 0, cos_devs*sizeof (struct cos_dev));
	for (i = 0; i < cos_devs; i++) 
	{
		fib_cos_devices[i].order = cos_order;
		fib_cos_devices[i].qset = cos_qset;
		sema_init (&fib_cos_devices[i].sem, 1);
		fib_cos_setup_cdev(fib_cos_devices + i, i);
		dptr = fib_cos_devices;
		dptr->qset = MAXQSETINIT;
		dptr->order = 0;
		memset(&dptr->data[0], 0, dptr->qset * sizeof(void *));
//		#if(MMAPLIB_DEBUG)
			 printk(KERN_INFO "%s) Allocation device starts QSET (%d) memset sz (%d)  \n", __FUNCTION__, dptr->qset, (dptr->qset * sizeof( void *)) );
//		#endif

		
		if ((vmalloc_area = (int *)vmalloc(dptr->qset * PAGE_SIZE)) == NULL) {
			result = -ENOMEM;  
			goto fail_malloc;  
		}
		for (j = 0; j < dptr->qset * PAGE_SIZE; j+= PAGE_SIZE) {  
			SetPageReserved(vmalloc_to_page((void *)(((unsigned long)vmalloc_area) + i)));  
		}  
#if 0
		for(j=0 ; j < dptr->qset; j++) 
		{
			#if(MMAPLIB_DEBUG)
				 printk(KERN_INFO "%s) Trying to allocating pages to data(%d) dptr->data(%p)\n"
					, __FUNCTION__, j, dptr->data[j] );
			#endif
			if (!dptr->data[j]) 
			{
				dptr->data[j] =
					(void *)__get_free_pages(GFP_KERNEL, dptr->order);

				#if(MMAPLIB_DEBUG)
					 printk(KERN_INFO "%s) allocating pages to data(%d) ptr(%p) \n",
						 __FUNCTION__, j, dptr->data[j] );
				#endif
				if (!dptr->data[j])
					printk(KERN_INFO "%s) unable to allocate memory for data -page \n", __FUNCTION__ );
				memset(dptr->data[j], 0, PAGE_SIZE << dptr->order);
				dptr->size  += PAGE_SIZE;
			}
		}
#endif		
	}


	return 0; /* succeed */

  fail_malloc:
	unregister_chrdev_region(dev, cos_devs);
	return result;
}

//module_init(cos_mmap_init);
//module_exit(cos_mmap_cleanup);
