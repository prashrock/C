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
						 unsigned int lo, unsigned int mid, unsigned int hi, bool cpy,
						 bool (*cmp)(MERGE_SORT_OBJ_TYPE *x, MERGE_SORT_OBJ_TYPE *y)
	)
{
	//Pre-condition is a[lo..mid] and a[mid+1...hi] are sorted
	unsigned int i = lo;
	unsigned int j = mid+1;
	unsigned int k;
	/* Below copy is avoided by swapping objs, for alternative calls *
	 * Same logic can be done for iterative calls but it is more     *
	 * complicated, so for now avoiding copy in recursive mode alone */
	if(cpy)
		for(k = lo; k <= hi; k++) tmp[k] = objs[k];
	for(k = lo; k <= hi; k++)
	{
		if(i > mid)                    objs[k] = tmp[j++];
		else if(j > hi)                objs[k] = tmp[i++];
		else if(cmp(&tmp[j], &tmp[i])) objs[k] = tmp[j++];
		else                           objs[k] = tmp[i++];
	}
	//Post-condition a[lo..hi] is sorted
}

/* Merge sort with recursion                                        *
 * Time Complexity  = O(n lg n)                                     *
 * Space Complexity = O(1)                                          */
static inline void
merge_sort_recurse_impl(MERGE_SORT_OBJ_TYPE src[], MERGE_SORT_OBJ_TYPE dst[],
						unsigned int lo, unsigned int hi,
						bool (*cmp)(MERGE_SORT_OBJ_TYPE *x, MERGE_SORT_OBJ_TYPE *y))
{
	unsigned int mid = lo + (hi - lo)/2;
	if(hi <= lo) return;
	merge_sort_recurse_impl(dst, src, lo, mid, cmp);
	merge_sort_recurse_impl(dst, src, mid+1, hi, cmp);
	merge(src, dst, lo, mid, hi, false, cmp);
}

/* Merge sort without recursion                                     *
 * Better efficiency than iterative approach since copy avoided in  *
 * each merge call                                                  *
 * 1M sort costs  = 345ms                                           *
 * 10M sort costs =   3s                                            */
static inline void
merge_sort_recurse(MERGE_SORT_OBJ_TYPE objs[], unsigned int n,
		   bool (*cmp)(MERGE_SORT_OBJ_TYPE *x, MERGE_SORT_OBJ_TYPE *y))
{
	MERGE_SORT_OBJ_TYPE *tmp = malloc(n * sizeof(MERGE_SORT_OBJ_TYPE));
	if(tmp == NULL)
	{
		printf("Error: merge_sort(%d) -- malloc() failed\n", n);
		return;
	}
	memcpy(tmp, objs, (n * sizeof(MERGE_SORT_OBJ_TYPE)));
	merge_sort_recurse_impl(objs, tmp, 0, n-1, cmp);
	free(tmp);
}

/* Merge sort without recursion                                     *
 * Best for = Large n (recursive stack depth grows logarithmically  *
 * Note = Can avoid copy from objs to tmp in each call of merge by  *
 * correctly swapping objs and tmp pointers across outer loop       *
 * 1M sort costs  = 388ms                                           *
 * 10M sort costs =   4s                                            */
static inline void
merge_sort(MERGE_SORT_OBJ_TYPE objs[], unsigned int n,
		   bool (*cmp)(MERGE_SORT_OBJ_TYPE *x, MERGE_SORT_OBJ_TYPE *y))
{
	unsigned int lo, m, hi = n-1;
	MERGE_SORT_OBJ_TYPE *tmp = malloc(n * sizeof(MERGE_SORT_OBJ_TYPE));
	if(tmp == NULL)
	{
		printf("Error: merge_sort(%d) -- malloc() failed\n", n);
		return;
	}
	for(m = 1; m < n; m = m+m)
	{		
		for(lo = 0; lo < (n - m); lo += (m+m))
			//objs[lo..lo+m-1] and objs[lo+m...lo+m+m-1] are two sub-arrays
			merge(objs, tmp, lo, lo+m-1, MIN((lo+m+m-1), hi), true, cmp);
	}
	free(tmp);
}

#endif //_MERGE_SORT_H_
