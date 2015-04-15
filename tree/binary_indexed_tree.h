#ifndef _DY_BI_TREE_DS_
#define _DY_BI_TREE_DS_
#include <stdio.h>
#include <stdlib.h>        /* atoi  */
#include <stdbool.h>       /* bool, true, false */
#include <ctype.h>         /* isspace */
#include <string.h>        /* memset */
#include <assert.h>        /* assert */
#include <limits.h>        /* INT_MAX */
#include <malloc.h>        /* malloc, free */
#include "compiler_api.h"  /* likely, MAX */

/* Defines a Binary Indexed Tree (BIT) aka Fenwick Tree               *
 * A BIT only stores meta information for each index of the original  *
 * dataset. With this meta information sum upto n operations can be   *
 * completed at O(log n) time instead of O(n) time                    *
 * This is a static BI Tree API for study purposes. This can easily be*
 * extended to a dynamic DS if required                               */
struct bi_tree
{
	size_t n;
	int tree[];
};

/*--------------------Helper Functions(Start)--------------------*/
/* Get #elements in BI_Tree. Time Complexity = O(1) */
static inline unsigned bi_tree_size_api(struct bi_tree *bi)
{
	if(bi == NULL) return 0;
	else		   return bi->n;
}
/*--------------------Helper Functions(End)--------------------*/
/*--------------------Internal Functions(Start)--------------------*/
/* Create the BI Tree container */
static inline struct bi_tree *bi_tree_init_impl(size_t n)
{
	struct bi_tree *bi;
	bi = malloc(sizeof(struct bi_tree) + sizeof(int[n+1]));
	if(bi)
		bi->n = n;
	return bi;
}
/* Update an index in the BI Tree. This involves propagating this bit *
 * to all other log(n) positions. Time Complexity = O(log n)          *
 * Note, idx is 1 based                                               */
static inline void bi_tree_update_impl(struct bi_tree *bi, int val, int idx)
{
	while(idx <= bi->n) {
		bi->tree[idx] += val; /* Add val to current node in BI Tree */
		idx += idx & (-idx);  /* Move to parent node of BI Tree Index */
	}
}
/* Sum up all elements till this element                              *
 * Note, idx is 1 based                                               */
static inline long bi_tree_get_sum_impl(struct bi_tree *bi, int idx)
{
	long sum = 0;
	while(idx > 0) {
		sum += bi->tree[idx]; /* Add current element to sum */
		idx -= idx & (-idx);  /* Move to parent node of BI Tree Index */
	}
	return sum;
}
/* Given an array and a BI Tree of sufficient size                    *
 * Initialize BI Tree. Time Complexity = O(n log n)                   */
static inline void bi_tree_create_impl(struct bi_tree *bi,
										   int arr[], size_t n)
{
	int i;
	bi->n = n;                             /* Size init */
	memset(bi->tree, 0, sizeof(int[n+1])); /* Clear BI Tree contents */
	for(i = 0; i < bi->n; i++)	           /* Update BI Tree */
		bi_tree_update_impl(bi, arr[i], i+1);
}
/*--------------------Internal Functions(End)--------------------*/
/*--------------------API Functions(start)--------------------*/
/* Initialize a Binary Indexed Tree, do not add any elements yet */
struct bi_tree *bi_tree_init(size_t n)   {return bi_tree_init_impl(n);}
unsigned bi_tree_size(struct bi_tree *bi){return bi_tree_size_api(bi);}
void bi_tree_destroy(struct bi_tree *bi) {free(bi); }
/* Initialize a Binary Indexed Tree with given input */
static struct bi_tree *bi_tree_create(int arr[], size_t n)
{
	struct bi_tree *bi =  bi_tree_init(n);
	if(bi)
		bi_tree_create_impl(bi, arr, n);
	return bi;
}
/* Insert one new element into the BI Tree at a particular index      *
 * idx is zero based                                                  */
static inline void bi_tree_insert(struct bi_tree *bi, int val, int idx)
{
	if(bi == NULL) return;
	bi_tree_update_impl(bi, val, idx + 1);
}
/* Delete one new element from a particular index (idx is zero based) */
static inline void bi_tree_delete(struct bi_tree *bi, int idx)
{
	if(bi == NULL) return;
	bi_tree_update_impl(bi, 0, idx + 1);
}
/* Get the sum of all elements till this index (idx is zero based)    */
static long bi_tree_get_sum(struct bi_tree *bi, int idx)
{
	if(bi == NULL) return 0;
	return bi_tree_get_sum_impl(bi, idx + 1);
}



/*--------------------API Functions(end)--------------------*/
#endif //_DY_BI_TREE_DS_
