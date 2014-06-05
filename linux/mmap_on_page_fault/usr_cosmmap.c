/* 
 * Copyright (c) 2010 by Brocade Communications Systems, Inc.
 * All rights reserved.
 *
 * Module name: usr_cosmmap.c
 *
 * Description:
 * mmap library routines.
 * mmap library which shared pages with l2sys/asic driver
 * currently record size is one page. copy of code will be
 * compiled in user space.
 * kernel code is compiled without COS_MMAP_USER
 * function names are same across user and kernel space.
 * ts=4
 * $Author: L3 Team
 * $Company: Brocade Communications Systems Inc
 * $History: Initial version by Anmol
 */

//#define COS_MMAP_USER
#include "usr_cosmmap.h"
#define MMAPLIB_DEBUG	0
#ifdef COS_MMAP_USER
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <asm/ioctls.h>
int cosfd = -1 ;
void *cosstart = (void *)NULL;
extern int errno;
#else
#include <linux/kernel.h>
#include <linux/stddef.h>
#include <linux/module.h>
#endif
struct cosmdata *cosmdata = NULL;
struct cosmdatakva *cosmdatakva = NULL;

#ifdef __KERNEL__
int _cos_mdata_index_drv(int region, char *buf, int *index)
{
	int li ;
	for(li  = cosmdata->kvaindex[region-1] ;
		 li < (cosmdata->kvaindex[region-1] + cosmdata->npz[region-1]);
		 li++)
	{
		if((char *)cosmdatakva->kva[li] != buf )
			continue;
		*index = (li - cosmdata->kvaindex[region-1]);

		if(MMAPLIB_DEBUG)
			printk( KERN_INFO "%s %d) index(%d) li(%d) kvaindex(%d) npz(%d) \n",
			 __FUNCTION__, __LINE__, *index, li, cosmdata->kvaindex[region-1],cosmdata->npz[region-1]); 
		return COSMMAPSUC ;
	}
	return COSMMAPERR;
}
#endif

int _cos_mdata_index_to_buf(int region, int index, char **buf)
{
#ifdef __KERNEL__
	/*
	 * driver is calling to get user buffer
	 */ 
	if(cosmdata->rtype[region-1] == COSMMAP_APP_REGION)
	{
		*buf = (char *) cosmdatakva->kva[cosmdata->kvaindex[region-1]+index];
		//printk( KERN_INFO "%s %d) index(%d) kva_index(%d) buf (%p) \n",
		//	__FUNCTION__, __LINE__, index, cosmdata->kvaindex[region-1], *buf );
		cosmdata->buf_state[region-1][index] = COS_BUF_DRV; 
		return COSMMAPSUC ;

	}
	else
		return COSMMAPERR;
	
#else
	if(cosmdata->rtype[region-1] == COSMMAP_DRV_REGION)
	{
		*buf = (char *)cosmdata->region[region-1]+(cosmdata->mmap_psz*index);
		cosmdata->buf_state[region-1][index] = COS_BUF_APP; 
		return COSMMAPSUC ;

	}
	else
		return COSMMAPERR;
#endif
		return COSMMAPERR;
		
}

int _cos_mdata_index(int region, char *buf, int *index, int validate)
{

	if(validate)
		if(_cos_validate_input(region) != COSMMAPSUC)
			return COSMMAPERR;

	if(buf < (char *) cosmdata->region[region-1])
		return COSMMAPERR;

	if( buf > ((char *) cosmdata->region[region-1] + cosmdata->rsz[region-1] ) )
	{
		#if(MMAPLIB_DEBUG)
			#ifdef __KERNEL__
			printk( KERN_INFO "%s) %d) buf(%x) region_num(%d) region(%x) sz (%d)\n", 
				__FUNCTION__, __LINE__, buf,  region-1, 
			 	(char *) cosmdata->region[region-1], cosmdata->rsz[region-1] );
			#else
			printf( "%s) %d) buf(%p) region_num(%d) region(%p) sz (%d)\n", 
				__FUNCTION__, __LINE__, buf,  region-1, 
			 	(char *) cosmdata->region[region-1], cosmdata->rsz[region-1] );
			#endif
		#endif
		return COSMMAPERR;
	}

	/*
 	 * validate right boundary (start of page/rr)
	 */
	if( ((buf - (char *)cosmdata->region[region-1])%cosmdata->mmap_psz) != 0 )
		return COSMMAPERR;

	*index = (buf - (char *)cosmdata->region[region-1])/cosmdata->mmap_psz ;

	/*
 	 * index can not be more  than number of allocated pages
	 */
	if(*index >= cosmdata->npz[region-1]) 
		return COSMMAPERR ;

	return COSMMAPSUC ;
}

