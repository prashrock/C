/* gcc -g -o reservoir_rand_sample reservoir_rand_sample.c */
#include <stdio.h>
#include <stdbool.h>       /* bool, true, false */
#include <pthread.h>       /* Pthreads library */
#include <stdlib.h>        /* atoi  */
#include <string.h>        /* memset */
#include <limits.h>        /* ULONG_MAX */
#include "parser.h"        /* Parser API */
#include "multi_thread.h"  /* PThread helper  */
#include "print_utils.h"   /* print_int_array */
#include "scan_utils.h"    /* input_integer */
#include "rand.h"          /* get_rand_int()  */


/* Get k random samples from a huge/infinite input stream       *
 * Have to call this function for each input stream value       *
 * Time Complexity = O(n)                                       *
 * Space Complexity = O(k)                                      */
static inline void reservoir_sample(int *sample, int *stream_val,
									unsigned int stream_idx, int k)
{
	/* If input stream idx < sample idx, store number */
	if(stream_idx < k)
		sample[stream_idx] = *stream_val;
	else
	{
		/* Generate a random number between 0 to stream_idx */
		int j = get_rand_int() % (stream_idx + 1);
		if(j < k)
			sample[j] = *stream_val;
	}	
}

void reservoir_sample_test(const int n, const int k)
{
	int i;
	int sample[k];
	init_rand();
	for(i = 0; i < n; i++)
		reservoir_sample(sample, &i, i, k);
	printf("%d Random Samples of %d:\n", k, n);
	print_int_array(sample, k);
}

int main()
{
	int n, k;
	printf("Enter Number of elements in stream 'n' :\n");
	if(input_integer(&n) == false) return 0;
	printf("Enter Number of samples needed 'k' :\n");
	if(input_integer(&k) == false) return 0;
	reservoir_sample_test(n, k);
	return 0;
}
