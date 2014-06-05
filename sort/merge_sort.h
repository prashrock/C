#ifndef _MERGE_SORT_H_
#define _MERGE_SORT_H_
#include <stdio.h>
#include <stdlib.h>         /* atoi  */
#include <stdbool.h>        /* bool, true, false */
#include <ctype.h>          /* isspace */
#include <string.h>         /* memset */
#include <assert.h>         /* assert */
#include <limits.h>         /* INT_MAX */

#ifndef MERGE_SORT_OBJ_TYPE
#define MERGE_SORT_OBJ_TYPE  int
#endif

/* Return true if x < y; else return false */
static inline bool def_ms_inc_comp(MERGE_SORT_OBJ_TYPE *x,
										   MERGE_SORT_OBJ_TYPE *y)
{
	if( *x < *y) return true;
	else         return false;
}

static inline void merge(MERGE_SORT_OBJ_TYPE objs[], MERGE_SORT_OBJ_TYPE tmp[],
		   bool (*cmp)(MERGE_SORT_OBJ_TYPE *x, MERGE_SORT_OBJ_TYPE *y),
		   int l, int mid, int hi)
{
	//Pre-condition is a[l..mid] and a[mid+1...hi] are sorted
	int i = l;
	int j = mid+1;
	int k;	
	for(k = l; k <= hi; k++) tmp[k] = objs[k];
	for(k = l; k <= hi; k++)
	{
		if(i > mid)                    objs[k] = tmp[j++];
		else if(j > hi)                objs[k] = tmp[i++];
		else if(cmp(&tmp[j], &tmp[i])) objs[k] = tmp[j++];
		else                           objs[k] = tmp[i++];
	}
	//Post-condition a[l..hi] is sorted
}

/* Merge sort with recursion                                        *
 * Time Complexity  = O(n lg n)                                     *
 * Space Complexity = O(1)                                          *
 * Best for         =                                               */
static inline void
merge_sort_recurse(MERGE_SORT_OBJ_TYPE objs[], unsigned int n,
		   bool (*cmp)(MERGE_SORT_OBJ_TYPE *x, MERGE_SORT_OBJ_TYPE *y))
{
	return;
}

/* Merge sort without recursion                                     *
 * Best for         =                                               */
static inline void
merge_sort(MERGE_SORT_OBJ_TYPE objs[], unsigned int n,
		   bool (*cmp)(MERGE_SORT_OBJ_TYPE *x, MERGE_SORT_OBJ_TYPE *y))
{
	unsigned int l, m = 1, r = n-1;
	MERGE_SORT_OBJ_TYPE *tmp = malloc(n * sizeof(MERGE_SORT_OBJ_TYPE));
	if(tmp == NULL)
	{
		printf("Error: merge_sort(%d) -- malloc() failed\n", n);
		return;
	}
	for(m = 1; m < n; m = m+m)
	{
		for(l = 0; l < (n - m); l += (m+m))
			//objs[l..l+m-1] and objs[l+m...l+m+m-1] are two sub-arrays
			merge(objs, tmp, cmp, l, l+m-1, MIN((l+m+m-1), r));
	}
	free(tmp);
}


#endif //_MERGE_SORT_H_
