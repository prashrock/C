//gcc -Wall -g -lpthread -o bit_tree_test bit_tree_test.c
#include <stdio.h>         /* printf */
#include <pthread.h>       /* Pthreads library */
#include <string.h>        /* strerror */
#include <stdbool.h>       /* bool, true, false */
#include "rand_utils.h"    /* init_rand(), get_rand_int_limit() */
#include "binary_indexed_tree.h"  /* BI Tree API */

#define MAX_RAND_KEY 20

int main()
{
	int val[] = {1, 2, 3, 4, 5, 6, 7, 8};
	int n = sizeof(val) / sizeof(val[0]);
	int i;
	struct bi_tree *bi;
	init_rand();
	bi = bi_tree_create(val, n);

	printf("%13s | %7s | %5s\n", "arr[id] = val", "bit_val", "sum");
	printf("--------------------------------\n");
	for(i = 0; i < bi->n; i++)
		printf("arr[%2d] = %d   | %7d | %5ld\n", i+1, val[i], bi->tree[i+1],
			   bi_tree_get_sum(bi, i));
	bi_tree_destroy(bi);
	return 0;
}