/*
 *  resource record size
 */
int cos_mmap_rrsz( void )
{
	if(cosmdata != NULL)
		return cosmdata->mmap_psz;
	return COSMMAPERR;
}
#if defined(__KERNEL__)
EXPORT_SYMBOL(cos_mmap_rrsz) ;
#endif

int cos_mmap_rsize( void )
{
	return COS_RR_SZ ;
}
#if defined(__KERNEL__)
EXPORT_SYMBOL(cos_mmap_rsize) ;
#endif

int cos_mmap_ready( void )
{
	if(cosmdata != NULL)
		return COSMMAPSUC;
	return COSMMAPERR;
}
#if defined(__KERNEL__)
EXPORT_SYMBOL(cos_mmap_ready) ;
#endif

#ifdef COS_MMAP_USER
int cos_mmap_spgz( void )
{
	return (int)sysconf(_SC_PAGESIZE);
}

int cos_mmap_pgz( void ) 
{
	return (int)sysconf(_SC_PAGESIZE);
}

/*
 *  unmap memory
 */

int cos_mmap_fini( void )
{
	if(cosstart && cosmdata) 
	{
		munmap(cosstart, cosmdata->alloc_sz);
		return COSMMAPSUC ;
	}
	return COSMMAPERR;
}
/*
 * mmap library init code
 * this code will perform
 * - open char device file, if not it will create and open
 * - get system page size and allocate number of pages based 
 *   on size provided by user.
 * - causes memory page fault by writing to start of each page
 * - which make driver to provide mmaped pages to user space
 *   and kva are copied to meta data.
 * - first page is dedicated  for meta data
 * - perform init required by library
 */ 

