//gcc -Wall -g -o cpu_test cpu_test.c -lpthread
#define _GNU_SOURCE   /* For CPU_ZERO to be picked up */
#include <stdio.h>
#include <stdint.h>
#include <string.h>   /* strlen */
#include <stdlib.h>   /* strtoull */
#include <inttypes.h> /* PRIx64 */
#include <pthread.h>  /* pthread routines */
#include <sched.h>    /* CPU_ZERO */
#include <time.h>     /* Nanosleep */

#define HEX_BASE 16
#define DEC_BASE 10
#define MAX_NUM_THREADS 64
static pthread_t thread_id[MAX_NUM_THREADS];
static unsigned lp_cnt[MAX_NUM_THREADS];

int pthread_set_core_affinity(unsigned core)
{
	pthread_t thread = pthread_self();
	cpu_set_t csmask;
	CPU_ZERO(&csmask);
	CPU_SET(core, &csmask);

	return pthread_setaffinity_np(thread, sizeof(cpu_set_t), &csmask);
}

void foo(void *arg)
{
	unsigned core_id = (unsigned) arg;
	if(pthread_set_core_affinity(core_id) != 0)
	{
		printf("Slave thread: CPU set affinity failed for core %u(zero-based)\n", core_id);
		return;
	}
	printf("Slave thread: Set to run on core %u(zero-based)\n", core_id);
	while(1)
	{
		lp_cnt[core_id]++;
	}
}

int main(int argc, char *argv[])
{
	unsigned core_id = 0;
	uint64_t bitmap, core_bitmap;
	if (argc != 2)
	{
		printf( "Usage: %s <core_bitmap_in_hex> \n", argv[0]);
		return -1;
	}
	core_bitmap = strtoull(argv[1], NULL, HEX_BASE);
	if(core_bitmap == 0)
	{
		printf("Atleast 1 CPU should be present in the bitmap\n");
		return -1;
	}
	/* Note - should validate that there are only upto 64 CPU ID's specified */
	bitmap = core_bitmap;
	printf("Entered Bitmap = 0x%" PRIX64 "\n", bitmap);
	for(; bitmap > core_id; bitmap ^= (1ULL << core_id))
	{
		core_id = __builtin_ctzll(bitmap);
		pthread_create(&thread_id[core_id], NULL, (void *) &foo, (void *)core_id);
		printf("Main thread: Creating pthread to on core %u(zero-based)\n", core_id);
	}

	/* Block main thread till both child threads are complete */
	bitmap = core_bitmap;
	for(; bitmap > core_id; bitmap ^= (1ULL << core_id))
	{
		core_id = __builtin_ctzll(bitmap);
		pthread_join(thread_id[core_id], NULL);
	}
	
	printf("Main Thread exiting\n");
	return 0;
}
