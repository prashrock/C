/* gcc -g -o dyn_fibonacci dyn_fibonacci.c  */
#include <stdio.h>
#include <stdbool.h>       /* bool, true, false */
#include <pthread.h>       /* Pthreads library */
#include <stdlib.h>        /* atoi  */
#include <string.h>        /* memset */
#include <limits.h>        /* ULONG_MAX */
#include "parser.h"        /* Parser API */
#include "scan_utils.h"    /* input_integer */
#include "multi_thread.h"  /* PThread helper  */

/* Use Dynamic Programming to calculate fibonacci #    *
 * Since this uses a recursive approach, it can only   *
 * handles upto idx=44, above this, recursion too deep */
unsigned int dyn_fib(const unsigned int idx)
{
	if(idx <= 1)return idx;
	else	    return (dyn_fib(idx - 1) + dyn_fib(idx - 2));
}

/* Dynamic Programming - Memoization for fibonacci #   *
 * Recursive + use lookup table->top-down approach     *
 * Time  Complexity = O(n)                             *
 * Space Complexity = O(n)                             *
 * handles upto idx=50, above this, uint32_t overflows */
unsigned int dyn_fib_memoize(unsigned int *x, const unsigned int idx)
{
	if(idx > 1 && x[idx] == 0){
		x[idx] = dyn_fib_memoize(x, idx-1) + dyn_fib_memoize(x, idx-2);
	}	
	return x[idx];
}

unsigned int dyn_fib_memoize_wrapper(const unsigned int idx)
{
	unsigned int arr[idx + 1]; /* We need to store idx==0 as well */
	memset(arr, 0, (idx + 1)*sizeof(unsigned int));
	arr[0] = 0;
	arr[1] = 1;
	dyn_fib_memoize(arr, idx);
	return arr[idx];
}


/* Dynamic Programming - Tabulation for fibonacci #    *
 * Construct lookup table till n ->bottum-up approach  *
 * Time  Complexity = O(n)                             *
 * Space Complexity = O(n)                             *
 * handles upto idx=50, above this, uint32_t overflows */
unsigned int dyn_fib_tabulate(const unsigned int idx)
{
	int i;
	unsigned int arr[idx + 1]; /* We need to store idx==0 as well */
	memset(arr, 0, (idx + 1)*sizeof(unsigned int));
	arr[0] = 0;
	arr[1] = 1;
	for(i = 2; i <= idx; i++)
		arr[i] = arr[i - 1] + arr[i - 2]; 
	return arr[idx];
}

int main()
{
	int idx, fib_mem, fib_tab;
	printf("Enter Unsigned Fibonacci IDX:\n");
	if(input_integer(&idx))
	{
		//fib = dyn_fib(idx);
		fib_mem = dyn_fib_memoize_wrapper(idx);
		fib_tab = dyn_fib_tabulate(idx);
		printf("Fib[%u] = %u  %u\n", idx, fib_mem, fib_tab);
	}
	return 0;
}