int cos_mmap_init(int sz)
{
	int psz ;
	int npg ;
	int lindex, ret ;

	cosfd = open(DFILENAME, O_RDWR );
	if((cosfd == -1) && (errno == ENOENT))
	{
		ret = mknod(DFILENAME, MKNODFLAG,MKNODDEVID);
		if(ret != 0 )
			return COSMMAPERR ;
		else
			cosfd = open(DFILENAME, O_RDWR );
	}
	
	if(cosfd != -1)
	{
		psz = (int ) sysconf(_SC_PAGESIZE);
		npg = (sz/psz)+START_OF_USR_BUF;
		/*printf("MMAP INIT SZ PARAMETER (%d) psz (%d) npg (%d) cosmdata sz (%d) \n", sz, psz, npg, sizeof( struct cosmdata) );*/
		cosstart = mmap(0, psz*npg, PROT_WRITE | PROT_READ, MAP_SHARED, cosfd, 0);
		if(cosstart != NULL)
		{
		#if(MMAPLIB_DEBUG)
			printf("start of region (%p) sizemmaped(%d) \n", cosstart, psz*npg );
		#endif
			/*
			 * lets create page fault so all reserved pages
			 * are in memory and no more page fault is 
			 * happening in kernel cos mmap code.
			 */
			for(lindex=0; lindex<npg;lindex++)
				*((char *)cosstart+(lindex*psz)) = 0 ; 

			cosmdata = (struct cosmdata *)cosstart;
			/*
			 * lets assign kva to support 27 region with 32 buffer per region 
			 * now kva is moved to second page , metadata consist of 2 pages
			 * first page contains metadata except kva and kva is kept in 
			 * second page. 
			 */
			cosmdatakva = (struct cosmdatakva *)((char *) cosstart + psz );
			cosmdata->avail = (void *) ((( char *)cosstart)+(psz*START_OF_USR_BUF));
			#if(MMAPLIB_DEBUG)
				printf("%s) cosmdatakva (%p) cosmdata (%p) cosstart (%p) cosmdata->avail (%p) \n",
						__FUNCTION__, cosmdatakva, cosmdata, cosstart, cosmdata->avail );
			#endif

			cosmdata->npg = (npg);
			cosmdata->sys_psz = psz ;
			cosmdata->mmap_psz = psz ;
			cosmdata->alloc_sz =  sz ;
			cosmdata->max_region = COS_MAX_REGION;
			cosmdata->kva_st_index = START_OF_USR_BUF ; /* first 2 page for meta data */
			for(lindex=0; lindex<COS_MAX_REGION;lindex++) {
				cosmdata->region[lindex] = (void *) NULL;
				cosmdata->wrfree[lindex] =  0;
				cosmdata->nfree[lindex]= 0;
				cosmdata->wrsfree[lindex] =  0;

				cosmdata->produce_index[lindex] =  0;
				cosmdata->consume_index[lindex] =  0;
				cosmdata->modify_index[lindex] =  0;
				cosmdata->modify_consume_index[lindex] =  0;
		#if(MMAPLIB_DEBUG)
					printf("%s) region (%d) <%p> \n", 
						 __FUNCTION__, lindex,  cosmdata->region[lindex] );
		#endif
			}
			//cosmdata->smd_flag = 1; /* default debugging is on */
			//for(lindex=0; lindex<MAX_KERNEL_BUFFER_AVAIL; lindex++)
			//{
			//	printf("%s) %d) index (%d) kva (%x) \n", __FUNCTION__, __LINE__, lindex, cosmdatakva->kva[lindex]);
			//}
			//printf("%s) %d) index (%d) kva0-1 (%x) <%x> <%x> <%x> <%x> <%x> <%x> <%x> \n", __FUNCTION__, __LINE__, lindex, cosmdata->kva[0], cosmdata->kva[1], cosmdata->kva[2], cosmdata->kva[50], cosmdata->kva[98], cosmdata->kva[146] , cosmdata->kva[194], cosmdata->kva[242]);  
			return COSMMAPSUC ;
		}
	
	}
	return  COSMMAPERR ;
}
/*
 * map region from mmaped memory
 * - user can allocate max of 12 regions
 * - provide size which translate to pages 
 *   should be multiple of pages.
 * - who owns this region ( APP/DRV)
 * - peform actions, fill data required for region
 */
