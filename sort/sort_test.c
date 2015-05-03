/* gcc -g -o -lrt sort_test sort_test.c  */
#include <stdio.h>
#include <stdbool.h>          /* bool, true, false */
#include <pthread.h>          /* Pthreads library */
#include <stdlib.h>           /* atoi  */
#include <string.h>           /* memset */
#include <limits.h>           /* ULONG_MAX */
#include "parser.h"           /* Parser API */
#include "multi_thread.h"     /* PThread helper  */

#include "string_api.h"       /* String utilities */
#include "scan_utils.h"       /* input_integer */
#include "rand_utils.h"       /* get_rand_int(), init_rand()*/
#include "time_api.h"         /* rt_measure_start(), rt_measure_end() */

#define DQ_OBJ_TYPE              int
#define SORT_OBJ_TYPE            DQ_OBJ_TYPE
#define SELECTION_SORT_OBJ_TYPE  SORT_OBJ_TYPE
#define INSERTION_SORT_OBJ_TYPE  SORT_OBJ_TYPE
#define SHELL_SORT_OBJ_TYPE      SORT_OBJ_TYPE
#define MERGE_SORT_OBJ_TYPE      SORT_OBJ_TYPE
#define QUEUE_PRINT_TYPE(_data)  (printf("%d, ", (_data)))
#include "dy_queue.h"         /* Dynamic Queue API */
#include "selection_sort.h"   /* Selection sort API's */
#include "insertion_sort.h"   /* Insertion sort API's */
#include "shell_sort.h"       /* Shell sort API's */
#include "merge_sort.h"       /* Merge sort API's + inversion cnt API */
#include "sort_api.h"         /* Wrapper API's around sort */
static dq_t *dyq;
static int *sort_array;       /* To convert between queue and array */
static int sort_array_idx = 0;/* Keep track of sort array index */
enum SORT_TYPE sort_type = SELECTION_SORT;
#define MAX_RAND 10


/* Dynamic Queue to array converter */
static void copy_queue_elem_to_array(int *val)
{
	sort_array[sort_array_idx++] = *val;
}

/* Generate n random integers in the range [0, MAX_RAND) and add them to  *
 * the provided dynamic array implementation (dy_queue)                   */
static bool generate_random_integers(dq_t *dyq, int n, int max_val,
									 unsigned long long *total_ptr)
{
	unsigned long long total = 0;
	int i, rand_val;
	for(i = 0; i < n; i++)     /* Add n random integers to container  */
	{
		rand_val = get_rand_int_limit(max_val);
		dq_enq(dyq, rand_val);
		total += rand_val;
	}
	if(total_ptr) *total_ptr = total;
	return true;
}

/* A hack to look at elements as an array - assume container does     *
 * not grow or shrink right now (not multi-thread safe)               */
static bool validate_sort_order_wrapper(dq_t *dyq, int n, unsigned
										long long total)
{
	bool ret;
	sort_array = malloc(sizeof(int[dq_count(dyq)]));
	if(sort_array == NULL)                    return false;
	dq_iterator(dyq, copy_queue_elem_to_array);
	ret = validate_sort_order(sort_array, n, total);
	free(sort_array);
	sort_array_idx = 0;
	return ret;
}
	
/* Common wrapper to invoke sort, measure time and print elements */
static bool sort_test(bool inprint, bool outprint)
{
	struct time_api_t ta;
	int n = dq_count(dyq);
	const char *sname;

	if(outprint)     sname = sort_name[sort_type];
	else             sname = NULL;
					
	if (n == 0)
	{
		printf("Info: Array empty, nothing to sort\n");
		return true;
	}
	if(inprint){
		printf("Elements before sorting: ");
		dq_iterator(dyq, dq_print_element);
		printf("\n");
	}
	if(rt_measure_start(&ta, false) == false) return false;
	sort_api(sort_type, dyq->elems, dq_count(dyq));
	if(rt_measure_end(&ta, false) == false)   return false;
	time_print_api(&ta, sname);
	if(inprint){
		printf("Elements after sorting: ");
		dq_iterator(dyq, dq_print_element);
		printf("\n");
	}
	if(n != dq_count(dyq)) {
		printf("Error: Missing elements in sorted array: ");
		printf("Expected %d, found %d elements\n", n, dq_count(dyq));
		return false;
	}
	
	return validate_sort_order_wrapper(dyq, n, 0);
}

static void rand_sort_test(int n, bool inprint, bool outprint)
{
	unsigned long long total = 0;
	int val;
	while(dq_deq(dyq, &val)); /* Clear any inputs in container            */
	generate_random_integers(dyq, n, MAX_RAND, &total);
	if(sort_test(inprint, outprint) == false)
		return;
	else if(validate_sort_order_wrapper(dyq, n, total) == true)
		printf("'%s' successful on %d random elements\n",
		   sort_name[sort_type], n);
}

/* Compare different sorting algorithms with n random integers */
static void cmp_sort_test(unsigned long long n)
{
	enum SORT_TYPE j, orig = sort_type;
	enum SORT_TYPE begin = SHELL_SORT;//SELECTION_SORT;
	unsigned long long i = 10;
	printf("\n|-------------------------------------------------------|\n");
	printf("  %8s  ", "Count");
	for(j = begin; j < INVALID_SORT; j++){
		if(j == MERGE_SORT_RECURSE_INV_CNT) continue;
		printf("%12s\t",sort_name_abbrv[j]);
	}
	printf("\n|-------------------------------------------------------|\n");
	for(; i <= n; i *= 10)
	{
		printf("%10llu", i);
		fflush(stdout);
		for(j = begin; j < INVALID_SORT; j++)
		{
			if(j == MERGE_SORT_RECURSE_INV_CNT) continue;
			sort_type = j;
			rand_sort_test(i, false, false);
			printf("  ");
			fflush(stdout);
		}
		printf("\n");
	}
	sort_type = orig;
}

