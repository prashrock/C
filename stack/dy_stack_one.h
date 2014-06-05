#ifdef _DY_STACK_DS_
#error "There can only be one Dynamic Stack usage per file"
#else
#define _DY_STACK_DS_
#include <stdio.h>
#include <stdlib.h>  /* atoi  */
#include <stdbool.h> /* bool, true, false */
#include <ctype.h>   /* isspace */
#include <string.h>  /* memset */
#include <assert.h>  /* assert */
#include <limits.h>  /* INT_MAX */

typedef struct dy_arr_stats
{
	unsigned resize_up_cnt;
	unsigned resize_down_cnt;
	unsigned resize_up_err_cnt;
	unsigned resize_down_err_cnt;
}dy_arr_stats;

typedef struct dy_stack_stats
{
	unsigned push_cnt;
	unsigned pop_cnt;
	unsigned full_cnt;
	unsigned empty_cnt;
	dy_arr_stats dy_arr;
}dy_stack_stats_t;

#ifndef STACK_PRINT_TYPE
#define STACK_PRINT_TYPE(_data_) printf("%d\n", _data_)
#endif
#ifndef DY_OBJ_TYPE
#define DY_OBJ_TYPE int
#endif

#define EMPTY_STACK (-1)
#define DY_STACK_MULTIPLIER (2)

/*--------------------Internal Functions(Start)--------------------*/
static int dy_top = EMPTY_STACK;
static int dy_size = 0;
static dy_stack_stats_t dy_stack_stats;
static DY_OBJ_TYPE *dy_items;
static inline void dy_push_impl(DY_OBJ_TYPE x) {dy_items[++dy_top] = x;}
static inline DY_OBJ_TYPE dy_pop_impl()   {return dy_items[dy_top--];}
static inline bool dy_full_impl(int top)  {return top+1 == dy_size;}
static inline bool dy_empty_impl(int top) {return top == EMPTY_STACK;}
static inline int  dy_stack_size_impl()   {return dy_size;}
static inline int  dy_stack_count_impl()  {return dy_top+1;}
static inline bool dy_stack_resize_impl(int new_size)
{
    int i;
	DY_OBJ_TYPE *new_items;
	if(new_size == 0) new_size = 1;
	assert(dy_stack_size_impl() >= dy_top);
	new_items =  malloc(sizeof(DY_OBJ_TYPE) * new_size);
	if(new_items == NULL)
		return false;
	for(i = 0; i <= dy_top; i++)
		new_items[i] = dy_items[i];
	free(dy_items);
	dy_size = new_size;
	dy_items = new_items;
	return true;
}
static inline void dy_stack_iterator_impl(void (*fn)(DY_OBJ_TYPE *x))
{
	if(fn == NULL)
		return; 
	int top_copy = dy_top;       
	while(dy_empty_impl(top_copy) == false)
		fn(&(dy_items[top_copy--]));
}
/*--------------------Internal Functions(End)--------------------*/
/*--------------------API Functions(Start)--------------------*/
static void dy_stack_print_element(DY_OBJ_TYPE *x) {STACK_PRINT_TYPE(*x);}
static inline int  dy_stack_count()    {return dy_stack_count_impl();}
static inline int  dy_stack_size()     {return dy_stack_size_impl();}
static inline bool is_dy_stack_empty() {return dy_empty_impl(dy_top);}	   
static inline bool is_dy_stack_full()  {return dy_full_impl(dy_top);}     
void dy_stack_iterator(void(*fn)(DY_OBJ_TYPE*x)){dy_stack_iterator_impl(fn);}
bool dy_stack_push(DY_OBJ_TYPE x) {
	if(is_dy_stack_full() == true) {
		dy_stack_stats.full_cnt++;
		if(dy_stack_size() == INT_MAX)
		{
			dy_stack_stats.dy_arr.resize_up_err_cnt++;
			return false;
		}
		if(dy_stack_resize_impl(dy_stack_size()*DY_STACK_MULTIPLIER)==false)
		{
			dy_stack_stats.dy_arr.resize_up_err_cnt++;
			return false;
		}
		dy_stack_stats.dy_arr.resize_up_cnt++;
	}
	dy_push_impl(x);
	dy_stack_stats.push_cnt++;
	return true;
}
static inline bool dy_stack_pop(DY_OBJ_TYPE *x) {
	if(is_dy_stack_empty()){
		dy_stack_stats.empty_cnt++;
		return false;
	}
	*x = dy_pop_impl();
	dy_stack_stats.pop_cnt++;
	if(dy_top > 0 && dy_top == dy_stack_size()/4)
	{
		if(dy_stack_resize_impl(dy_stack_size()/DY_STACK_MULTIPLIER)==false)
		{
			dy_stack_stats.dy_arr.resize_down_err_cnt++;
			return false;
		}
		dy_stack_stats.dy_arr.resize_down_cnt++;
	}
	return true;
}
static inline void dy_stack_print_stats(dy_stack_stats_t *stats)
{
	printf("Dynamic Stack Statistics:\n");
	printf("Stack Size            = %10u\t",dy_stack_size());
	printf("Stack Count           = %10u\n",dy_stack_count());
	printf("Push  Count           = %10u\t",stats->push_cnt);
	printf("Pop   Count           = %10u\n",stats->pop_cnt);
	printf("Full  Count           = %10u\t",stats->full_cnt);
	printf("Empty Count           = %10u\n",stats->empty_cnt);
	printf("Dy_ARR Size UP Count  = %10u\t",stats->dy_arr.resize_up_cnt);
	printf("Dy_ARR Size Down Count= %10u\n",stats->dy_arr.resize_down_cnt);
	printf("Dy_ARR Size UP Err    = %10u\t",stats->dy_arr.resize_up_err_cnt);
	printf("Dy_ARR Size Down Err  = %10u\n",stats->dy_arr.resize_down_err_cnt);
	/* Keep stats as read on clear */
	memset(stats, 0, sizeof(dy_stack_stats_t));
}
/*--------------------API Functions(Etart)--------------------*/
#endif //_DY_STACK_DS_