int cos_mmap_region(int region, int sz, int region_type)
{
	int npg ;
	int i ;

//	printf("%s) %d) region(%d) sz (%d) type (%d)\n", __FUNCTION__, __LINE__, region, sz, region_type  );

	if(region > COS_MAX_REGION || region < 1 )
		return COSMMAPERR ;

	if((region_type < COSMMAP_DRV_REGION) ||
		(region_type > COSMMAP_APP_REGION))
		return COSMMAPERR ;

	if(cosstart != ( void *)NULL) 
	{
		/*
		 * size should be multiple of system page size
		 */ 
		if((sz%cosmdata->sys_psz) != 0 )
			return COSMMAPERR ;

		if( sz > (cosmdata->sys_psz*COS_MAX_PAGE_AGENT))
			return COSMMAPERR ;


		if(cosmdata == NULL)
			return COSMMAPERR ;

		/*
		 * calculate number of system pages.
		 */
		npg = sz / cosmdata->sys_psz;

		#if(MMAPLIB_DEBUG)
			printf("%s) region(%d) addr <%p> \n", 
		 		__FUNCTION__, region-1, cosmdata->region[region-1] );
		#endif
		
		if(cosmdata->region[region-1] != (void *) NULL)
			return COSMMAPREGIONERR ;

		cosmdata->region[region-1] =  cosmdata->avail ;
		cosmdata->rsz[region-1] =  sz;
		/*
		 * number of system pages allocated
		 */
		cosmdata->npz[region - 1] = npg;
		cosmdata->avail = ((char *) (cosmdata->avail )) + (npg*cosmdata->sys_psz) ;
		cosmdata->rtype[region-1] = region_type ;
		/*
		 * start of page for user/drv applications
		 */
		cosmdata->next_avail[region-1] = cosmdata->region[region-1];
		cosmdata->next_buf_index[region-1] = 0 ;

		
		/*
		 * Mark buffer free/invalid
		 */
		for(i=0; i < COS_MAX_PAGE_PER_CHIP; i++)
		{
			if(i<npg) {
				cosmdata->buf_state[region-1][i] = COS_BUF_FREE; 
				
			}
			else
				cosmdata->buf_state[region-1][i] = COS_BUF_INVALID; 
		}
		/*
		 * next kernel address/index available correspond to uva
		 */
		//cosmdata->next_avail_kern[region-1] = cosmdata->kva[cosmdata->kva_st_index];
		cosmdata->next_avail_kern[region-1] = cosmdatakva->kva[cosmdata->kva_st_index];
		cosmdata->nbi_kern[region-1] = cosmdata->kva_st_index;

		/*
		 * add kva start index in meta data 
		 */
		cosmdata->kvaindex[region-1] = cosmdata->kva_st_index ;
		cosmdata->kva_st_index += npg;

		//printf("%s) %d) region(%d) kvaindex(%d) kva (%x) npg (%d) \n", __FUNCTION__, __LINE__, region-1, cosmdata->kvaindex[region-1], cosmdatakva->kva[ cosmdata->kvaindex[region-1]], npg);
		//printf("%s) %d) region(%d) kvaindex(%d) kva (%x) npg (%d) \n", __FUNCTION__, __LINE__, region-1, cosmdata->kvaindex[region-1], cosmdata->kva[ cosmdata->kvaindex[region-1]], npg);
	

		return COSMMAPSUC;
		
	}
//	printf("%s) %d)  returning error \n", __FUNCTION__, __LINE__ );
	return COSMMAPERR ;
		
}
#endif

/*
 * code for region validation
 */
int _cos_validate_input(int region)
{
	if(cosmdata == NULL)
		return COSMMAPERR ;

	if( region > cosmdata->max_region)
		return COSMMAPERR ;

	if(cosmdata->region[region-1] == (void *)NULL)
		return COSMMAPERR;

	return COSMMAPSUC;

}

/*
 * buffer allocation API for user/driver
 */
int cos_mmap_alloc_buf(int region, char **buf)
{
#ifdef COS_MMAP_USER
	return _cos_mmap_alloc_buf_usr(region, buf);
#else
	return _cos_mmap_alloc_buf_kern(region, buf);
#endif
}
#ifdef __KERNEL__
EXPORT_SYMBOL(cos_mmap_alloc_buf) ;
#endif

#ifdef __KERNEL__
/*
 * buffer allocation ( for library only)
 */
int _cos_mmap_alloc_buf_kern(int region, char **buf)
{
	int li ;
	if(_cos_validate_input(region) != COSMMAPSUC)
	{
		return COSMMAPERR;
	}

	if(cosmdata->rtype[region-1] != COSMMAP_DRV_REGION)
	{
		return COSMMAPINVALRTYPE ;
	}
	
	li = cosmdata->nbi_kern[region-1]-cosmdata->kvaindex[region-1] ;

	if(MMAPLIB_DEBUG)
		printk(KERN_INFO "%s) %d) li(%d) bufstate (%d) region(%d) nbi_kern{%d), kvaindex(%d) \n",
			__FUNCTION__, __LINE__, li, cosmdata->buf_state[region-1][li], region-1,
		 	cosmdata->nbi_kern[region-1], cosmdata->kvaindex[region-1] );

	if( cosmdata->buf_state[region-1][li] != COS_BUF_FREE )
	{
		return COSMMAP_ALLOC_FAIL ;
	}
 
	*buf = (char *)cosmdata->next_avail_kern[region-1] ;
	cosmdata->buf_state[region-1][li] = COS_BUF_DRV;

	if(li >= (cosmdata->npz[region - 1] - 1))  /* validate */
		cosmdata->nbi_kern[region-1] = cosmdata->kvaindex[region-1];
	else
		cosmdata->nbi_kern[region-1]++;

	cosmdata->next_avail_kern[region-1]  = cosmdatakva->kva[cosmdata->nbi_kern[region-1]];

	return COSMMAPSUC ;
	
}
#endif
#ifdef COS_MMAP_USER
/*
 * buffer allocation for library use
 */
