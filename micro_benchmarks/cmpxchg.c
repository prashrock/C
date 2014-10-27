/* Measure overhead for locked/unlocked cmpxchg                          *
 * Compile (O2 ensures inline):                                          *
 *    gcc -g -Wall -lrt -O2 -o cmpxchg cmpxchg.c                         *
 * Running:                                                              *
 *    Use "isolcpu=0" to reserve a core (cpuid_zero_based)               *
 *    Use "taskset -c 1 ./cmpxchg" to pin process to core (core_bitmask) *
 */
#include <stdint.h>                /* uint32_t */
#include <stdio.h>                 /* printf */
#include <stdlib.h>                /* exit(3) */
#include <time.h>                  /* timespec, clock_gettime */
#include <sys/time.h>

#include "atomic.h"                /* unlocked_cmpxchg, locked_cmpxchg */
#include "time_api.h"              /* rdtsc() */

#define LOOPS BILLION
uint32_t dummy_data;

/* Create 2 separate test-cases and avoid a function pointer       *
 * If function pointer was used, then cmpxchg cannot be inlined    *
 * resulting in a penality of atleast 2 cycles (2 cycles on 3.5Ghz)*/
uint64_t measure_unlocked_cmpxchg(uint64_t loops)
{
	struct time_api_t time;
	struct tsc_api_t tsc;
	uint64_t i;
	uint32_t data = 1;
	uint32_t res = 0;

	/* Prime TLB/Cache before going to the actual loop */
	res = unlocked_cmpxchg(&data, 1, 1);
	
	rt_measure_start(&time, true);
	tsc_measure_start(&tsc);
	
	for (i = 0; i < loops; i++) {
		res += unlocked_cmpxchg(&data, 1, 1);
	}
	
	tsc_measure_end(&tsc);
	rt_measure_end(&time, true);
	time_print_api(&time, "   Info: Unlocked cmpxchg");
	rt_task_statistics(i, &time, &tsc);
	
	dummy_data = res;   /* Avoid "set but not used" warning */
	return i;
}

uint64_t measure_locked_cmpxchg(uint64_t loops)
{
	struct time_api_t time;
	struct tsc_api_t tsc;
	uint64_t i;
	uint32_t data = 1;
	uint32_t res = 0;

	/* Prime TLB/Cache before going to the actual loop */
	res = locked_cmpxchg(&data, 1, 1);

	rt_measure_start(&time, true);
	tsc_measure_start(&tsc);
	
	for (i = 0; i < loops; i++) {
		res += locked_cmpxchg(&data, 1, 1);
	}
	
	tsc_measure_end(&tsc);
	rt_measure_end(&time, true);
	time_print_api(&time, "   Info: Locked cmpxchg");
	rt_task_statistics(i, &time, &tsc);
	
	dummy_data = res;   /* Avoid "set but not used" warning */
	return i;
}


int main()
{
	printf("\nMeasuring unlocked cmpxchg for %llu loops:\n", LOOPS);
	measure_unlocked_cmpxchg(LOOPS);

	printf("\nMeasuring locked cmpxchg for %llu loops:\n", LOOPS);
	measure_locked_cmpxchg(LOOPS);
	
	return 0;
}
