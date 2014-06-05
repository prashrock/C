/* gcc -g -o dyn_min_num_jumps_to_end dyn_min_num_jumps_to_end.c  */
#include <stdio.h>
#include <stdbool.h>       /* bool, true, false */
#include <pthread.h>       /* Pthreads library */
#include <stdlib.h>        /* atoi  */
#include <string.h>        /* memset */
#include <limits.h>        /* ULONG_MAX */
#include "parser.h"        /* Parser API */
#include "multi_thread.h"  /* PThread helper  */
#include "scan_utils.h"    /* input_integer */

#define DQ_OBJ_TYPE int
#define QUEUE_PRINT_TYPE(_data) printf(" %d,", (_data));
#include "dy_queue.h"      /* Dynamic Queue API */

/* Question:
 * Given an array of integers which represent max # steps that  *
 * can be made from a given step, calculate the minimum number  *
 * of jumps to reach end of array                               */

bool subset_sum(int set[], const int n, const int sum)
{
	int i;
	int j;
	/* Sum in row and set[] numbers in column */
	bool matrix[sum+1][n+1];
	memset(matrix, 0, sum*n);
	/* First row, pre-initialize lookup row for (sum-set[i])  */
	for(j = 0; j <= n; j++)
		matrix[0][j] = true;
	/* First column, pre-initialize for empty set[], except row 0 */
	for(i = 1; i <= sum; i++)
		matrix[i][0] = false;
	/* Calculate all matrix indexes with Dynamic Programming */
	for(i = 1; i <= sum; i++)
	{
		for(j = 1; j <= n; j++){
			int set_idx = j-1;
			//if (i < set[set_idx])
			//	matrix[i][j] = matrix[i][j-1];
			//else
				matrix[i][j] = matrix[i][j-1] ||
					matrix[i-set[set_idx]][j-1];
		}
	}
	/* Print the Matrix */
	printf("Rows = Sums and Columns = n\n");
	for (j = 0; j <= n; j++)
	{
		if(j == 0)         printf("--- | --- ");
		else	           printf("%3d ",set[j-1]);
	}
	printf("\n--------------------------------------------\n");
	for (i = 0; i <= sum; i++)
	{
		if(i == 0) printf("--- | ");
		else       printf("%3d | ", i);
		for (j = 0; j <= n; j++)
			printf ("%3d ", matrix[i][j]);
		printf("\n");
	}
	for(j = 1; j <= n; j++)
		if(matrix[sum][j]) return true;
	return false;
}

/* Can handle duplicates and negative numbers            */
bool subset_sum_brute(int set[], int n, int sum)
{
	int idx = n-1;         /* idx is 0 based */
	if(sum == 0)           return true;
	if(n == 0 && sum != 0) return false;
    /* Remove this if to handle negative numbers in set  */
	//if(set[idx] > sum)
	    //return subset_sum_brute(set, n-1, sum);
	/* 2 cases in dynamic programming:                   *
	 * a) Don't include last element in calculating sum  *
	 * b) Include last element in calculating sum        */
	return (subset_sum_brute(set, n-1, sum) ||
			subset_sum_brute(set, n-1, sum - set[idx]));
}

void subset_input_data()
{
	int n, sum;
	dq_t *dq = dq_init();
	while(1){
		printf("Please Enter Array Integer:\n");
		if(input_integer(&n))	dq_enq(dq, n);
		else break;
	}
	printf("Please Enter Expected Sum:\n");
	if(input_integer(&sum) == false) return;
	printf("User entered %d integers and sum %d\n", dq_count(dq), sum);
	dq_iterator(dq, dq_print_element);
	printf("\n");
	if(subset_sum(dq->elems, dq_count(dq), sum))
		printf("Sum can be reached with subset\n");
	else
		printf("Sum CANNOT be reached with subset\n");
	dq_destroy(dq);
}

int main()
{
	subset_input_data();
	return 0;
}
