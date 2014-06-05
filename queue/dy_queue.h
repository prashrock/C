#ifndef _DY_QUEUE_DS_
#define _DY_QUEUE_DS_
#include <stdio.h>
#include <stdlib.h>        /* atoi  */
#include <stdbool.h>       /* bool, true, false */
#include <ctype.h>         /* isspace */
#include <string.h>        /* memset */
#include <assert.h>        /* assert */
#include <limits.h>        /* INT_MAX */

#ifndef QUEUE_PRINT_TYPE
#define QUEUE_PRINT_TYPE(_data_) printf("%d\n", _data_)
#endif
#ifndef DQ_OBJ_TYPE
#define DQ_OBJ_TYPE int
#endif

#define DQ_MULTIPLIER (2)

#ifndef dy_arr_stats_def
#define dy_arr_stats_def 1
typedef struct dy_arr_stats
{
	unsigned resize_up_cnt;
	unsigned resize_down_cnt;
	unsigned resize_up_err_cnt;
	unsigned resize_down_err_cnt;
}dy_arr_stats;
#endif

typedef struct
{
	unsigned enq_cnt;
	unsigned deq_cnt;
	unsigned full_cnt;
	unsigned empty_cnt;
	dy_arr_stats dy_arr;
}dq_stats_t;

/* When Tail(P) ==  Head(C),     Queue is Empty                *
 * When Tail(P) ==  Head(C) - 1, Queue is Full                 *
 * the Power of 2 property provides bitwise-and for            *
 * Queue_Count    = (tail - head) & (size - 1)                 *
 * Queue_free_cnt = a) Queue_Count(head, (tail + 1), size)OR   *
 *                = b) Queue_Count(head, (tail + mask, size))  *
 * Note: Power of 2 queue can hold atmost (size - 1) elements  *
 * Since code maintains distance between producer & consumer   *
 * of either 1 (in a) or mask (in b), subtractions will not    *
 * cause overflow.                                             *
 */
typedef struct
{
	unsigned int size;   /* Current sizeof(dq_elems) */
	unsigned int head;   /* Index of first element in queue */
	unsigned int tail;   /* Index of first free space in queue */
	DQ_OBJ_TYPE *elems;
	dq_stats_t stats;	
}dq_t;


/*--------------------Internal Functions(Start)--------------------*/
static inline int  dq_size_impl(dq_t *dq) {return dq->size;}
static inline dq_t *dq_init_impl()
{
	dq_t *dq = malloc(sizeof(dq_t));
	if(dq)
		memset(dq, 0 , sizeof(dq_t));
	return dq;
}
static inline void dq_destroy_impl(dq_t *dq)
{
	if(dq)
	{
		if(dq->elems)
			free(dq->elems);
		free(dq);
	}
}

static inline int dq_count_impl(unsigned int tail, unsigned int head, unsigned int size)
{
	return ((tail - head) & (size - 1));
}
static inline int dq_space_impl(unsigned int tail, unsigned int head, unsigned int size)
{
	return dq_count_impl(head, tail + 1, size);
}
static inline bool is_dq_empty_impl(dq_t *dq)
{
	return dq_count_impl(dq->tail, dq->head, dq->size) == 0;
}
static inline bool is_dq_full_impl(dq_t *dq)
{
	return (dq_count_impl(dq->tail, dq->head, dq->size) + 1 == dq->size);
}
static inline bool dq_resize_impl(dq_t *dq, int new_size)
{
    int i, cnt;
	unsigned int new_tail = dq_count_impl(dq->tail, dq->head, dq->size);
	DQ_OBJ_TYPE *new_elems;
	if(new_size == 0) new_size = 2;
	assert(dq_size_impl(dq) >= dq_count_impl(dq->tail, dq->head, dq->size));
	new_elems =  malloc(sizeof(DQ_OBJ_TYPE) * new_size);
	if(new_elems == NULL)
		return false;
	for(i = dq->head, cnt = 0; cnt < dq_count_impl(dq->tail, dq->head, dq->size); i++, cnt++)
		new_elems[cnt] = dq->elems[i % dq_size_impl(dq)];
	free(dq->elems);
	dq->head = 0;
	dq->tail = new_tail;
	dq->size = new_size;
	dq->elems = new_elems;
	return true;
}
static inline DQ_OBJ_TYPE dq_peek_impl(dq_t *dq){return dq->elems[dq->head];}
static inline void dq_enq_impl(dq_t *dq, DQ_OBJ_TYPE x)
{
	dq->elems[dq->tail++ % dq_size_impl(dq)] = x;
}
static inline DQ_OBJ_TYPE dq_deq_impl(dq_t *dq)
{
	return dq->elems[dq->head++ % dq_size_impl(dq)];
}
static inline void  dq_iterator_impl(dq_t *dq, void (*fn)(DQ_OBJ_TYPE *x))
{
	unsigned int i, cnt;
	if(fn == NULL)
		return;
	for(i = dq->head, cnt = 0; cnt < dq_count_impl(dq->tail, dq->head, dq->size);
		cnt++, (i=(i+1) % dq_size_impl(dq)))
		fn(&(dq->elems[i]));
}

