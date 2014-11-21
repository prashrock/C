#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <stdint.h>                /* uint32_t */
#include <sys/mman.h>              /* mmap, protection flags, map flags */
#include <unistd.h>                /* sleep(), NULL  */
#include <stdbool.h>               /* true, false */

#define REGULAR_PAGE_SIZE        (4 * 1024)
#define HUGEPAGE_SIZE            (2 * 1024 * 1024)

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
