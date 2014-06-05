#ifndef _TIME_API_H_
#define _TIME_API_H_
#include <stdio.h>
#include <stdlib.h>         /* atoi  */
#include <stdbool.h>        /* bool, true, false */
#include <string.h>         /* memset */
#include <assert.h>         /* assert */
#include <limits.h>         /* INT_MAX */
#include <stdint.h>         /* uint64_t */
#include <inttypes.h>       /* PRIu64 */
#include <time.h>           /* timespec, clock_gettime */
#include <errno.h>          /* errno */

#define BILLION (1000000000LL)

struct time_api_t {
	struct timespec start;
	struct timespec end;
	uint64_t time_ns;
};

#define TIME_API_MAX_TIME_UNITS (sizeof(time_unit)/sizeof(char *))
#define TIME_API_SECONDS_UNIT   (3)
static const char *time_unit[] = 
{
	"ns",
	"us",
	"ms",
	"s",
	"mins",
	"hours",
};

/* Translate timespec to ns */
static inline uint64_t timespec_to_ns(struct timespec *val)
{
	return (uint64_t)val->tv_sec * BILLION + (uint64_t)val->tv_nsec;
}	
/* Convert time in ns to bestfit time-unit and return respective string */
static inline const char *convert_tv_ns_to_bestfit_time_unit(uint64_t *tv)
{
	int i = 0;
	/* covers ns to s */
	while((*tv/1000) && (i < TIME_API_SECONDS_UNIT))
	{
		*tv = *tv/1000;
		i++;
	}
	/* covers s to hours */
	while((*tv/60) && (i >= TIME_API_SECONDS_UNIT) && (i != TIME_API_MAX_TIME_UNITS-1))
	{
		*tv = *tv/60;
		i++;
	}
	/* Avoid printing ns twice */
	if(i == 0) return NULL;
	else       return time_unit[i];
}
/* Calculate diff between two timespec values */
static inline void timespec_diff_calc(struct time_api_t *ta)
{
	struct timespec *start = &ta->start;
	struct timespec *end = &ta->end;
	struct timespec diff;
	if ((end->tv_nsec - start->tv_nsec) < 0)
	{
		diff.tv_sec = end->tv_sec - start->tv_sec - 1;
		diff.tv_nsec = BILLION + end->tv_nsec - start->tv_nsec;
	}
	else
	{
		diff.tv_sec = end->tv_sec - start->tv_sec;
		diff.tv_nsec = end->tv_nsec - start->tv_nsec;
	}
	ta->time_ns = timespec_to_ns(&diff);
}
/* Below are the user API's for measuring and printing time */
/* RT = The identifier of the systemwide realtime clock. */
static inline bool rt_measure_start(struct time_api_t *ta, bool print)
{
	if(clock_gettime(CLOCK_REALTIME, &ta->start) != 0)
	{
		if(print)
			printf("Error:clock_gettime failed : '%s'\n", strerror(errno));
		return false;
	}
	return true;
}
static inline bool rt_measure_end(struct time_api_t *ta, bool print)
{
	if(clock_gettime(CLOCK_REALTIME, &ta->end) != 0)
	{
		if(print)
			printf("Error:clock_gettime failed : '%s'\n", strerror(errno));
		return false;
	}
	timespec_diff_calc(ta);
	return true;
}
/* Prints the time elapsed in best possible unit */
static inline void time_print_api(struct time_api_t *ta, const char *x)
{
	uint64_t val = ta->time_ns;
	const char *unit;
	unit = convert_tv_ns_to_bestfit_time_unit(&val);
	if(x)
	{
		printf("%s took %" PRIu64 " %s ", x, ta->time_ns, time_unit[0]);
		if(unit) /* If time > ns */
			printf(" (%" PRIu64 " %s)", val, unit);
		printf("\n");
	}
	else
	{
		printf("%10" PRIu64 " %3s", val, unit);
	}
}
#endif //_TIME_API_H_
