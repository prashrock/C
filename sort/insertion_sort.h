#ifndef _INSERTION_SORT_H_
#define _INSERTION_SORT_H_
#include <stdio.h>
#include <stdlib.h>         /* atoi  */
#include <stdbool.h>        /* bool, true, false */
#include <ctype.h>          /* isspace */
#include <string.h>         /* memset */
#include <assert.h>         /* assert */
#include <limits.h>         /* INT_MAX */

#ifndef INSERTION_SORT_OBJ_TYPE
#define INSERTION_SORT_OBJ_TYPE  int
#endif


/* Return true if x < y; else return false */
static inline bool def_is_inc_comp(INSERTION_SORT_OBJ_TYPE *x,
										   INSERTION_SORT_OBJ_TYPE *y)
{
	if( *x < *y) return true;
	else         return false;
}

/* Insertion sort - For each element make sure its greater than prev elem  *
 * + Best case = Array already sorted, n-1 compares and 0 exchanges        *
 * - Worst case = Array in descending order, n^2/2 comps and n^2/2 xchgs   *
 * + For partially sorted arrays, Insertion sort runs in linear time       *
 * Time Complexity  = O(n^2)                                               *
 * Space Complexity = O(1)                                                 *
 * Best for         = Partially sorted arrays                              */
static inline void
insertion_sort(INSERTION_SORT_OBJ_TYPE objs[], unsigned int n,
			   bool (*cmp)(INSERTION_SORT_OBJ_TYPE *x, INSERTION_SORT_OBJ_TYPE *y))
{
	int i, j;
	for(i = 0; i < n; i++)
	{
		for(j = i; j > 0; j--)  /* if (objs[j] < objs[j-1]) */
			if(cmp(&objs[j], &objs[j - 1]))
				SWAP(objs[j], objs[j - 1]);
			else break;
	}
}

#endif //_INSERTION_SORT_H_
