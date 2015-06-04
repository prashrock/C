#ifndef _RAND_UTILS_API_
#define _RAND_UTILS_API_
#include <stdio.h>
#include <stdbool.h>       /* bool, true, false */
#include <stdlib.h>        /* srand(), rand()  */
#include <time.h>          /* get current time - time() */
#include <sys/time.h>      /* gettimeofday() */
#include <unistd.h>        /* getpid() */

#ifdef __cplusplus
extern "C" {
#endif

/* Include PID + current time for seed initialization */
static inline void init_rand()
{
	struct timeval time;
	gettimeofday(&time, NULL);
	/* Multiply with a prime number */
	srand((((time.tv_sec ^ getpid()) * 100003) + time.tv_usec));
}

static inline int get_rand_int()
{
	return rand();
}

/* Generate a random number between [0 to limit-1] */
static inline int get_rand_int_limit(int limit)
{
	return get_rand_int() % limit;
}

#ifdef __cplusplus
}
#endif
	
#endif //_RAND_UTILS_API_