/*--------------------Internal Functions(End)--------------------*/
/*--------------------API Functions(Start)--------------------*/
dq_t *dq_init()                             {return dq_init_impl();}
void dq_destroy(dq_t *dq)                   {return dq_destroy_impl(dq);}
void dq_print_element(DQ_OBJ_TYPE *x)       {QUEUE_PRINT_TYPE(*x);}
bool is_dq_empty(dq_t *dq)                  {return is_dq_empty_impl(dq);}
bool is_dq_full(dq_t *dq)                   {return is_dq_full_impl(dq);}
int  dq_size(dq_t *dq)                      {return dq_size_impl(dq);}
int  dq_count(dq_t *dq)                     {return dq_count_impl(dq->tail, dq->head, dq_size(dq));}
DQ_OBJ_TYPE dq_peek(dq_t *dq)               {return dq_peek_impl(dq);}
void dq_iterator(dq_t *dq, void (*fn)(DQ_OBJ_TYPE *x))
{
	dq_iterator_impl(dq, fn);
} 
bool dq_enq(dq_t *dq, DQ_OBJ_TYPE x)
{
	if(is_dq_full(dq) == true || dq->elems == NULL)
	{
		if(dq->elems)
			dq->stats.full_cnt++;
		if(dq_size(dq) == INT_MAX ||
		   dq_resize_impl(dq, (dq_size(dq) * DQ_MULTIPLIER)) == false)
		{
			dq->stats.dy_arr.resize_up_err_cnt++;
			return false;
		}
		dq->stats.dy_arr.resize_up_cnt++;
	}
	dq_enq_impl(dq, x);
	dq->stats.enq_cnt++;
	return true;
}
bool dq_deq(dq_t *dq, DQ_OBJ_TYPE *x)
{
	if(is_dq_empty(dq) == true) {                                        
		dq->stats.empty_cnt++;
		return false;
	}
	*x = dq_deq_impl(dq);
	dq->stats.deq_cnt++;
	if(is_dq_empty(dq) == false && dq_count(dq) == dq_size(dq)/4)
	{
		if(dq_resize_impl(dq, (dq_size(dq) / DQ_MULTIPLIER)) == false)
		{
			dq->stats.dy_arr.resize_down_err_cnt++;
			return false;
		}
		dq->stats.dy_arr.resize_down_cnt++;
	}
	return true;
}
void dq_print_stats(dq_t *dq)
{
	printf("Dynamic Queue Statistics:\n");
	printf("Queue Size            = %10u\t", dq_size(dq));
	printf("Queue Count           = %10u\n", dq_count(dq));
	printf("Enq   Count           = %10u\t", dq->stats.enq_cnt);
	printf("Deq   Count           = %10u\n", dq->stats.deq_cnt);
	printf("Full  Count           = %10u\t", dq->stats.full_cnt);
	printf("Empty Count           = %10u\n", dq->stats.empty_cnt);
	printf("Dy_ARR Size UP Count  = %10u\t", dq->stats.dy_arr.resize_up_cnt);
	printf("Dy_ARR Size Down Count= %10u\n",
		   dq->stats.dy_arr.resize_down_cnt);
	printf("Dy_ARR Size UP Err    = %10u\t",
		   dq->stats.dy_arr.resize_up_err_cnt);
	printf("Dy_ARR Size Down Err  = %10u\n",
		   dq->stats.dy_arr.resize_down_err_cnt);
	/* Keep stats as read on clear */
	memset(&dq->stats, 0, sizeof(dq_stats_t));
}

/*--------------------API Functions(Etart)--------------------*/
#endif //_DY_QUEUE_DS_