int _cos_mmap_alloc_buf_usr(int region, char **buf)
{
	if(_cos_validate_input(region) != COSMMAPSUC)
	{
		return COSMMAPERR;
	}

	if(cosmdata->rtype[region-1] != COSMMAP_APP_REGION)
	{
		return COSMMAPINVALRTYPE ;
	}

	#if(MMAPLIB_DEBUG) 
		printf("%s) %d) region(%d) next_buf_index(%d) nxt_avail(%p) rrsz(%d) \n",
		__FUNCTION__, __LINE__, region-1, cosmdata->next_buf_index[region-1],
		(char *)cosmdata->next_avail[region-1], cosmdata->mmap_psz ) ; 
	#endif
	
	if( cosmdata->buf_state[region-1][cosmdata->next_buf_index[region-1]] != COS_BUF_FREE )
			return COSMMAP_ALLOC_FAIL ;
 
	*buf = (char *)cosmdata->next_avail[region-1] ;
	cosmdata->buf_state[region-1][cosmdata->next_buf_index[region-1]] = COS_BUF_APP;

	if(cosmdata->next_buf_index[region-1] >= (cosmdata->npz[region - 1] -1) )
		cosmdata->next_buf_index[region-1] = 0;
	else
		cosmdata->next_buf_index[region-1]++;

	#if(MMAPLIB_DEBUG)
		 printf("%s) %d) region(%d) next_buf_index(%d) nxt_avail(%p) rrsz(%d) \n",
		__FUNCTION__, __LINE__, region-1, cosmdata->next_buf_index[region-1],
		(char *)cosmdata->next_avail[region-1], cosmdata->mmap_psz ); 
	#endif


	 cosmdata->next_avail[region-1]  = ( void *)(((char *)cosmdata->region[region-1]) \
						 + (cosmdata->next_buf_index[region-1] * cosmdata->mmap_psz));
	#if(MMAPLIB_DEBUG)
		 printf("%s) %d) region(%d) next_buf_index(%d) nxt_avail(%p) rrsz(%d) \n",
		__FUNCTION__, __LINE__, region-1, cosmdata->next_buf_index[region-1],
		(char *)cosmdata->next_avail[region-1], cosmdata->mmap_psz ); 
	#endif


	return COSMMAPSUC ;
}
#endif

/*
 * API to check if buffer is available
 * from particular region.
 * region should pre-created by user space
 */
int cos_mmap_get_buf( int region, char **buf)
{
	if(_cos_validate_input(region) != COSMMAPSUC)
	{
#if defined(__KERNEL__)
	printk(KERN_INFO "%s) %d) region (%d) \n", __FUNCTION__, __LINE__, region );
#endif
		return COSMMAPERR;
	}
	return _cos_mmap_get_buf(region, buf);
}
#if defined(__KERNEL__)
EXPORT_SYMBOL(cos_mmap_get_buf) ;
#endif
/*
 * library code for internal use
 */