static inline void print_sort_mode_table()
{
	printf("\tBelow is a list of supported sort types:\n");
	printf("\t|----------------------------------------|\n");
	printf("\t|                          Name -- Mode  |\n");
	printf("\t|----------------------------------------|\n");
	enum SORT_TYPE i;
	for(i = 0; i != INVALID_SORT; i++)
		printf("\t|%30s -- %4d  |\n",sort_name[i], i);
	printf("\t|----------------------------------------|\n");
}
	
static void print_help_string()
{
	printf("\n\nPress Enter to exit or follow below commands\n");
	printf("\t          ? - Print this text again\n");
	printf("\t         +1 - Add numbers into Array\n");
	printf("\t          - - Remove number from begining of Array\n");
	printf("\t          p - Print Array (sorted or unsorted)\n");
	printf("\t          n - Print Size information of Array\n");
	printf("\t      sort* - Sort user inputted integers\n");
	printf("\t       mode - Calc mode of sorted Array\n");
	printf("\t      occur - Find 1st/last occur in sorted Array\n");
	printf("\t  rand_sort - Generate & Sort 'n' random integers\n");
	printf("\t    pr_sort - Generate, Sort, Print 'n' random integers\n");
	printf("\t   cmp_sort - Compare sorts with 'n' rand integers\n");
	printf("\t  sort_type - Select type of sort (Insert, Quick, etc.)\n");
//	printf("\t         t2 - create '2' threads to sort 1M random nos\n ");
	printf("\t       NOTE - * CMDS take optional loop_count (sort 2)\n");
}

static void multi_char_CLI(const char *c)
{
	int lp = -1;
	const char *space;
	bool lp_found = false;
	if((space = my_strstr_with_strlen(c, " ")))
	{
		lp = atoi(space + 1);
		printf("lp = %d\n", lp);
		lp_found = true;
	}
	if(strncmp(c, "sort", strlen("sort")) == 0)
	{
		do { sort_test(true, true);} while(lp > 0 && lp--);
	}
	if(strncmp(c, "mode", strlen("mode")) == 0)
	{
		int mode, mode_freq;
		sort_array = malloc(sizeof(int[dq_count(dyq)]));
		if(sort_array != NULL) {
			dq_iterator(dyq, copy_queue_elem_to_array);
			if(find_mode_in_sorted_array(sort_array, dq_count(dyq),
										 &mode, &mode_freq) == false)
				printf("Error: Mode calculation failed.\n");
			else
				printf("Mode of %d elements = %d (freq=%d)\n",
					   dq_count(dyq), mode, mode_freq);
		}
		free(sort_array);
		sort_array_idx = 0;
	}
	if(strncmp(c, "occur", strlen("occur")) == 0)
	{	
		int first_idx, last_idx;
		if(lp_found == false)
			printf("Usage: occur <val_to_search>\n");
		else {
			sort_array = malloc(sizeof(int[dq_count(dyq)]));
			if(sort_array != NULL) {
				dq_iterator(dyq, copy_queue_elem_to_array);
			}
			first_idx = find_first_or_last_idx(sort_array,
											   dq_count(dyq),
											   lp, true);
			last_idx = find_first_or_last_idx(sort_array,
											  dq_count(dyq),
											  lp, false);
			free(sort_array);
			sort_array_idx = 0;
			printf("First occurence of %d = %d (zero-based position)\n",
				   lp, first_idx);
			printf(" Last occurence of %d = %d (zero-based position)\n",
				   lp, last_idx);
		}
	}
	else if(strncmp(c, "rand_sort", strlen("rand_sort")) == 0)
	{
		if(lp < 1)
			printf("Usage: rand_sort <n>\n");
		else
			rand_sort_test(lp, false, true);
	}
	else if(strncmp(c, "pr_sort", strlen("pr_sort")) == 0)
	{
		if(lp < 1)
			printf("Usage: pr_sort <n>\n");
		else
			rand_sort_test(lp, true, true);
	}
	else if(strncmp(c, "cmp_sort", strlen("cmp_sort")) == 0)
	{
		if(lp < 1)
			printf("Usage: cmp_sort <n>\n");
		else
			cmp_sort_test(lp);
	}
	else if(strncmp(c, "sort_type", strlen("sort_type")) == 0)
	{
		if(lp == -1 || lp >= INVALID_SORT)
		{
			printf("Info: To set new sort type, use 'sort_type <0-n>'\n");
			printf("Info: Current sort_type = '%s'\n", sort_name[sort_type]);
			print_sort_mode_table();
		}
		else
		{
			printf("previous sort_type = '%s'\n", sort_name[sort_type]);
			sort_type = lp;
			printf("new sort_type = '%s'\n",sort_name[sort_type]);
		}
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
		//handle_multi_threaded_test(x, queue_test_thread, false);
		//printf("Enq_tot=%lu Deq_tot=%lu\n", enq_tot, deq_tot);
		//enq_tot = deq_tot = 0;
		//queue_print_stats(&q_stats);
		break;
	default:
		break;
	}
}

int main()
{
	init_rand();
	dyq = dq_init();
	print_help_string();
	handle_CLI(single_char_CLI, single_char_CLI_with_num, multi_char_CLI);
	dq_destroy(dyq);
	return 0;
}


