//gcc -Wall -g -lpthread -o queue_test queue_test.c
#include <stdio.h>         /* printf */
#include <pthread.h>       /* Pthreads library */
#include <string.h>        /* strerror */
#include "parser.h"        /* Parser API */
#include "multi_thread.h"  /* PThread helper  */

//#define DYN_QUEUE 1
#define POW2_QUEUE 1
#ifdef DYN_QUEUE
#include "dy_queue.h"      /* Dynamic Queue API  */
dq_t *dyq;
dq_stats_t dq_stat_val;
#define queue_enq(_val)  dq_enq(dyq, (_val))
#define queue_deq(_ptr)  dq_deq(dyq, (_ptr))
#define queue_print_element dq_print_element
#define queue_iterator(qp) dq_iterator(dyq, (qp))
#define queue_count() dq_count(dyq)
#define queue_size() dq_size(dyq)
#define queue_print_stats(_val) dq_print_stats(dyq)
#define q_stats dq_stat_val
#elif POW2_QUEUE
#define SQ2_MAX_ENTRIES 8
#define queue_enq  sq2_enq
#define queue_deq  sq2_deq
#define queue_print_element sq2_print_element
#define queue_iterator sq2_iterator
#define queue_count sq2_count
#define queue_size sq2_size
#define queue_print_stats sq2_print_stats
#define q_stats sq2_stats
#include "queue_pow2.h"         /* Static Pow 2 Queue API  */
#else
#define SQ_MAX_ENTRIES 5
#define queue_enq  sq_enq
#define queue_deq  sq_deq
#define queue_print_element sq_print_element
#define queue_iterator sq_iterator
#define queue_count sq_count
#define queue_size sq_size
#define queue_print_stats sq_print_stats
#define q_stats sq2_stats
#include "queue.h"         /* Static Queue API  */
#endif

#define LP_CNT      10000


static unsigned long enq_tot, deq_tot;

void queue_test_thread(struct thread_args *x)
{
	int i,  val;
	for(i = 0; i < LP_CNT; i++)
	{
		if(pthread_mutex_lock(x->mutex) == 0)
		{
			if(queue_enq(i + x->tid))
				enq_tot += (i + x->tid);
			pthread_mutex_unlock(x->mutex);
		}
		else break;
		if(pthread_mutex_lock(x->mutex) == 0)
		{
			if(queue_deq(&val))
				deq_tot += val;
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
	printf("\t +1 - Add a number into Queue\n");
	printf("\t  - - Remove a number from Queue\n");
	printf("\t  p - Print Queue\n");
	printf("\t  n - Print Size information of Queue\n");
	printf("\t t2 - create '2' threads to handle 1 million enq/deq\n ");
}

static void single_char_CLI(const char *c)
{
	int x;
	switch( *c )
	{
	case 'p': case 'P':
		printf("Printing Queue elements:\n");
		queue_iterator(queue_print_element);
		queue_print_stats(&q_stats);
		break;
	case '-':
		if(queue_deq(&x)){
			printf("Info: Successfully removed element = ");
			QUEUE_PRINT_TYPE(x);
		}
		else
			printf("Error: Deq failed, Queue empty ?\n");
		break;
	case 'n': case 'N':
		printf("Queue Count=%d Size=%d\n", queue_count(), queue_size());
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
		if(queue_enq(x) == false)
			printf("Error: Enq failed. Queue Full ?\n");
		break;
	case 't': case 'T':
		handle_multi_threaded_test(x, queue_test_thread, false);
		printf("Enq_tot=%lu Deq_tot=%lu\n", enq_tot, deq_tot);
		enq_tot = deq_tot = 0;
		queue_print_stats(&q_stats);
		break;
	default:
		break;
	}
}

int main()
{
#ifdef DYN_QUEUE
	dyq = dq_init();
#endif
	print_help_string();
	handle_CLI(single_char_CLI, single_char_CLI_with_num, NULL);
#ifdef DYN_QUEUE
	dq_destroy(dyq);
#endif
	return 0;
}
