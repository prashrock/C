#ifdef _QUEUE_POW2_DS_
#error "There can only be one Static Power_2 Queue usage"
#else
#define _QUEUE_POW2_DS_
#include <stdio.h>
#include <stdlib.h>          /* atoi  */
#include <stdbool.h>         /* bool, true, false */
#include <ctype.h>           /* isspace */
#include <string.h>          /* memset */
#include <limits.h>          /* INT_MAX */
#include "bit_ops.h"         /* IS_POW2 */

typedef struct queue_stats
{
	unsigned enq_cnt;
	unsigned deq_cnt;
	unsigned full_cnt;
	unsigned empty_cnt;
}sq2_statistics;

#ifndef QUEUE_PRINT_TYPE
#define QUEUE_PRINT_TYPE(_data_) printf("%d\n", _data_)
#endif
#ifndef SQ2_OBJ_TYPE
#define SQ2_OBJ_TYPE int
#endif
#ifndef SQ2_MAX_ENTRIES
#define SQ2_MAX_ENTRIES 20
#endif

#if !(IS_POW2(SQ2_MAX_ENTRIES))
#error "Static Pow 2 Queue must have Pow 2 entries"
#endif

/*--------------------Internal Functions(Start)--------------------*/
static unsigned int sq2_head = 0;/* Index of the first element in queue */
static unsigned int sq2_tail = 0;
static SQ2_OBJ_TYPE sq2_elems[(SQ2_MAX_ENTRIES)];
static sq2_statistics sq2_stats;

static inline int  sq2_size_impl()         {return SQ2_MAX_ENTRIES;}
static inline int  sq2_count_impl()
{
	return ((sq2_tail - sq2_head) & (sq2_size_impl() - 1));
}
static inline int sq2_space_impl()
{
	return sq2_count_impl(sq2_head, sq2_tail + 1, sq2_size_impl());
}
static inline bool is_sq2_empty_impl(){return sq2_count_impl() == 0;}
static inline bool is_sq2_full_impl()
{
	return sq2_count_impl() + 1 == SQ2_MAX_ENTRIES;
}
static inline SQ2_OBJ_TYPE sq2_peek_impl()  {return sq2_elems[sq2_head];}
static inline void sq2_enq_impl(SQ2_OBJ_TYPE x)
{
	sq2_elems[sq2_tail++ % sq2_size_impl()] = x;
}
static inline SQ2_OBJ_TYPE sq2_deq_impl()
{
	return sq2_elems[sq2_head++ % sq2_size_impl()];
}
static inline void  sq2_iterator_impl(void (*fn)(SQ2_OBJ_TYPE *x))
{
	unsigned int i, cnt;
	if(fn == NULL)
		return;
	for(i = sq2_head, cnt = 0; cnt < sq2_count_impl();
		cnt++, (i=(i+1) % sq2_size_impl()))
		fn(&(sq2_elems[i]));
}

/*--------------------Internal Functions(End)--------------------*/
/*--------------------API Functions(Start)--------------------*/
void sq2_print_element(SQ2_OBJ_TYPE *x)       {QUEUE_PRINT_TYPE(*x);}
bool is_sq2_empty()                           {return is_sq2_empty_impl();}
bool is_sq2_full()                            {return is_sq2_full_impl();}
void sq2_iterator(void (*fn)(SQ2_OBJ_TYPE *x)){sq2_iterator_impl(fn);} 
int  sq2_count()                              {return sq2_count_impl();}
int  sq2_size()                               {return sq2_size_impl();}
bool sq2_enq(SQ2_OBJ_TYPE x)
{                              
	if(is_sq2_full() == false) {
		sq2_enq_impl(x);
		sq2_stats.enq_cnt++;
		return true;
	}
	sq2_stats.full_cnt++;
	return false;
}
bool sq2_deq(SQ2_OBJ_TYPE *x)
{
	if(is_sq2_empty() == false) {                                        
		*x = sq2_deq_impl();
		sq2_stats.deq_cnt++;
		return true;
	}
	sq2_stats.empty_cnt++;
	return false; 
}
void sq2_print_stats(sq2_statistics *stats)
{
	printf("Static Pow 2 Queue Statistics:\n");
	printf("Enq   Count = %u\n", stats->enq_cnt);
	printf("Deq   Count = %u\n", stats->deq_cnt);
	printf("Full  Count = %u\n", stats->full_cnt);
	printf("Empty Count = %u\n", stats->empty_cnt);
	/* Keep stats as read on clear */
	memset(stats, 0, sizeof(sq2_statistics));
}

/*--------------------API Functions(Etart)--------------------*/
#endif //_QUEUE_POW2_DS_
