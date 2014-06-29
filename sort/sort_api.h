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

#endif //_SORT_API_H_
