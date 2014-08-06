/* gcc -g -o -lrt binary_search_sorted_rotated_array binary_search_sorted_rotated_array.c  */
#include <stdio.h>
#include <stdbool.h>          /* bool, true, false */
#include <pthread.h>          /* Pthreads library */
#include <stdlib.h>           /* atoi  */
#include <string.h>           /* memset */
#include <limits.h>           /* ULONG_MAX */
#include "parser.h"           /* Parser API */

#include "compiler_api.h"     /* MIN */
#include "scan_utils.h"       /* input_integer */
#include "rand_utils.h"       /* rand_int(), srand_cur_time()*/
#include "time_api.h"         /* rt_measure_start(), rt_measure_end() */

#define DQ_OBJ_TYPE              int
#define QUEUE_PRINT_TYPE(_data)  (printf("%d, ", (_data)))
#include "dy_queue.h"         /* Dynamic Queue API */
dq_t *dyq;
dq_stats_t dq_stat_val;

/* Find the minimum element in a Sorted Array which was    *
 * rotated around unknown # times. Handle duplicates.      *
 * Impl: Modified Binary Search                            *
 * Time complexity:                                        *
 *      Without duplicates worst-case = O(lg n)            *
 *      With duplicates worst-case    = O(n)               */
static int bs_rotated_sorted_array(int arr[], int low, int high)
{
	/* Handle case without rotation and without duplicates */
	if(arr[low] < arr[high])       return arr[low];
	/* Handle bound conditions + Divide-Conquer base case  */
	if(low > high || low == high)  return arr[low];
	/* Find middle element                                 */
	int mid = low + (high - low)/2;
	/* Watch out for duplicate elements -- causes O(n)     */
	if(arr[low] == arr[mid] && arr[high] == arr[mid])
		return MIN(bs_rotated_sorted_array(arr, low, mid-1),
				   bs_rotated_sorted_array(arr, mid+1, high));
	/* Check if mid is the minimum element */
	if(mid < high && arr[mid] < arr[mid-1])
		return arr[mid];
	/* Decide recursion on left-half or right-half         *
	 * Always chose the side which looks "unsorted"        *
	 * This is the main difference wrt Binary Search       */
	/* Consider - 7 1 2 |3| 4 5 6                          */
	if(arr[mid] < arr[high])
		return bs_rotated_sorted_array(arr, low, mid-1);
	/* Consider - 3 4 5 |6| 7 1 2                          */
	return bs_rotated_sorted_array(arr, mid+1, high);
}

static void print_help_string()
{
	printf("\n\nPress Enter to exit or follow below commands\n");
	printf("\t          ? - Print this text again\n");
	printf("\t         +1 - Add numbers into Array\n");
	printf("\t          - - Remove number from begining of Array\n");
	printf("\t          p - Print Array (sorted or unsorted)\n");
	printf("\t          n - Print Size information of Array\n");
	printf("\t bin_search - Bin search on entered Rotated Sorted Array\n");
}

static void multi_char_CLI(const char *c)
{
	DQ_OBJ_TYPE	min;
	if(strncmp(c, "bin_search", strlen("bin_search")) == 0)
	{
		min = bs_rotated_sorted_array(dyq->elems, 0, dq_count(dyq)-1);
		printf("Min element in rotated sorted array  = ");
		QUEUE_PRINT_TYPE(min);
		printf("\n");
	}
}

static void single_char_CLI(const char *c)
{
	DQ_OBJ_TYPE x; 
	switch( *c )
	{
	case 'p': case 'P':
		printf("Printing Array elements:\n");
		dq_iterator(dyq, dq_print_element);
		printf("\n");
		break;
	case '-':
		if(dq_deq(dyq, &x)){
			printf("Info: Successfully removed element = ");
			QUEUE_PRINT_TYPE(x);
			printf("\n");
		}
		else
			printf("Error: Deq failed, Queue empty ?\n");
		break;
	case 'n': case 'N':
		printf("Array Count=%d Size=%d\n", dq_count(dyq), dq_size(dyq));
		break;
	case '?':
		print_help_string();
		break;
	default:
		break;
	}
}

static void single_char_CLI_with_num(const char *c)
{
	DQ_OBJ_TYPE x = atoi(c+1);
	switch(*c)
	{
	case '+':
		if(dq_enq(dyq, x) == false)
			printf("Error: Enq failed. Queue Full ?\n");
		break;
	case 't': case 'T':
		printf("No Thread usage yet\n");
		break;
	default:
		break;
	}
}

static inline void test_driver()
{
	int arr1[] =  {5, 6, 1, 2, 3, 4};
	int n1 = sizeof(arr1)/sizeof(arr1[0]);
	printf("The minimum element is %d (expected = 1)\n",
		   bs_rotated_sorted_array(arr1, 0, n1-1));

	int arr2[] =  {1, 1, 0, 1};
	int n2 = sizeof(arr2)/sizeof(arr2[0]);
	printf("The minimum element is %d (expected = 0)\n",
		   bs_rotated_sorted_array(arr2, 0, n2-1));

	int arr3[] =  {1, 1, 2, 2, 3};
	int n3 = sizeof(arr3)/sizeof(arr3[0]);
	printf("The minimum element is %d (expected = 1)\n",
		   bs_rotated_sorted_array(arr3, 0, n3-1));

	int arr4[] =  {3, 3, 3, 4, 4, 4, 4, 5, 3, 3};
	int n4 = sizeof(arr4)/sizeof(arr4[0]);
	printf("The minimum element is %d (expected = 3)\n",
		   bs_rotated_sorted_array(arr4, 0, n4-1));

	int arr5[] =  {2, 2, 2, 2, 2, 2, 2, 2, 0, 1, 1, 2};
	int n5 = sizeof(arr5)/sizeof(arr5[0]);
	printf("The minimum element is %d (expected = 0)\n",
		   bs_rotated_sorted_array(arr5, 0, n5-1));

	int arr6[] =  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1};
	int n6 = sizeof(arr6)/sizeof(arr6[0]);
	printf("The minimum element is %d (expected = 1)\n",
		   bs_rotated_sorted_array(arr6, 0, n6-1));

	int arr7[] =  {2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2};
	int n7 = sizeof(arr7)/sizeof(arr7[0]);
	printf("The minimum element is %d (expected = 0)\n",
		   bs_rotated_sorted_array(arr7, 0, n7-1));
}

int main()
{
	printf("Do some sanity test cases first:\n");
	test_driver();
	printf("If sanity looks ok, open it up to user\n");
	dyq = dq_init();
	print_help_string();
	handle_CLI(single_char_CLI, single_char_CLI_with_num, multi_char_CLI);
	dq_destroy(dyq);
	return 0;
}
