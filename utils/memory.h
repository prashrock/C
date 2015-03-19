#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <stdint.h>                /* uint32_t */
#include <sys/mman.h>              /* mmap, protection flags, map flags */
#include <unistd.h>                /* sleep(), NULL  */
#include <stdbool.h>               /* true, false */

#define REGULAR_PAGE_SIZE        (4 * 1024)
#define HUGEPAGE_SIZE            (2 * 1024 * 1024)

/* For explicit control into L1/L2/L3 cacheline, use prefetcht0/1/2 Assembly*/
/* Prefetch memory at virtual address into CPU cache for read               */
static inline void prefetch_for_read(const void *addr)
{
	/* __builtin_prefetch(addr, r/w, locality)                               *
	 * r/w is a compile time constant:                                       *
	 *   0  = Prefetch is preparing for read                                 *
	 *   1  = Prefetch is preparing for write                                *
	 * locality is a compile time constant:                                  *
	 *   0  = Data has no temporal locality. Remove immediately after access *
	 *   1/2= Low/Moderate degree of temporal locality                       *
	 *   3  = High degree of temporal locality (left in all levels of cache) */
	__builtin_prefetch(addr, 0, 3);
}

/* Prefetch memory at virtual address into CPU cache for write               */
static inline void prefetch_for_write(const void *addr)
{
	/* __builtin_prefetch(addr, r/w, locality)                               */
	__builtin_prefetch(addr, 1, 3);
}

/* Calculate number of pages required for a given chunk of memory */
static inline unsigned int calc_num_pages(size_t size, int page_size)
{
	return (size / page_size) + !!(size % page_size);
}

/* A simple wrapper function for mmap() */
static inline void *
mmap_helper(void *addr_hint, size_t len, int *access, int flags,
			int fd, off_t offset, int page_size, bool touch)
{
	int prot, i, alloc_size_mb;
	void *virt_addr;
	
	/* If user has specific access privileges, use it, else use R/W */
	if(access)  prot = *access;
	else		prot = PROT_READ | PROT_WRITE;
	
	virt_addr = mmap(addr_hint, len, prot, flags, fd, offset);

	/* Touch every single page to make them resident */
	if(virt_addr && touch)
	{
		alloc_size_mb = calc_num_pages(len, page_size);
		for (i = 0; i < alloc_size_mb; i++)
			*(int *)(virt_addr + (i * page_size)) = 1;
	}
	return virt_addr;
}

/* Can be used to create an anonymous private memory region           */
static inline void *
create_priv_anon_mem(void *addr_hint, size_t len, int *access,
					 int page_size, bool touch)
{
	int flags;
	/* Set the flags to satisfy anonymous + private map */
	flags = (MAP_ANONYMOUS | MAP_PRIVATE);
	return mmap_helper(addr_hint, len, access, flags, -1, 0,
					   page_size, touch);
}

/* Can be used to create an anonymous private memory region           */
static inline void *
create_shared_anon_mem(void *addr_hint, size_t len, int *access,
					 int page_size, bool touch)
{
	int flags;
	/* Set the flags to satisfy anonymous + shared map */
	flags = (MAP_ANONYMOUS | MAP_SHARED);
	return mmap_helper(addr_hint, len, access, flags, -1, 0,
					   page_size, touch);
}

#endif //_MEMORY_H_