int _cos_mmap_get_buf( int region, char **buf)
{
	int pindex = cosmdata->consume_index[region-1] ;

	if(pindex > cosmdata->npz[region-1] -1)
	{
#if 0
	printk(KERN_INFO "%s) %d) region (%d) consume_index(%d) \n", __FUNCTION__, __LINE__, region-1, pindex );
#endif
		return COSMMAPERR;
	}

#if defined(__KERNEL__)
	if(cosmdata->buf_state[region-1][pindex] == COS_BUF_DRV_AVAIL)
#else
	if(cosmdata->buf_state[region-1][pindex] == COS_BUF_APP_AVAIL)
#endif
	{
		#if(MMAPLIB_DEBUG)
			#ifdef __KERNEL__
			 printk(KERN_INFO  "%s) %d) produce_index(%d) region (%d) consume_index(%d)  \n" 
			, __FUNCTION__, __LINE__,
			 cosmdata->produce_index[region-1], region-1, cosmdata->consume_index[region-1]);
			#else
			 printf("%s) %d) produce_index(%d) region (%d) consume_index(%d)  \n" 
			, __FUNCTION__, __LINE__,
			 cosmdata->produce_index[region-1], region-1, cosmdata->consume_index[region-1]);
			#endif
		#endif

		if(_cos_mdata_index_to_buf(region, cosmdata->consume_index[region-1], buf) != COSMMAPERR)
		{
	
			#if(MMAPLIB_DEBUG)
				#ifdef __KERNEL__
				 printk(KERN_INFO  "%s) %d) consume_index(%d) region (%d) buf (%p)  \n", __FUNCTION__,  \
				 __LINE__, cosmdata->consume_index[region-1], region-1, *buf);
				#else
				 printf("%s) %d) consume_index(%d) region (%d) buf (%p)  \n", __FUNCTION__,  \
				 __LINE__, cosmdata->consume_index[region-1],\
 				region-1,  *buf);
				#endif
			#endif
			
			cosmdata->consume_index[region-1]++;
			if(cosmdata->consume_index[region-1] > (cosmdata->npz[region-1] -1 ))
				cosmdata->consume_index[region-1] = 0;
			
			return COSMMAPSUC ;
		}
	}
	return COSMMAPNODATA;
}

/*
 * alloced buffers are marked ready by producer
 * and consumer can access these buffers.
 */

int cos_mmap_put_buf(int region, char *buf)
{
	if(_cos_validate_input(region) != COSMMAPSUC)
	{
		return COSMMAPERR;
	}

#ifdef COS_MMAP_USER
	return _cos_mmap_put_buf(region, buf, COS_BUF_APP, 0);		
#else
	return _cos_mmap_put_buf(region, buf, COS_BUF_DRV, 0);		
#endif
}
#if defined(__KERNEL__)
EXPORT_SYMBOL(cos_mmap_put_buf) ;
#endif


/*
 * library code for inernal use
 */
int _cos_mmap_put_buf(int region, char *buf, int type, int validate)
{
	int index = 0;
#ifdef COS_MMAP_USER
	if(_cos_mdata_index(region, buf, &index, validate) != COSMMAPSUC)
	{
		return COSMMAPERR ;
	}
#else
	if(_cos_mdata_index_drv(region, buf, &index) != COSMMAPSUC) 
	{
		#if(MMAPLIB_DEBUG)
			 printk(KERN_INFO "%s %d) region(%d) \n", 
				 __FUNCTION__, __LINE__, region-1 );
		return COSMMAPERR ;
		#endif
	}
#endif
	
	cosmdata->produce_index[region-1]++;
	if( cosmdata->produce_index[region-1] > (cosmdata->npz[region-1] -1 ))
		cosmdata->produce_index[region-1] = 0;

	if(type == COS_BUF_APP)
	{
		if( cosmdata->buf_state[region-1][index] == COS_BUF_APP)
		{
			cosmdata->buf_state[region-1][index] = COS_BUF_DRV_AVAIL ;
		}
		else
		{
			return COSMMAPWRNGBSTATE;
		}
	}
	if(type == COS_BUF_DRV)
	{
		if( cosmdata->buf_state[region-1][index] == COS_BUF_DRV) 
		{

			#if(MMAPLIB_DEBUG) 
				#ifdef __KERNEL__
				printk(KERN_INFO "%s) %d) index (%d) \n", 
				__FUNCTION__, __LINE__, index );
				#else
				printf("%s) %d) index (%d) \n", 
				__FUNCTION__, __LINE__, index );
				#endif
			#endif

			cosmdata->buf_state[region-1][index] = COS_BUF_APP_AVAIL ;
		}
		else
		{
			return COSMMAPWRNGBSTATE;
		}
	}
	return COSMMAPSUC;
}



#ifdef COS_MMAP_USER
/*
 * library code for inernal use
 */
