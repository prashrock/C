/* 
 * Copyright (c) 2010 by Brocade Communications Systems, Inc.
 * All rights reserved.
 *
 * Module name: cosmmap.h
 *
 * Description:
 * mmap library routines.
 */
#ifndef __COS_MMAP_H__
#define __COS_MMAP_H__

#include <linux/ioctl.h>
#include <linux/cdev.h>

//#define COS_MAJOR 232   /* fixed major by default */
#define COS_MAJOR 114   /* fixed major by default */
#define FIB_COS_MAJOR 115   /* fixed major by default */

#define COS_DEVS 1    

/*
 * The bare device is a variable-length region of memory.
 * Use a linked list of indirect blocks.
 *
 * "cos_dev->data" points to an array of pointers, each
 * pointer refers to a memory page.
 *
 */
#define COS_ORDER    0 /* one page at a time */
/* keeping commented for history */
//#define MAX_USER_FREE_BUF	196
//#define MAX_USER_FREE_BUF	288
//#define MAX_USER_FREE_BUF	288
//#define MAX_USER_FREE_BUF	864
// Adding (32*3 buffers for L3)
#define MAX_USER_FREE_BUF	960

#define MAX_META_DATA_BUF	2
#define MAX_FREE_BUF		(MAX_USER_FREE_BUF + MAX_META_DATA_BUF)
#define COS_QSET    MAX_FREE_BUF
#define MAXQSETINIT  COS_QSET
#define KRNL_DUMP_DATA_CNT 3
#define KRNL_MMAP_MDATA_PAGE_OFFSET 0
#define KRNL_MMAP_MDATAKVA_PAGE_OFFSET 1 /* kva is not part of first page */


struct cos_dev {
	void *data[MAXQSETINIT];
	unsigned long  uva[MAXQSETINIT]; /* user virtual address for kva mapping */
	struct cos_dev *next;  /* next listitem */
	int vmas;                 /* active mappings */
	int order;                /* the current allocation order */
	int qset;                 /* the current array size */
	size_t size;              /* 32-bit will suffice */
	struct semaphore sem;     /* Mutual exclusion */
	struct cdev cdev;
};

extern struct cos_dev *cos_devices;

extern struct file_operations cos_fops;

/*
 * The different configurable parameters
 */
extern int cos_major;     /* main.c */
extern int cos_devs;
extern int cos_order;
extern int cos_qset;

/*
 * Prototypes for shared functions
 */
int cos_trim(struct cos_dev *dev);
struct cos_dev *cos_follow(struct cos_dev *dev, int n);

#endif


