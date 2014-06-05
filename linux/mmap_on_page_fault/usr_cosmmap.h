/* 
 * Copyright (c) 2010 by Brocade Communications Systems, Inc.
 * All rights reserved.
 *
 * Module name: usr_cosmmap.h
 *
 * Description:
 * mmap library routines.
 */
#ifndef __USR_COSMMAP_H___
#define __USR_COSMMAP_H__

#include "../../../../bccb/sys/include/mmap/cosmmaplib.h"

#ifdef COS_MMAP_USER
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#endif

#define COS_MAX_HIS	100
/*
 * default memory details 
 */
#define COS_RR_SZ			4096

#define COS_MAX_PAGE_PER_CHIP 	32
#define COS_MAX_PAGE_AGENT 		32
#define COS_MAX_PAGE_DRV 		32
#define COS_PAGE_DRV 			32

#define START_OF_USR_BUF		2

						
/*
 * Max number of pages supported
 * static max limit to 867
 * keeping commented for history reason.
 */
//#define MAX_KERNEL_BUFFER_AVAIL	200
//#define MAX_KERNEL_BUFFER_AVAIL	290
//#define MAX_KERNEL_BUFFER_AVAIL	867
//#define MAX_KERNEL_BUFFER_AVAIL	578
//27 region for L2 with 32 buffers each 4K
//#define MAX_KERNEL_BUFFER_AVAIL	866
//adding 3 more region for L3 so total will (30*32 + 2 )
//total region 30 ( 27 for L2, 3 for L3 
#define MAX_KERNEL_BUFFER_AVAIL	962

struct cosmdatakva
{
	void *kva[MAX_KERNEL_BUFFER_AVAIL];
};


/*
 * meta data for mmap managemnt 
 */
struct cosmdata 
{
	void *region[COS_MAX_REGION]; /* per region start address */
	unsigned int rsz[COS_MAX_REGION];  /* size per region */
	unsigned int npz[COS_MAX_REGION]; /* num of pages per region */
	void *next_avail[COS_MAX_REGION]; /* next buffer available for allocation */
	unsigned short  next_buf_index[COS_MAX_REGION]; /* alloc next index for user region */
	unsigned short  rtype[COS_MAX_REGION]; /* type of region */

	int npg; /* number of system pages allocated */
	int alloc_sz; /* total size allocated */
	int max_region ; /* max region allowed by library */
	int sys_psz ; /* page size, system */
	int mmap_psz ; /* page size , rr size */
	int kva_st_index; /* kva start index , init to 1 as 0 is for meta data */
	unsigned char buf_state[COS_MAX_REGION][COS_MAX_PAGE_PER_CHIP]; /* state */
	
	/* keeping commented for history reason */
	//unsigned short buf_state[COS_MAX_REGION][COS_MAX_PAGE_PER_CHIP]; /* state */
	//int buf_state[COS_MAX_REGION][COS_MAX_PAGE_AGENT]; /* state */
	/*
	 * kva should point to next page which contains void *kva[MAX_KERNEL_BUFFER_AVAIL];  
	 * as we are starting to support 27 region with 32 buffers per region so metadata 
	 * can not fit in one page, moving out kva to new page 
	 */
	//void *kva[MAX_KERNEL_BUFFER_AVAIL];
	
	void *next_avail_kern[COS_MAX_REGION]; /* next buffer available for allocation */
	unsigned short nbi_kern[COS_MAX_REGION]; /* alloc next index for kernel region */

	unsigned short kvaindex[COS_MAX_REGION]; /* start index for user to kernel, */

	volatile unsigned short  produce_index[COS_MAX_REGION]; /* alloc, data, avail for consumer - producer->consumer */
	volatile unsigned short  consume_index[COS_MAX_REGION]; /* alloc, data, avail for consumer - producer->consumer */

	volatile unsigned short  modify_index[COS_MAX_REGION]; /* alloc by producer, modify by consumer, avail */
	volatile unsigned short  modify_consume_index[COS_MAX_REGION]; /*  */

	unsigned short wrfree[COS_MAX_REGION];
	unsigned short nfree[COS_MAX_REGION];
	unsigned int wrsfree[COS_MAX_REGION];

	void *avail ; /* mmap_region time next avail */
} ;


/*
 * common functions
 */
int _cos_mdata_index_to_buf(int region, int index, char **buf) ;
int _cos_mdata_index(int region, char *buf, int *index, int validate) ;
int _cos_validate_input(int region) ;
int _cos_mmap_get_buf( int region, char **buf);
int _cos_mmap_put_buf(int region, char *buf, int type, int validate);
/*
 * Common ends here 
 */


#ifdef COS_MMAP_USER
extern int errno ;
/*
 * character device name
 */
#define DFILENAME "/dev/cosmmap"
/*
 * user space specific functions.
 */

#define MKNODFLAG S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH | S_IFCHR
/*
 * following deviceid will convert to (232,0) (major, minor)
 */
//#define MKNODDEVID 0x0000e800
#define MKNODDEVID 0x00007200

int cos_mmap_spgz( void );
int cos_mmap_region(int region, int sz, int region_type) ;
int cos_mmap_retrieve_buf( int region, char **buf);
int _cos_mmap_alloc_buf_usr(int region, char **buf);
int _cos_mmap_retrieve_buf( int region, char **buf);
#else
/*
 * kernel specific functions.
 */
int _cos_mmap_alloc_buf_kern(int region, char **buf) ;
int _cos_mdata_index_drv(int region, char *buf, int *index) ;
int _cos_mmap_return_buf(int region, char *buf);
#endif



#endif

