#ifndef _DY_STACK_DS_
#define _DY_STACK_DS_
#include <stdio.h>
#include <stdlib.h>  /* atoi  */
#include <stdbool.h> /* bool, true, false */
#include <ctype.h>   /* isspace */
#include <string.h>  /* memset */
#include <assert.h>  /* assert */
#include <limits.h>  /* INT_MAX */

#ifndef STACK_PRINT_TYPE
#define STACK_PRINT_TYPE(_data_) printf("%d\n", _data_)
#endif
#ifndef DY_STACK_OBJ_TYPE
#define DY_STACK_OBJ_TYPE int
#endif

#define EMPTY_STACK (-1)
#define DY_STACK_MULTIPLIER (2)

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

typedef struct dy_stack_stats
{
	unsigned push_cnt;
	unsigned pop_cnt;
	unsigned full_cnt;
	unsigned empty_cnt;
	dy_arr_stats dy_arr;
}dy_stack_stats_t;

typedef struct
{
	int top;
	int size;
	DY_STACK_OBJ_TYPE *elems;
	dy_stack_stats_t stats;
}dstack_t;


/*--------------------Internal Functions(Start)--------------------*/
static inline dstack_t *dy_stack_init_impl()
{
	dstack_t *ds = malloc(sizeof(dstack_t));
	if(ds)
	{
		memset(ds, 0 , sizeof(dstack_t));
		ds->top = EMPTY_STACK;
	}
	return ds;
}
static inline void dy_stack_destroy_impl(dstack_t *ds)
{
	if(ds)
	{
		if(ds->elems)
			free(ds->elems);
		free(ds);
	}
}
static inline void dy_push_impl(dstack_t *ds, DY_STACK_OBJ_TYPE x)
{ds->elems[++ds->top] = x;}
static inline DY_STACK_OBJ_TYPE dy_pop_impl(dstack_t *ds)
{return ds->elems[ds->top--];}
static inline bool dy_full_impl(dstack_t *ds, int top)
{return top+1 == ds->size;}
static inline bool dy_empty_impl(dstack_t *ds, int top)
{return top == EMPTY_STACK;}
static inline int  dy_stack_size_impl(dstack_t *ds)   {return ds->size;}
static inline int  dy_stack_count_impl(dstack_t *ds)  {return ds->top+1;}
static inline bool dy_stack_resize_impl(dstack_t *ds, int new_size)
{
    int i;
	DY_STACK_OBJ_TYPE *new_items;
	if(new_size == 0) new_size = 1;
	assert(dy_stack_size_impl(ds) >= ds->top);
	new_items =  malloc(sizeof(DY_STACK_OBJ_TYPE) * new_size);
	if(new_items == NULL)
		return false;
	for(i = 0; i <= ds->top; i++)
		new_items[i] = ds->elems[i];
	free(ds->elems);
	ds->size = new_size;
	ds->elems = new_items;
	return true;
}
static inline void dy_stack_iterator_impl(dstack_t *ds, void (*fn)(DY_STACK_OBJ_TYPE *x))
{
	if(fn == NULL)
		return; 
	int top_copy = ds->top;       
	while(dy_empty_impl(ds, top_copy) == false) fn(&(ds->elems[top_copy--]));
}
static inline void dy_stack_reverse_iterator_impl(dstack_t *ds,
												  void (*fn)(DY_STACK_OBJ_TYPE *x))
{
	if(fn == NULL)
		return;
	int top_copy = 0;
	while(top_copy < dy_stack_count_impl(ds))	fn(&(ds->elems[top_copy++]));
}
/*--------------------Internal Functions(End)--------------------*/
/*--------------------API Functions(Start)--------------------*/
static inline dstack_t *dy_stack_init()
{return dy_stack_init_impl();}
static inline void dy_stack_destroy(dstack_t *ds)
{return dy_stack_destroy_impl(ds);}
static inline void dy_stack_print_element(DY_STACK_OBJ_TYPE *x)
{STACK_PRINT_TYPE(*x);}
static inline int  dy_stack_count(dstack_t *ds)
{return dy_stack_count_impl(ds);}
static inline int  dy_stack_size(dstack_t *ds)
{return dy_stack_size_impl(ds);}
static inline bool is_dy_stack_empty(dstack_t *ds)
{return dy_empty_impl(ds, ds->top);}	   
static inline bool is_dy_stack_full(dstack_t *ds)
{return dy_full_impl(ds, ds->top);}     
void dy_stack_iterator(dstack_t *ds, void(*fn)(DY_STACK_OBJ_TYPE*x))
{dy_stack_iterator_impl(ds, fn);}
void dy_stack_reverse_iterator(dstack_t *ds, void(*fn)(DY_STACK_OBJ_TYPE*x))
{dy_stack_reverse_iterator_impl(ds, fn);}
static inline bool dy_stack_push(dstack_t *ds, DY_STACK_OBJ_TYPE x) {
	if(is_dy_stack_full(ds) == true) {
		ds->stats.full_cnt++;
		if(dy_stack_size(ds) == INT_MAX)
		{
			ds->stats.dy_arr.resize_up_err_cnt++;
			return false;
		}
		if(dy_stack_resize_impl(ds, dy_stack_size(ds)*DY_STACK_MULTIPLIER)==false)
		{
			ds->stats.dy_arr.resize_up_err_cnt++;
			return false;
		}
		ds->stats.dy_arr.resize_up_cnt++;
	}
	dy_push_impl(ds, x);
	ds->stats.push_cnt++;
	return true;
}
static inline bool dy_stack_pop(dstack_t *ds, DY_STACK_OBJ_TYPE *x) {
	if(is_dy_stack_empty(ds)){
		ds->stats.empty_cnt++;
		return false;
	}
	*x = dy_pop_impl(ds);
	ds->stats.pop_cnt++;
	if(ds->top > 0 && ds->top == dy_stack_size(ds)/4)
	{
		if(dy_stack_resize_impl(ds, dy_stack_size(ds)/DY_STACK_MULTIPLIER)==false)
		{
			ds->stats.dy_arr.resize_down_err_cnt++;
			return false;
		}
		ds->stats.dy_arr.resize_down_cnt++;
	}
	return true;
}
static inline void dy_stack_print_stats(dstack_t *ds)
{
	printf("Dynamic Stack Statistics:\n");
	printf("Stack Size            = %10u\t",dy_stack_size(ds));
	printf("Stack Count           = %10u\n",dy_stack_count(ds));
	printf("Push  Count           = %10u\t",ds->stats.push_cnt);
	printf("Pop   Count           = %10u\n",ds->stats.pop_cnt);
	printf("Full  Count           = %10u\t",ds->stats.full_cnt);
	printf("Empty Count           = %10u\n",ds->stats.empty_cnt);
	printf("Dy_ARR Size UP Count  = %10u\t",ds->stats.dy_arr.resize_up_cnt);
	printf("Dy_ARR Size Down Count= %10u\n",ds->stats.dy_arr.resize_down_cnt);
	printf("Dy_ARR Size UP Err    = %10u\t",ds->stats.dy_arr.resize_up_err_cnt);
	printf("Dy_ARR Size Down Err  = %10u\n",ds->stats.dy_arr.resize_down_err_cnt);
	/* Keep stats as read on clear */
	memset(&ds->stats, 0, sizeof(dy_stack_stats_t));
}
/*--------------------API Functions(Etart)--------------------*/
#endif //_DY_STACK_DS_
