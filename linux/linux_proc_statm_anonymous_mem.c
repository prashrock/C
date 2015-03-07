//gcc -g -o linux_proc_statm_anonymous_mem linux_proc_statm_anonymous_mem.c
#include <sys/mman.h>    /* mmap, protection flags, map flags */
#include <unistd.h>      /* sleep(), getpid() */
#include <stdio.h>       /* printf() */
//#include <stdint.h>

#define TOTAL_MEM_IN_MB (100)  //100 MiB
#define PAGE_SIZE (4096)       //4KB

/* Use /proc/self/statm to see how memory usage varies at each stage          *
 * On Linux, a shared anonymous map is actually file-based. The kernel        *
 * creates a file in a tmpfs (instance of /dev/zero). The file is immediately *
 * unlinked so it cannot be accessed by any other processes unless they       *
 * inherited the map (via fork()). Look at the statistical_tools notes for    *
 * more information                                                           */
int main()
{
	int i;
	int num_pages = (TOTAL_MEM_IN_MB << 20) / PAGE_SIZE;
	char str[100];
	snprintf(str, 100, "cat /proc/%d/statm", getpid());
	
	printf("When process starts:\n");
	system(str);
	
	printf("Press return to start mmap of %d MB\n", TOTAL_MEM_IN_MB);
	getchar();
	
	/* 1) mmap 100 MiB of Shared Anonymous Memory (Check CS_Linux_process notes) */
	char *p = mmap(NULL, TOTAL_MEM_IN_MB << 20, PROT_READ | PROT_WRITE,
				   MAP_ANONYMOUS | MAP_SHARED, -1, 0);

	printf("mmap() done @ VA %p. Total Program size should have increased by %d pages by now.\n", p, num_pages);
	system(str);
	printf("Press enter/return to write to each page\n");
	getchar();
	
    /* 2) Touch every single page to make them resident */
	for (i = 0; i < num_pages; i++)
		p[i * PAGE_SIZE] = 1;

	printf("Dirtied %d pages. Resident Pages and Shared pages should have increased by %d pages now.", num_pages, num_pages);
	printf("Press enter/return to exit:\n");
	system(str);
	getchar();

	return 0;
}
