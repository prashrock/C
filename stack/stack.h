#ifdef _STACK_DS_
#error "There can only be one Static Stack usage"
#else
#define _STACK_DS_
#include <stdio.h>
#include <stdlib.h>  /* atoi  */
#include <stdbool.h> /* bool, true, false */
#include <ctype.h>   /* isspace */
#include <string.h>  /* memset */
#include <limits.h>  /* INT_MAX */

typedef struct stack_stats
{
	unsigned push_cnt;
	unsigned pop_cnt;
	unsigned full_cnt;
	unsigned empty_cnt;
}stack_stats;

#ifndef STACK_PRINT_TYPE
#define STACK_PRINT_TYPE(_data_) printf("%d\n", _data_)
#endif
#ifndef OBJ_TYPE
#define OBJ_TYPE int
#endif
#ifndef MAX_ENTRIES
#define MAX_ENTRIES 20
#endif


#define EMPTY_STACK (-1)


/*--------------------Internal Functions(Start)--------------------*/
static int top = EMPTY_STACK;
static stack_stats simple_stack_stats;
static OBJ_TYPE items[(MAX_ENTRIES)];
static inline void push_impl(OBJ_TYPE x)  {items[++top] = x;}
static inline OBJ_TYPE pop_impl()         {return items[top--];}
static inline bool full_impl(int top)     {return top+1 == MAX_ENTRIES;}
static inline bool empty_impl(int top)    {return top == EMPTY_STACK;}
static inline int  stack_size_impl()      {return MAX_ENTRIES;}
static inline int  stack_count_impl()     {return top+1;}
static inline void stack_iterator_impl(void (*fn)(OBJ_TYPE *x))
{
	if(fn == NULL)
		return; 
	int top_copy = top;       
	while(empty_impl(top_copy) == false)
		fn(&(items[top_copy--]));
}
/*--------------------Internal Functions(End)--------------------*/
/*--------------------API Functions(Start)--------------------*/
void stack_print_element(OBJ_TYPE *x) {STACK_PRINT_TYPE(*x);}	   	       
bool is_stack_empty()                 {return empty_impl(top);}	   
bool is_stack_full()                  {return full_impl(top);}     
void stack_iterator(void (*fn)(OBJ_TYPE *x)){stack_iterator_impl(fn);} 
int  stack_count()                     {return stack_count_impl();}          
int  stack_size()                      {return stack_size_impl();}
bool stack_push(OBJ_TYPE x) {                                              
	if(is_stack_full() == false) {
		push_impl(x);
		simple_stack_stats.push_cnt++;
		return true;
	}
	simple_stack_stats.full_cnt++;
	return false;
}
bool stack_pop(OBJ_TYPE *x) {
	if(is_stack_empty() == false) {                                        
		*x = pop_impl();
		simple_stack_stats.pop_cnt++;
		return true;
	}
	simple_stack_stats.empty_cnt++;
	return false; 
}
void stack_print_stats(stack_stats *stats)
{
	printf("Stack Statistics:\n");
	printf("Push  Count = %u\n", stats->push_cnt);
	printf("Pop   Count = %u\n", stats->pop_cnt);
	printf("Full  Count = %u\n", stats->full_cnt);
	printf("Empty Count = %u\n", stats->empty_cnt);
	/* Keep stats as read on clear */
	memset(stats, 0, sizeof(stack_stats));
}

/*--------------------API Functions(Etart)--------------------*/
#endif //_STACK_DS_

