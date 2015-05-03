#ifndef _SORT_API_H_
#define _SORT_API_H_
#include <stdio.h>
#include <stdlib.h>         /* atoi  */
#include <stdbool.h>        /* bool, true, false */
#include <ctype.h>          /* isspace */
#include <string.h>         /* memset */
#include <assert.h>         /* assert */
#include <limits.h>         /* INT_MAX */

/* Wrapper to collectively try different sorts */

#ifndef SORT_OBJ_TYPE
#define SORT_OBJ_TYPE   int
#endif


enum SORT_TYPE {
	SELECTION_SORT = 0,
	INSERTION_SORT,
	SHELL_SORT,
	MERGE_SORT_RECURSE,
	MERGE_SORT_ITERATE,
	MERGE_SORT_RECURSE_INV_CNT,
	INVALID_SORT,
};

static const char *sort_name[] = 
{
	"Selection Sort",
	"Insertion Sort",
	"Shell Sort",
	"Merge Sort - recurse",
	"Merge Sort - iterate",
	"Merge Sort - recurse + inv_cnt",
};

static const char *sort_name_abbrv[] = 
{
	"Select",
	"Insert",
	"Shell",
	"Mrg(Rec)",
	"Mrg(Itr)",
	"Mrg(R+Inv)",
};

static inline bool sort_api(enum SORT_TYPE st,
							SORT_OBJ_TYPE objs[], unsigned int cnt)
{
	bool ret = true;
	switch(st)
	{
	case SELECTION_SORT:
		selection_sort(objs, cnt, def_ss_inc_comp);
		break;
	case INSERTION_SORT:
		insertion_sort(objs, cnt, def_is_inc_comp);
		break;
	case SHELL_SORT:
		shell_sort(objs, cnt, def_shell_sort_inc_comp);
		break;
	case MERGE_SORT_RECURSE:
		merge_sort_recurse(objs, cnt, def_ms_inc_comp);
		break;
	case MERGE_SORT_ITERATE:
		merge_sort(objs, cnt, def_ms_inc_comp);
		break;
	case MERGE_SORT_RECURSE_INV_CNT:
		merge_sort_recurse_inversion_cnt(objs, cnt, def_ms_inc_comp);
		break;
	default:
		printf("Error: Invalid sort type %d\n", st);
		ret = false;
	}
	return ret;
}

/* Given a sorted dynamic array, check if elements are in sorted order    *
 * If total of elements is provided, check if the elements in the dynamic *
 * array sum up to this value                                             */
static inline bool validate_sort_order(const int *array, int n,
								unsigned long long total)
{
	int i;
	bool total_check = true;
	if(n == 0)             return true; /* Nothing to validate */
	if(total == 0)         total_check = false;
	total -= array[0];
	for(i = 1; i < n; i++)
	{
		if(array[i] < array[i-1])
		{
			printf("Error: Sort Invariant not met  - ");
			printf("%d found before %d\n", array[i-1], array[i]);
			return false;
		}
		total -= array[i];
	}
	/* Avoid checking if user wanted to do total checks until now */
	if(total_check && total) {
		printf("Error: SUM(Array) < User_Total (by %llu)\n", total);
		return false;
	}
	return true;
}


/* Find element which re-occurs the most. If more than one mode   *
 * present, return the first occuring mode                        */
static inline bool find_mode_in_sorted_array(const int *array, int n,
											int *mode, int *mode_freq)
{
	int i, gmode, lmode;
	unsigned gmode_freq, lmode_freq;
	if(n == 0)             return true;  /* Nothing to validate */
	if(array == NULL)      return false; /* Empty array - errro */
	/* Make sure the Input array is already sorted              */
	if(validate_sort_order(array, n, 0) == false) {
		return false;
	}
	gmode = lmode = array[0];
	gmode_freq = lmode_freq = 1;
	for(i = 1; i < n; i++)
	{
		if(array[i] == lmode) lmode_freq++;
		else {
			if(lmode_freq > gmode_freq) {
				gmode_freq = lmode_freq;
				gmode = lmode;
			}
			lmode = array[i];
			lmode_freq = 1;
		}
	}
	if(mode)      *mode = gmode;
	if(mode_freq) *mode_freq = gmode_freq;
	return true;
}

/* Find first or last occurence of particular number in the given *
 * sorted array (invariant).                                      *
 * Time Complexity = O(log n) very similar to binary search       */
static inline int find_first_or_last_idx(const int *array, int n,
										  int k, bool first)
{
	int low = 0, high = n-1, mid;
	int idx = -1;
	if(n == 0 || array == NULL) /* Sanity Checks                  */
		return idx;
	while(low <= high)
	{
		bool go_low;
		mid = low + (high - low) / 2;
		if(array[mid] < k)      /* Search only in right sub-tree  */
			go_low = false;
		else if(array[mid] > k) /* Search only in left sub-tree   */
			go_low = true;
        /* Found element. Binary search ends here, we go further  */
		else {
			idx = mid;
			go_low = first;
		}
		/* Based on decisions above, adjust low or high           */
		if(go_low) high = mid - 1;
		else       low  = mid + 1;
	}
	return idx;
}
#endif //_SORT_API_H_
