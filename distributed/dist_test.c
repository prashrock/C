/* gcc -g -o dist_test dist_test.c */
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
#include "string_api.h"    /* String utilities */
#include "election.h"      /* Election API */


#define DQ_OBJ_TYPE int
#define QUEUE_PRINT_TYPE(_data) printf(" %d,", (_data));
#include "dy_queue.h"      /* Dynamic Queue API */

static void find_majority_test()
{
	int n, majority;
	dq_t *dq = dq_init();
	printf("Input Integer Array(press enter after each integer):\n");
	while(1){
		if(input_integer(&n))	dq_enq(dq, n);
		else break;
	}
	printf("User entered %d integers:\n", dq_count(dq));
	dq_iterator(dq, dq_print_element);
	printf("\n");
	if(boyer_moore_majority_vote(dq->elems, dq_count(dq), &majority))
		printf("Majority Element = %d\n", majority);
	else
		printf("No Majority Found\n");
	dq_destroy(dq);
}
static void print_help_string()
{
	printf("\n\nPress Enter to exit or follow below commands\n");
	printf("\t        ? - Print this text again\n");
	printf("\tmajority* - Boyer Moore Majority Vote Algorithm\n");
	printf("\t NOTE - * CMDS take optional loop_count, eg majority 2\n");
}
static void multi_char_CLI(const char *c)
{
	int lp = 1;
	const char *space;
	if((space = my_strstr_with_strlen(c, " ")))
	{
		lp = atoi(space + 1);
		if(lp == 0) lp = 1;
		printf("lp = %d\n", lp);
	}
	if(strncmp(c, "majority", strlen("majority")) == 0)
	{
		printf("Find integer occuring for half or more times\n");
		while(lp--) find_majority_test();
	}
}
static void single_char_CLI(const char *c)
{
	switch( *c )
	{
	case '?':
		print_help_string();
		break;
	default:
		break;
	}
}
static void single_char_CLI_with_num(const char *c)
{
	//int x = atoi(c+1);
	switch(*c)
	{
	case 't': case 'T':
		//handle_multi_threaded_test(x, queue_test_thread, false);
		printf("No Thread usage yet\n");
		break;
	default:
		break;
	}
}
int main()
{
	print_help_string();
	handle_CLI(single_char_CLI, single_char_CLI_with_num, multi_char_CLI);
	return 0;
}
