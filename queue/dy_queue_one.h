#ifdef _DY_QUEUE_DS_
#error "There can only be one Dynamic Queue usage"
#else
#define _DY_QUEUE_DS_
#include <stdio.h>
#include <stdlib.h>        /* atoi  */
#include <stdbool.h>       /* bool, true, false */
#include <ctype.h>         /* isspace */
#include <string.h>        /* memset */
#include <assert.h>        /* assert */
#include <limits.h>        /* INT_MAX */

#ifndef dy_arr_stats
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
}dq_statistics;

#ifndef QUEUE_PRINT_TYPE
#define QUEUE_PRINT_TYPE(_data_) printf("%d\n", _data_)
#endif
#ifndef DQ_OBJ_TYPE
#define DQ_OBJ_TYPE int
#endif

#define DQ_MULTIPLIER (2)


/*--------------------Internal Functions(Start)--------------------*/
static unsigned int dq_size_var = 0;/* Current sizeof(dq_elems) */
static unsigned int dq_head = 0;    /* Index of the first element in queue */
static unsigned int dq_tail = 0;
static DQ_OBJ_TYPE *dq_elems = NULL;
static dq_statistics dq_stats;

static inline int  dq_size_impl()         {return dq_size_var;}
static inline int  dq_count_impl()
{
	return ((dq_tail - dq_head) & (dq_size_impl() - 1));
}
static inline int dq_space_impl()
{
	return dq_count_impl(dq_head, dq_tail + 1, dq_size_impl());
}
static inline bool is_dq_empty_impl(){return dq_count_impl() == 0;}
static inline bool is_dq_full_impl()
{
	return (dq_count_impl() + 1 == dq_size_var);
}
static inline bool dq_resize_impl(int new_size)
{
    int i, cnt;
	unsigned int new_dq_tail = dq_count_impl();
	DQ_OBJ_TYPE *new_elems;
	if(new_size == 0) new_size = 2;
	assert(dq_size_impl() >= dq_count_impl());
	new_elems =  malloc(sizeof(DQ_OBJ_TYPE) * new_size);
	if(new_elems == NULL)
		return false;
	for(i = dq_head, cnt = 0; cnt < dq_count_impl(); i++, cnt++)
		new_elems[cnt] = dq_elems[i % dq_size_impl()];
	free(dq_elems);
	dq_head = 0;
	dq_tail = new_dq_tail;
	dq_size_var = new_size;
	dq_elems = new_elems;
	return true;
}
static inline DQ_OBJ_TYPE dq_peek_impl()  {return dq_elems[dq_head];}
static inline void dq_enq_impl(DQ_OBJ_TYPE x)
{
	dq_elems[dq_tail++ % dq_size_impl()] = x;
}
static inline DQ_OBJ_TYPE dq_deq_impl()
{
	return dq_elems[dq_head++ % dq_size_impl()];
}
static inline void  dq_iterator_impl(void (*fn)(DQ_OBJ_TYPE *x))
{
	unsigned int i, cnt;
	if(fn == NULL)
		return;
	for(i = dq_head, cnt = 0; cnt < dq_count_impl();
		cnt++, (i=(i+1) % dq_size_impl()))
		fn(&(dq_elems[i]));
}

/*--------------------Internal Functions(End)--------------------*/
/*--------------------API Functions(Start)--------------------*/
void dq_print_element(DQ_OBJ_TYPE *x)       {QUEUE_PRINT_TYPE(*x);}
bool is_dq_empty()                           {return is_dq_empty_impl();}
bool is_dq_full()                            {return is_dq_full_impl();}
void dq_iterator(void (*fn)(DQ_OBJ_TYPE *x)){dq_iterator_impl(fn);} 
int  dq_count()                              {return dq_count_impl();}
int  dq_size()                               {return dq_size_impl();}
bool dq_enq(DQ_OBJ_TYPE x)
{
	if(is_dq_full() == true || dq_elems == NULL)
	{
		if(dq_elems)
			dq_stats.full_cnt++;
		if(dq_size() == INT_MAX ||
		  dq_resize_impl(dq_size() * DQ_MULTIPLIER) == false)
		{
			dq_stats.dy_arr.resize_up_err_cnt++;
			return false;
		}
		dq_stats.dy_arr.resize_up_cnt++;
	}
	dq_enq_impl(x);
	dq_stats.enq_cnt++;
	return true;
}
bool dq_deq(DQ_OBJ_TYPE *x)
{
	if(is_dq_empty() == true) {                                        
		dq_stats.empty_cnt++;
		return false;
	}
	*x = dq_deq_impl();
	dq_stats.deq_cnt++;
	if(is_dq_empty() == false && dq_count() == dq_size()/4)
	{
		if(dq_resize_impl(dq_size() / DQ_MULTIPLIER) == false)
		{
			dq_stats.dy_arr.resize_down_err_cnt++;
			return false;
		}
		dq_stats.dy_arr.resize_down_cnt++;
	}
	return true;
}
void dq_print_stats(dq_statistics *stats)
{
	printf("Dynamic Queue Statistics:\n");
	printf("Queue Size            = %10u\t", dq_size());
	printf("Queue Count           = %10u\n", dq_count());
	printf("Enq   Count           = %10u\t", stats->enq_cnt);
	printf("Deq   Count           = %10u\n", stats->deq_cnt);
	printf("Full  Count           = %10u\t", stats->full_cnt);
	printf("Empty Count           = %10u\n", stats->empty_cnt);
	printf("Dy_ARR Size UP Count  = %10u\t",stats->dy_arr.resize_up_cnt);
	printf("Dy_ARR Size Down Count= %10u\n",stats->dy_arr.resize_down_cnt);
	printf("Dy_ARR Size UP Err    = %10u\t",stats->dy_arr.resize_up_err_cnt);
	printf("Dy_ARR Size Down Err  = %10u\n",stats->dy_arr.resize_down_err_cnt);
	/* Keep stats as read on clear */
	memset(stats, 0, sizeof(dq_statistics));
}

/*--------------------API Functions(Etart)--------------------*/
#endif //_DY_QUEUE_DS_
