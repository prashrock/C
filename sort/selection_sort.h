#ifndef _SELECTION_SORT_H_
#define _SELECTION_SORT_H_
#include <stdio.h>
#include <stdlib.h>         /* atoi  */
#include <stdbool.h>        /* bool, true, false */
#include <ctype.h>          /* isspace */
#include <string.h>         /* memset */
#include <assert.h>         /* assert */
#include <limits.h>         /* INT_MAX */

#ifndef SELECTION_SORT_OBJ_TYPE
#define SELECTION_SORT_OBJ_TYPE  int
#endif


/* Return true if x < y; else return false */
static inline bool def_ss_inc_comp(SELECTION_SORT_OBJ_TYPE *x,
										   SELECTION_SORT_OBJ_TYPE *y)
{
	if( *x < *y) return true;
	else         return false;
}

/* Selection sort - Identify smallest and swap with i                      *
 * n^2 comparisons and upto n exchanges                                    *
 * + Minimal data movement (each element is in its final position directly)*
 * - Quadratic time even if input is fully sorted                          *
 * Time Complexity  = O(n * (n - 1) / 2) = O(n^2)                          *
 * Space Complexity = O(1)                                                 * 
 * Best for         = Minimum data movement is preferred                   */
static inline void
selection_sort(SELECTION_SORT_OBJ_TYPE objs[], unsigned int n,
			   bool (*cmp)(SELECTION_SORT_OBJ_TYPE *x, SELECTION_SORT_OBJ_TYPE *y))
{
	int i, j, min;
	for(i = 0; i < n; i++)
	{
		min = i;
		for(j = i + 1; j < n; j++)
            /* if (objs[j] < obj[min]) min = j */
			if(cmp(&objs[j], &objs[min]))	min = j;
		if(i != min) /* Avoid swap if cur element is min */
			SWAP(objs[i], objs[min]);
	}
}

#endif //_SELECTION_SORT_H_
