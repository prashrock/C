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
dq_t *dyq;
dq_stats_t dq_stat_val;
enum SORT_TYPE sort_type = SELECTION_SORT;

uint64_t tmp;
/* Common wrapper to invoke sort, measure time and print elements */
static void sort_test(bool inprint, bool outprint)
{
	struct time_api_t ta;
	int n = dq_count(dyq);
	const char *sname;
	if(outprint)     sname = sort_name[sort_type];
	else             sname = NULL;
					
	if (n == 0)
	{
		printf("Info: Array empty, nothing to sort\n");
		return;
	}
	if(inprint){
		printf("Elements before sorting: ");
		dq_iterator(dyq, dq_print_element);
		printf("\n");
	}
	if(rt_measure_start(&ta, false) == false) return;
	sort_api(sort_type, dyq->elems, dq_count(dyq));
	if(rt_measure_end(&ta, false) == false)   return;
	time_print_api(&ta, sname);
	if(inprint){
		printf("Elements after sorting: ");
		dq_iterator(dyq, dq_print_element);
		printf("\n");
	}
}
/* Handle test cases such as missing/overwritten elements after sort and  *
 * Sort invariant failure */
static void rand_sort_test(int n, bool inprint, bool outprint)
{
	unsigned long long total = 0;
	int i, rand_val, new_val;
	while(dq_deq(dyq, &rand_val));
	for(i = 0; i < n; i++)
	{
		rand_val = get_rand_int_limit(n);
		dq_enq(dyq, rand_val);
		total += rand_val;
	}
	sort_test(inprint, outprint);
	rand_val = 0;
	for(i = 0; i < n; i++)
	{
		if(dq_deq(dyq, &new_val) == false)
			break;
		if(new_val < rand_val)
		{
			printf("Error: Sort Invariant not met  - ");
			printf("%d found before %d\n",
				   rand_val, new_val);
			return;
		}
		rand_val = new_val;
		total -= rand_val;
	}
	if(total)
		printf("Error: Elements lost after sort (tot=%llu)\n",
			total);
	else if(outprint == true)
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
	printf("\t  rand_sort - Generate & Sort 'n' random integers\n");
	printf("\t    pr_sort - Generate, Sort, Print 'n' random integers\n");
	printf("\t   cmp_sort - Compare sorts with 'n' rand integers\n");
	printf("\t       mode - Select type of sort (Insert, Quick, etc.)\n");
//	printf("\t         t2 - create '2' threads to sort 1M random nos\n ");
	printf("\t       NOTE - * CMDS take optional loop_count (sort 2)\n");
}

static void multi_char_CLI(const char *c)
{
	int lp = -1;
	const char *space;
	if((space = my_strstr_with_strlen(c, " ")))
	{
		lp = atoi(space + 1);
		printf("lp = %d\n", lp);
	}
	if(strncmp(c, "sort", strlen("sort")) == 0)
	{
		do { sort_test(true, true);} while(lp > 0 && lp--);
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
	else if(strncmp(c, "mode", strlen("mode")) == 0)
	{
		if(lp == -1 || lp >= INVALID_SORT)
		{
			printf("Info: To set new sort mode, use 'mode <0-n>'\n");
			printf("Info: Current sort_mode = '%s'\n", sort_name[sort_type]);
			print_sort_mode_table();
		}
		else
		{
			printf("previous sort_mode = '%s'\n", sort_name[sort_type]);
			sort_type = lp;
			printf("new sort_mode = '%s'\n",sort_name[sort_type]);
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
	dyq = dq_init();
	print_help_string();
	handle_CLI(single_char_CLI, single_char_CLI_with_num, multi_char_CLI);
	dq_destroy(dyq);
	return 0;
}


