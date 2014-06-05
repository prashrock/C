#ifndef _SHELL_SORT_H
#define _SHELL_SORT_H
#include <stdio.h>
#include <stdlib.h>         /* atoi  */
#include <stdbool.h>        /* bool, true, false */
#include <ctype.h>          /* isspace */
#include <string.h>         /* memset */
#include <assert.h>         /* assert */
#include <limits.h>         /* INT_MAX */

#ifndef SHELL_SORT_OBJ_TYPE
#define SHELL_SORT_OBJ_TYPE  int
#endif


/* Return true if x < y; else return false */
static inline bool def_shell_sort_inc_comp(SHELL_SORT_OBJ_TYPE *x,
										   SHELL_SORT_OBJ_TYPE *y)
{
	if( *x < *y) return true;
	else         return false;
}

/* Shell sort - do 'gap' insertion-sort with gap of diff sizes      *
 * 'gap' size is determined by different formulae (Knuth, Pratt)    *
 * below code uses Knuth formula (O(n^(3/2)))                       *
 * + Fixed size code print with sub quadratic time complexity       *
 * - Stride lengths > 1, not very good for linked-lists             *
 * Time Complexity  = O(n^(3/2)) (non-trivial analysis)             *
 * Space Complexity = O(1)                                          *
 * Best for         = Partially sorted arrays                       */
static inline void
shell_sort(SHELL_SORT_OBJ_TYPE objs[], unsigned int n,
		   bool (*cmp)(SHELL_SORT_OBJ_TYPE *x, SHELL_SORT_OBJ_TYPE *y))
{
	int i, j, gap;
    /* Calculate gap with Knuth formula, try gap = 1, 4, 13, 40, etc */
	for(gap = 1; gap <= (n-1)/9; gap = 3*gap+1);
	for(; gap > 0; gap /= 3)
	{
		//Insertion GAP-Sort
		for(i = gap; i < n; i++)
		{
			for(j = i; j >= gap; j -= gap)/* if(objs[j]<objs[j-gap]) */
				if(cmp(&objs[j], &objs[j - gap]))
					SWAP(objs[j], objs[j - gap]);
				else break;
		}
	}
}

#endif //_SHELL_SORT_H
