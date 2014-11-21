#include <stdio.h>                 /* printf */
#include <stdlib.h>                /* atol() */
#include "memory.h"                /* create_priv_anon_mem */
#include "scan_utils.h"            /* getch() */

int main(int argc, char **argv)
{
	size_t size;
	void *addr;
	int page_size = REGULAR_PAGE_SIZE;
	if(argc != 2){
		printf("Usage : %s <size_MB>\n", argv[0]);
		return -1;
	}

	size = (size_t) atol(argv[1]);
	size = size << 20;  /* Convert size to MB */
	if(size == 0){
		printf("Error: Size cannot be zero or size too large ??\n");
		return -1;
	}

	printf("%luB size requires %d pages of size %d\n", size,
		   calc_num_pages(size, page_size), page_size);
	printf("Press any key to do the mmap.\n");
	getch();
	
	addr = create_shared_anon_mem(NULL, size, NULL, page_size, true);
	
	printf("Created Private Anonymous memory at virtual address %p\n",
		   addr);
	printf("Press any key to exit.\n");
	getch();

	return 0;
}
