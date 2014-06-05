#ifdef _QUEUE_DS_
#error "There can only be one Static Queue usage"
#else
#define _QUEUE_DS_
#include <stdio.h>
#include <stdlib.h>  /* atoi  */
#include <stdbool.h> /* bool, true, false */
#include <ctype.h>   /* isspace */
#include <string.h>  /* memset */
#include <limits.h>  /* INT_MAX */

typedef struct queue_stats
{
	unsigned enq_cnt;
	unsigned deq_cnt;
	unsigned full_cnt;
	unsigned empty_cnt;
}queue_stats;

#ifndef QUEUE_PRINT_TYPE
#define QUEUE_PRINT_TYPE(_data_) printf("%d\n", _data_)
#endif
#ifndef SQ_OBJ_TYPE
#define SQ_OBJ_TYPE int
#endif
#ifndef SQ_MAX_ENTRIES
#define SQ_MAX_ENTRIES 20
#endif



/*--------------------Internal Functions(Start)--------------------*/
static unsigned int Q_N;
static unsigned int sq_head = 0;/* Index of the first element in queue */
static unsigned int sq_tail = 0;
static SQ_OBJ_TYPE q_elems[(SQ_MAX_ENTRIES)];
static queue_stats sq_stats;

static inline bool is_sq_empty_impl(int n){return n == 0;}
static inline bool is_sq_full_impl(int n) {return n == SQ_MAX_ENTRIES;}
static inline int  sq_size_impl()         {return SQ_MAX_ENTRIES;}
static inline SQ_OBJ_TYPE sq_peek_impl()  {return q_elems[sq_head];}
static inline int  sq_count_impl()        {return Q_N;}
static inline void sq_enq_impl(SQ_OBJ_TYPE x)
{
	q_elems[sq_tail++] = x;
	if(sq_tail == sq_size_impl()) sq_tail = 0;
	Q_N++;
}
static inline SQ_OBJ_TYPE sq_deq_impl()
{
	SQ_OBJ_TYPE dq_elem = q_elems[sq_head++];
	if(sq_head == sq_size_impl()) sq_head = 0;
	Q_N--;
	return dq_elem;
}
static inline void  sq_iterator_impl(void (*fn)(SQ_OBJ_TYPE *x))
{
	unsigned int i, cnt;
	if(fn == NULL)
		return;
	for(i = sq_head, cnt = 0; cnt < Q_N; cnt++, (i=(i+1) % sq_size_impl()))
		fn(&(q_elems[i]));
}


/*--------------------Internal Functions(End)--------------------*/
/*--------------------API Functions(Start)--------------------*/
void sq_print_element(SQ_OBJ_TYPE *x)       {QUEUE_PRINT_TYPE(*x);}
bool is_sq_empty()                          {return is_sq_empty_impl(Q_N);}
bool is_sq_full()                           {return is_sq_full_impl(Q_N);}
void sq_iterator(void (*fn)(SQ_OBJ_TYPE *x)){sq_iterator_impl(fn);} 
int  sq_count()                             {return sq_count_impl();}
int  sq_size()                              {return sq_size_impl();}
bool sq_enq(SQ_OBJ_TYPE x)
{                              
	if(is_sq_full() == false) {
		sq_enq_impl(x);
		sq_stats.enq_cnt++;
		return true;
	}
	sq_stats.full_cnt++;
	return false;
}
bool sq_deq(SQ_OBJ_TYPE *x)
{
	if(is_sq_empty() == false) {                                        
		*x = sq_deq_impl();
		sq_stats.deq_cnt++;
		return true;
	}
	sq_stats.empty_cnt++;
	return false; 
}
void sq_print_stats(queue_stats *stats)
{
	printf("Static Queue Statistics:\n");
	printf("Enq   Count = %u\n", stats->enq_cnt);
	printf("Deq   Count = %u\n", stats->deq_cnt);
	printf("Full  Count = %u\n", stats->full_cnt);
	printf("Empty Count = %u\n", stats->empty_cnt);
	/* Keep stats as read on clear */
	memset(stats, 0, sizeof(queue_stats));
}

/*--------------------API Functions(Etart)--------------------*/
#endif //_QUEUE_DS_
