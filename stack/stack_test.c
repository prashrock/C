//gcc -Wall -g -lpthread -o stack stack.c
#include <stdio.h>         /* printf */
#include <pthread.h>       /* Pthreads library */
#include <string.h>        /* strerror */
#include "parser.h"        /* Parser API */
#include "multi_thread.h"  /* PThread helper  */

#define DYN_STACK 1
#ifdef DYN_STACK
#include "dy_stack.h"      /* Stack API  */
dstack_t *ds;
#define stack_push(_val) dy_stack_push(ds, (_val))
#define stack_pop(_ptr)  dy_stack_pop(ds, (_ptr))
#define stack_print_element dy_stack_print_element
#define stack_iterator(fn) dy_stack_iterator(ds, (fn))
#define stack_count() dy_stack_count(ds)
#define stack_size() dy_stack_size(ds)
#define stack_print_stats(_val) dy_stack_print_stats(ds)
#else
#define MAX_ENTRIES 50
#include "stack.h"         /* Stack API  */
#endif

#define LP_CNT      10000


static unsigned long push_tot, pop_tot;

void stack_test_thread(struct thread_args *x)
{
	int i, val;
	for(i = 0; i < LP_CNT; i++)
	{
		if(pthread_mutex_lock(x->mutex) == 0)
		{
			if(stack_push(i + x->tid))
				push_tot += (i + x->tid);
			pthread_mutex_unlock(x->mutex);
		}
		else break;
		if(pthread_mutex_lock(x->mutex) == 0)
		{
			if(stack_pop(&val))
				pop_tot += val;
			pthread_mutex_unlock(x->mutex);
		}
		else break;
	}
	if(i != LP_CNT)
		printf("Thread %d made %d cycles out of %d\n", x->tid, i, LP_CNT);
}
	
static void print_help_string()
{
	printf("\n\nPress Enter to exit or follow below commands\n");
	printf("\t  ? - Print this text again\n");
	printf("\t +1 - Add a number into Stack\n");
	printf("\t  - - Remove a number from Stack\n");
	printf("\t  p - Print Stack\n");
	printf("\t  n - Print Size information of Stack\n");
	printf("\t  t2- create '2' threads to handle 1 million push/pop\n ");
}

static void single_char_CLI(const char *c)
{
	int x;
	switch( *c )
	{
	case 'p': case 'P':
		printf("Printing Stack elements:\n");
		stack_iterator(stack_print_element);
		stack_print_stats(&simple_stack_stats);
		break;
	case '-':
		if(stack_pop(&x)){
			printf("Info: Successfully removed element = ");
			STACK_PRINT_TYPE(x);
		}
		else
			printf("Error: Pop failed, stack empty ?\n");
		break;
	case 'n': case 'N':
		printf("Stack Count=%d Size=%d\n", stack_count(), stack_size());
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
	int x = atoi(c+1);
	switch(*c)
	{
	case '+':
		if(stack_push(x) == false)
			printf("Error: Push failed. Stack Full ?\n");
		break;
	case 't': case 'T':
		handle_multi_threaded_test(x, stack_test_thread, false);
		printf("Push_tot=%lu Pop_tot=%lu\n", push_tot, pop_tot);
		push_tot=pop_tot=0;
		stack_print_stats(&simple_stack_stats);
		break;
	default:
		break;
	}
}

int main()
{
#ifdef DYN_STACK
	ds = dy_stack_init();
#endif
	print_help_string();
	handle_CLI(single_char_CLI, single_char_CLI_with_num, NULL);
	dy_stack_destroy(ds);
	return 0;
}
