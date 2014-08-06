/* gcc -g -o -lrt search_test search_test.c  */
#include <stdio.h>
#include <stdbool.h>          /* bool, true, false */
#include <pthread.h>          /* Pthreads library */
#include <stdlib.h>           /* atoi  */
#include <string.h>           /* memset */
#include <limits.h>           /* ULONG_MAX */
#include "parser.h"           /* Parser API */

#include "scan_utils.h"       /* input_integer */
#include "rand_utils.h"       /* rand_int(), srand_cur_time()*/
#include "time_api.h"         /* rt_measure_start(), rt_measure_end() */
#include "binary_search.h"    /* Binary Search API */

#define DQ_OBJ_TYPE              int
#define QUEUE_PRINT_TYPE(_data)  (printf("%d, ", (_data)))
#include "dy_queue.h"         /* Dynamic Queue API */
dq_t *dyq;
dq_stats_t dq_stat_val;

static inline void binary_search_rotated_test()
{
	int arr1[] =  {5, 6, 1, 2, 3, 4};
	int n1 = sizeof(arr1)/sizeof(arr1[0]);
	printf("Do some sanity test cases first:\n");
	printf("The minimum element is %d (expected = 1)\n",
		   binary_search_rotated_sorted_array(arr1, 0, n1-1));

	int arr2[] =  {1, 1, 0, 1};
	int n2 = sizeof(arr2)/sizeof(arr2[0]);
	printf("The minimum element is %d (expected = 0)\n",
		   binary_search_rotated_sorted_array(arr2, 0, n2-1));

	int arr3[] =  {1, 1, 2, 2, 3};
	int n3 = sizeof(arr3)/sizeof(arr3[0]);
	printf("The minimum element is %d (expected = 1)\n",
		   binary_search_rotated_sorted_array(arr3, 0, n3-1));

	int arr4[] =  {3, 3, 3, 4, 4, 4, 4, 5, 3, 3};
	int n4 = sizeof(arr4)/sizeof(arr4[0]);
	printf("The minimum element is %d (expected = 3)\n",
		   binary_search_rotated_sorted_array(arr4, 0, n4-1));

	int arr5[] =  {2, 2, 2, 2, 2, 2, 2, 2, 0, 1, 1, 2};
	int n5 = sizeof(arr5)/sizeof(arr5[0]);
	printf("The minimum element is %d (expected = 0)\n",
		   binary_search_rotated_sorted_array(arr5, 0, n5-1));

	int arr6[] =  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1};
	int n6 = sizeof(arr6)/sizeof(arr6[0]);
	printf("The minimum element is %d (expected = 1)\n",
		   binary_search_rotated_sorted_array(arr6, 0, n6-1));

	int arr7[] =  {2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2};
	int n7 = sizeof(arr7)/sizeof(arr7[0]);
	printf("The minimum element is %d (expected = 0)\n",
		   binary_search_rotated_sorted_array(arr7, 0, n7-1));
	printf("If sanity looks ok, open it up to user\n");
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
		min = binary_search_rotated_sorted_array(dyq->elems, 0, dq_count(dyq)-1);
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


int main()
{
	//binary_search_rotated_test();
	dyq = dq_init();
	print_help_string();
	handle_CLI(single_char_CLI, single_char_CLI_with_num, multi_char_CLI);
	dq_destroy(dyq);
	return 0;
}
