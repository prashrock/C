#ifndef _UNION_FIND_DS_
#define _UNION_FIND_DS_
#include <stdio.h>
#include <stdlib.h>  /* atoi  */
#include <stdbool.h> /* bool, true, false */
#include <ctype.h>   /* isspace */
#include <string.h>  /* memset */
#include <assert.h>  /* assert */
#include <limits.h>  /* INT_MAX */
#include <stdint.h>  /* uint32_t */
#include "bit_ops.h" /* bit_align_up_nearest_pow2 */
#define UF_DS_RESIZE_MULTIPLIER 2

/* Union-Find DS for Dynamic Connectivity                *
 * Weighted Union-Find (QuickUnion with Path Compression)*
 * Notes on Implementation:                              *
 * + Can enable dynamic UF scaling with a run-time flag  *
 * + If UF Scaling disabled, 0 performace hit (no checks)*
 * - Not Multi-Thread Safe (caller to handle MT safety)  *
 * - UF DS cannot handle connection delete (!union)      *
 * = Element ID's and Component ID's are 0 based         */

enum UF_FLAGS_BITMAP {
	/* Container=Dynamic Array */
	UF_DS_SIZE_UNLIMITED_BITMASK=1<<0,
};

/* For cache optimality, below struct could be split to  *
 * seperate arrays (ID[], rank[]) to allow ID's to be    *
 * physically contiguous lending to (hopefully :)) lesser*
 * cache misses when doing union() and find()            *
 * Can easily fix this when using this API in production */
typedef struct
{
	int id;   /* id[i]   = parent of i, 0 based          */
	int rank; /* rank[i] = rank of sub-tree rooted at i  */
}uf_elem_t;
typedef struct
{
	unsigned int find_cnt; /* #Find operations on UF DS  */
	unsigned int resize_cnt; /* #resize up ops on UF DS  */
	unsigned int find_err_cnt; /* #find errors           */
	unsigned int resize_err_cnt; /* #resize up errors    */
}uf_stats_t;
typedef struct
{
	unsigned int elem_cnt;/* #Elements in UF DS          */
	unsigned int cmp_cnt; /* #Components in UF DS        */
	uint32_t flags;       /* UF flags(dynamic scale, etc)*/
	uf_stats_t stats;     /* UF DS statistics            */
	uf_elem_t elems[0];   /* Id/Rank Table for each elem */
}uf_t;

/* Print the elements root ID and rank                   */
static inline void uf_print_id_rank(uf_elem_t *x)
{ printf("ID = %d, Rank = %d\n", x->id, x->rank); }
/* Print the elements root ID alone                      */
static inline void uf_print_id(uf_elem_t *x)
{ printf(" %d,", x->id); }

/*--------------------Internal Functions(Start)--------------------*/
/* Create an empty UF  DS. #elems = align_pow2(elem_cnt) *
 * Initializes Union Find DS (no connections specified)  *
 * Without connections each element is contained in an   *
 * isolated component (element's ID == component root)   */
static inline uf_t *uf_create_impl(unsigned int elem_cnt)
{
	unsigned int mem_size, i;
	uf_t *uf;
	/* Despite User requested size align to power of two */
	elem_cnt  = bit_align_up_nearest_pow2(elem_cnt);
	mem_size  = sizeof(uf_t) + elem_cnt * sizeof(uf_elem_t);
	uf        = malloc(mem_size);
	if(uf)
	{
		memset(uf, 0 , mem_size);
		uf->elem_cnt = elem_cnt;
		uf->cmp_cnt  = elem_cnt;
		for(i = 0; i < uf->elem_cnt; i++)
			uf->elems[i].id = i;
	}
	return uf;
}
/* Destroy the Union Find DS and free associated memory  */
static inline void uf_destroy_impl(uf_t *uf)
{ free(uf); }
/* Dynamic Array size up implementation. If find() goes  *
 * outside array bounds and if unlimited size flag is    *
 * specified, resize up the container.                   */
static inline uf_t *uf_resize_up_impl(uf_t *old_uf,
								   unsigned int new_elem_cnt)
{
	uf_t *new_uf = uf_create_impl(new_elem_cnt);
	if(new_uf == NULL)
	{
		old_uf->stats.resize_err_cnt++;
		return old_uf;
	}
	else
	{
		new_elem_cnt = new_uf->elem_cnt;
		/* Copy all old elements - no amortized benefits */
		memcpy(new_uf->elems, old_uf->elems,
			   (sizeof(uf_elem_t) * old_uf->elem_cnt));
		/* Copy old UF structure to new and update cnt   */
		memcpy(new_uf, old_uf, sizeof(uf_t));
		new_uf->elem_cnt = new_elem_cnt;
		/* Update Component count from Old UF structure  */
		new_uf->cmp_cnt += new_uf->elem_cnt - old_uf->elem_cnt;
		/* Old UF DS is now obsolete - can be freed      */
		uf_destroy_impl(old_uf);
		new_uf->stats.resize_cnt++;
		return new_uf;
	}
}
/* Each element should belong to one component. Locate   *
 * the root element for this component.                  *
 * During each traversal do path compression by storing  *
 * immediate parent's id in place                        *
 * Take care of re-sizing UF DS if needed                *
 * Returns:                                              *
 * component ID (0 based) on success, -1 on failure      */
