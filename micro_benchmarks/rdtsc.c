/* Measure overhead for readtsc/readtscp                                 *
 * Compile (O2 ensures inline):                                          *
 *    gcc -g -Wall -lrt -O2 -o readtsc readtsc.c                         *
 * Running:                                                              *
 *    Use "isolcpu=0" to reserve a core (cpuid_zero_based)               *
 *    Use "taskset -c 1 ./readtsc" to pin process to core (core_bitmask) *
 */
#include <stdint.h>                /* uint32_t */
#include <stdio.h>                 /* printf */
#include <stdlib.h>                /* exit(3) */
#include <time.h>                  /* timespec, clock_gettime */
#include <sys/time.h>

#include "atomic.h"                /* unlocked_cmpxchg, locked_cmpxchg */
#include "time_api.h"              /* rdtsc() */

#define LOOPS BILLION
#define CPU   1
uint64_t dummy_data;

/* Create separate test-cases and avoid a function pointer         *
 * If function pointer was used, then rdtsc cannot be inlined      *
 * resulting in a penality of atleast 2 cycles (2 cycles on 3.5Ghz)*/
uint64_t measure_rdtsc(uint64_t loops)
{
	struct time_api_t time;
	struct tsc_api_t tsc;
	uint64_t i;
	uint64_t res;

	/* Prime TLB/Cache before going to the actual loop */
	res = rdtsc();

	rt_measure_start(&time, true);
	tsc_measure_start(&tsc);
	
	for (i = 0; i < loops; i++) {
		res += rdtsc();
	}
	
	tsc_measure_end(&tsc);
	rt_measure_end(&time, true);
	time_print_api(&time, "   Info: rdtsc");
	rt_task_statistics(i, &time, &tsc);
	
	dummy_data = res;   /* Avoid "set but not used" warning */
	return i;
}

uint64_t measure_rdtscp(uint64_t loops)
{
	struct time_api_t time;
	struct tsc_api_t tsc;
	uint64_t i;
	uint64_t res;

	/* Prime TLB/Cache before going to the actual loop */
	res = rdtscp(CPU);

	rt_measure_start(&time, true);
	tsc_measure_start(&tsc);
	
	for (i = 0; i < loops; i++) {
		res += rdtscp(CPU);
	}
	
	tsc_measure_end(&tsc);
	rt_measure_end(&time, true);
	time_print_api(&time, "   Info: rdtsc");
	rt_task_statistics(i, &time, &tsc);
	
	dummy_data = res;   /* Avoid "set but not used" warning */
	return i;
}

int main()
{
	printf("\nMeasuring time taken for %llu loops of rdtsc():\n", LOOPS);
	measure_rdtsc(LOOPS);

	printf("\nMeasuring time taken for %llu loops of rdtscp():\n", LOOPS);
	measure_rdtscp(LOOPS);
	return 0;
}