int _cos_mmap_retrieve_buf( int region, char **buf)
{
	int index = cosmdata->modify_consume_index[region-1];

	if( index > cosmdata->npz[region-1] -1 )
		return COSMMAPERR;

	if(	cosmdata->buf_state[region-1][index] == COS_BUF_APP_AVAIL)
	{
		*buf = (char *)cosmdata->region[region-1]+(index*cosmdata->mmap_psz);

		cosmdata->buf_state[region-1][index] = COS_BUF_APP_RTRVD ;

		cosmdata->modify_consume_index[region-1]++;
		if(cosmdata->modify_consume_index[region-1] > (cosmdata->npz[region-1] -1))
			cosmdata->modify_consume_index[region-1] = 0;
		return COSMMAPSUC;
		
	}
	return COSMMAPNODATA;
}

/*
 * API to check if driver has returned 
 * status for previously allocated buffer
 * from user space.
 * driver works on buffer and then return 
 * with information 
 */
int cos_mmap_retrieve_buf( int region, char **buf)
{
	if(_cos_validate_input(region) != COSMMAPSUC)
	{
		return COSMMAPERR;
	}
	if(cosmdata->rtype[region-1] != COSMMAP_APP_REGION)
	{
		return COSMMAPERR;
	}
	return  _cos_mmap_retrieve_buf( region, buf);
	
}

#else

/*
 * API for driver code to return buffer with
 * status.
 */
int cos_mmap_return_buf(int region, char *buf)
{
	if(_cos_validate_input(region) != COSMMAPSUC)
	{
		return COSMMAPERR;
	}

	#if(MMAPLIB_DEBUG)
		 printk(KERN_INFO "%s %d) region(%d) rtype(%d) \n",
	 		__FUNCTION__, __LINE__, region-1, cosmdata->rtype[region-1] );
	#endif

	if(cosmdata->rtype[region-1] != COSMMAP_APP_REGION)
	{
		return COSMMAPERR;
	}

	#if(MMAPLIB_DEBUG)
		printk(KERN_INFO "%s %d) region(%d) \n",  
		__FUNCTION__, __LINE__, region-1 );
	#endif

	return _cos_mmap_return_buf(region, buf);
	
}
#if defined(__KERNEL__)
EXPORT_SYMBOL(cos_mmap_return_buf) ;
#endif
/*
 * internal use only
 */

int _cos_mmap_return_buf(int region, char *buf)
{
	int index = 0;

	#if(MMAPLIB_DEBUG)
		 printk(KERN_INFO "%s %d) region(%d) \n",
 			__FUNCTION__, __LINE__, region-1 );
	#endif

	if(_cos_mdata_index_drv(region, buf, &index) != COSMMAPSUC) 
	{
		#if(MMAPLIB_DEBUG)
			 printk(KERN_INFO "%s %d) region(%d) \n",
 				__FUNCTION__, __LINE__, region-1 );
		#endif
		return COSMMAPERR ;
	}

	if(index != cosmdata->modify_index[region-1])
	{
		 printk(KERN_INFO "%s %d) region(%d) index(%d) expected modify_index(%d) \n",
 					__FUNCTION__, __LINE__, region-1, index, cosmdata->modify_index[region-1] );
	}

	#if(MMAPLIB_DEBUG)
		 printk(KERN_INFO "%s %d) region(%d) index(%d) \n",
 			__FUNCTION__, __LINE__, region-1, index );
	#endif

	if(cosmdata->buf_state[region-1][index] != COS_BUF_DRV) 
	{
		 printk(KERN_INFO "%s %d) region(%d) index(%d) expected state COS_BUF_DRV actual(%d) \n",
 					__FUNCTION__, __LINE__, region-1, index, cosmdata->buf_state[region-1][index]);

		#if(MMAPLIB_DEBUG)
			 printk(KERN_INFO "%s %d) region(%d) \n",
 				__FUNCTION__, __LINE__, region-1 );
		#endif
		return COSMMAPERR ;
	}

	cosmdata->buf_state[region-1][index] = COS_BUF_APP_AVAIL;
	cosmdata->modify_index[region-1]++;

	if(cosmdata->modify_index[region-1] > (cosmdata->npz[region-1] -1))
		cosmdata->modify_index[region-1] = 0;

	#if(MMAPLIB_DEBUG)
		 printk(KERN_INFO "%s %d) region(%d) modify_index(%d) num_modify(%d) \n",
	 			__FUNCTION__, __LINE__, region-1, cosmdata->modify_index[region-1], 
	 			cosmdata->num_modified[region-1] ); 
	#endif

	return COSMMAPSUC;
}