static inline int uf_find_impl(uf_t **uf_ptr, int p)
{
	uf_elem_t *elem;
	uf_t *uf = *uf_ptr;
	/* Validate element IDs before fetching Component IDs*/
	if(p < 0)
		goto uf_find_err;
	else if(p >= uf->elem_cnt)
	{
		/* Check for Dynamic resizing option             */
		if((uf->flags & UF_DS_SIZE_UNLIMITED_BITMASK))
		{
			uf = uf_resize_up_impl(uf, p+1);
			if(uf == NULL) goto uf_find_err;
			/* Reset User's pointer to new resized UF DS */
			*uf_ptr = uf;
		}
		else
			goto uf_find_err;
	}
	elem = uf->elems;
	while(p != elem[p].id)
	{
		/* Use Path Compression by Halving during lookup */
		elem[p].id = elem[elem[p].id].id;
		p = elem[p].id;
	}
	uf->stats.find_cnt++;
	return p;
uf_find_err:
	uf->stats.find_err_cnt++;
	return -1;
}
/* Merge two components containing elements 'p' & 'q'    *
 * Weighted-Quick Union impl to avoid tall trees         *
 * Link root of the smaller tree to root of larger tree  */
static inline bool uf_union_impl(uf_t **uf_ptr, int p, int q)
{
	uf_t *uf;
	uf_elem_t *elems;
	int i, j;
    /* Retrieve Component ID's for p and q respectively  */
	i = uf_find_impl(uf_ptr, p);
	j = uf_find_impl(uf_ptr, q);
	/* uf_ptr can change, use compile time barrier for MT*/
	uf = *uf_ptr;
	elems = uf->elems;
	/* If element ID's are equal, no union required      */
	if(p == q)         return false;
	/* If component ID lookup failed, union not possible */
	if(i < 0 || j < 0) return false;
	/* If elements already in same component, return     */
	if(i == j)         return true;
	/* Make root of smaller rank point to root of larger *
	 * rank. When both components are equal in size, we  *
	 * have to promote one component to be the parent    *
	 * thereby bumping up its rank.                      */
	if     (elems[i].rank < elems[j].rank) elems[i].id = j;
	else if(elems[i].rank > elems[j].rank) elems[j].id = i;
	else{
		elems[j].id = i;
		elems[i].rank++;
	}
	uf->cmp_cnt--;
	return true;
}
/* Two elements are connected i.e., in the same component *
 * if they have the same parent. Solved using find()      */
static inline bool uf_is_connected_impl(uf_t **uf_ptr, int p, int q)
{
	return (uf_find_impl(uf_ptr, p) == uf_find_impl(uf_ptr, q));
}
/* API for user to iterate through Element ID table       */
static inline void uf_iterator_impl(uf_t *uf, void (*fn)(uf_elem_t *x))
{
	int i;
	if(fn == NULL)
		return;
	for(i = 0; i < uf->elem_cnt; i++)
		fn(&(uf->elems[i]));
}
/*--------------------Internal Functions(End)--------------------*/
/*--------------------API Functions(Start)--------------------*/
/* Create an UF DS with elem_cnt elements and components  */
static inline uf_t *uf_create(int elem_cnt)
{ return uf_create_impl(elem_cnt); }
/* Remove UF DS and all its elements                      */
static inline void uf_destroy(uf_t *uf)
{ return uf_destroy_impl(uf); }
/* Returns the number of Elements present                 */
static inline unsigned int uf_elem_count(uf_t *uf)
{ return uf->elem_cnt; }
/* Returns the number of Components present               */
static inline unsigned int uf_comp_count(uf_t *uf)
{ return uf->cmp_cnt; }
/* Locate root of component or return -1 on error         */
static inline int uf_find(uf_t **uf_ptr, int p)
{ return uf_find_impl(uf_ptr, p); }
/* Merge two element ID's (i.e., merge two components)    */
static inline bool uf_union(uf_t **uf_ptr, int p, int q)
{ return uf_union_impl(uf_ptr, p, q); }
/* Check if 2 elements are connected (in same component)  */
static inline bool uf_is_connected(uf_t **uf_ptr, int p, int q)
{ return uf_is_connected_impl(uf_ptr, p, q); }
/* API for user to iterate through Element ID table       */
static inline void uf_iterator(uf_t *uf, void(*fn)(uf_elem_t *x))
{uf_iterator_impl(uf, fn);}
/* Enable Dynamic Array support for UF. This feature      *
 * scales up the Array size only if either one of         *
 * uf_union() operands is beyond the scope of the existing*
 * array.                                                 *
 * Note: Resize down is not supported                     */
static inline void uf_set_size_unlimited(uf_t *uf)
{ uf->flags |= UF_DS_SIZE_UNLIMITED_BITMASK; }
/* Prevent UF to resize up anymore. (Default = on)        */
static inline void uf_set_size_limited(uf_t *uf)
{ uf->flags &= ~(uint32_t)UF_DS_SIZE_UNLIMITED_BITMASK; }
/* Clear UF statistics                                    */
static inline void uf_clear_stats(uf_t *uf)
{ memset(&uf->stats, 0, sizeof(uf_stats_t)); }
static inline void uf_print_stats(uf_t *uf)
{
	printf("Union Find DS Statistics:\n");
	printf("#Elements             = %10u\n",uf_elem_count(uf));
	printf("#Components           = %10u\n",uf_comp_count(uf));
	printf("#Find operations      = %10u\n",uf->stats.find_cnt);
	printf("#Find errors          = %10u\n",uf->stats.find_err_cnt);
	if((uf->flags & UF_DS_SIZE_UNLIMITED_BITMASK))
	{
		printf("#Resize up operations = %10u\n",uf->stats.resize_cnt);
		printf("#Resize Up errors     = %10u\n",uf->stats.resize_err_cnt);
	}
	//printf("Flags                 = %10x\n",uf->flags);
}
/*--------------------API Functions(End)--------------------*/
#endif //_UNION_FIND_DS_