#endif

/*
 * free buffer - available for user space  
 */

int cos_mmap_free_buf(int region, char *buf)
{

#ifdef __KERNEL__
	return COSMMAPERR ;
#else
	int index ;

	if(_cos_validate_input(region) != COSMMAPSUC) 
		return COSMMAPERR;

	if(_cos_mdata_index(region, buf, &index, 0) != COSMMAPSUC) 
	{
		return COSMMAPERR ;
	}

	if(index != cosmdata->nfree[region-1])
	{
		printf("cos_mmap_free err(region(%d) expected nfree(%d), freeing(%d) buf(%p)\n",
					region-1, cosmdata->nfree[region-1], index, buf);
		cosmdata->wrfree[region-1]++;
		return COSMMAPFERR;
	}
	else
	{
		if(index >= (cosmdata->npz[region-1] -1))
			cosmdata->nfree[region-1] = 0;
		else
			cosmdata->nfree[region-1]++;
	}
	if( (cosmdata->buf_state[region-1][index] == COS_BUF_APP) ||
		(cosmdata->buf_state[region-1][index] == COS_BUF_APP_RTRVD))
	{
		cosmdata->buf_state[region-1][index] = COS_BUF_FREE;
	}
	else
	{
		printf("cos_mmap_free err(region(%d) expected state( COS_BUF_APP or COS_BUF_APPDRV_RCVD), buf state(%d) index(%d) buf(%p)\n",
					region-1, cosmdata->buf_state[region-1][index], index, buf);
		cosmdata->wrsfree[region-1]++;
		return COSMMAPFERR;
	}
#endif

	return COSMMAPSUC;
	
}

#ifdef COS_MMAP_USER
/*
 * agent can pass information to driver that data
 * is available for use, kernel mmap lib will
 * try to call function registered by driver if any.
 * function regiser should perform minimal things
 * as it is blocked.
 */
int cos_mmap_data_ready(int val)
{
	int ret = 0 ;

	/* make region check so right value is passed to driver */

	if(val < 1 || val > COS_MAX_REGION)
		return COSMMAPERR ;

	if(cosfd != -1)
	{
		ret = ioctl(cosfd, SIOCDEVPRIVATE, &val);
		if(ret < 0 )
			return COSMMAPERR ;
		return COSMMAPSUC;
		
		
	}
	return COSMMAPERR ;
}
#endif


int cos_mmap_region_info( int region, struct reg_info *info)
{
	int reg;
	int index ;

	if(_cos_validate_input(region) != COSMMAPSUC)
		return COSMMAPERR;

	if( info == NULL)
		return COSMMAPERR;
	
	reg = region - 1;
	if(info != NULL)
	{
		info->num_use = 0;
		info->wrfree = cosmdata->wrfree[reg];
		info->wrsfree = cosmdata->wrsfree[reg];

		if(cosmdata->rtype[reg] != COSMMAP_DRV_REGION)
			info->next_buf_index = cosmdata->next_buf_index[reg];
		else
			info->next_buf_index = cosmdata->nbi_kern[reg]-cosmdata->kvaindex[reg] ;

		info->wrfree = cosmdata->wrfree[reg];
		info->wrsfree = cosmdata->wrsfree[reg];
		info->mdata = cosmdata ;
		info->mdatakva =  cosmdatakva ;

		info->rtype = cosmdata->rtype[reg];
		info->npg = cosmdata->npz[reg];
		for(index = 0 ; index < info->npg ; index++) 
		{
			info->state[index] = cosmdata->buf_state[reg][index] ;
			if(cosmdata->buf_state[reg][index] != COS_BUF_FREE)
				info->num_use++;
		}
		return COSMMAPSUC;
		
	}
	return COSMMAPERR;
}

#ifdef __KERNEL__
EXPORT_SYMBOL(cos_mmap_region_info) ;
#endif
