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

/* Union-Find DS for Dynamic Connectivity                *
 * Weighted Union-Find (QuickUnion with Path Compression)*
 * implementation = not Multi-Thread safe                *
 * Note - DS cannot handle connection delete (!union)    */

enum UF_FLAGS {
	UF_DS_SIZE_UNLIMITED = 1,/* Container=Dynamic Array  */
};

/* DS is not cache optimal.This should be split into two *
 * seperate arrays (ID[], rank[]) to allow ID's to be    *
 * physically contiguous giving (hopefully) lesser cache *
 * misses when doing union() and find()                  *
 * Can easily fix this when using this API in production */
typedef struct
{
	int id;   /* id[i]   = parent of i, 0 based          */
	int rank; /* rank[i] = rank of sub-tree rooted at i  */
}uf_elem_t;

typedef struct
{
	unsigned int elem_cnt;/* #Elements in UF DS          */
	unsigned int cmp_cnt; /* #Components in UF DS        */
	uint32_t flags;       /* UF flags(dynamic scale, etc)*/
	uf_elem_t elems[0];   /* Id/Rank Table for each elem */
}uf_t;

/* Print the elements root ID and rank                   */
static inline void uf_print_id_rank(uf_elem_t *x)
{ printf("ID = %d, Rank = %d\n", x->id, x->rank); }
/* Print the elements root ID alone                      */
static inline void uf_print_id(uf_elem_t *x)
{ printf(" %d,", x->id); }

/*--------------------Internal Functions(Start)--------------------*/
/* Create an empty Union Find DS with 'cnt' elements     */
static inline uf_t *uf_create_impl(unsigned int cnt)
{
	unsigned int size = sizeof(uf_t) + (cnt * sizeof(uf_elem_t)); 
	uf_t *uf = malloc(size);
	if(uf)
		memset(uf, 0 , size);
	return uf;
}
/* Initializes Union Find DS (no connections specified)  *
 * Without connections each element is contained in an   *
 * isolated component (element's ID == component root)   */
static inline void uf_init_impl(uf_t *uf, unsigned int cnt)
{
	int i;
	if(!uf) return;
	uf->elem_cnt = cnt;
	uf->cmp_cnt = cnt;
	for(i = 0; i < uf->elem_cnt; i++)
		uf->elems[i].id = i;
}
/* Destroy the Union Find DS and free associated memory  */
static inline void uf_destroy_impl(uf_t *uf)
{ free(uf); }
/* Each element should belong to one component. Locate   *
 * the root element for this component.                  *
 * During each traversal do path compression by storing  *
 * immediate parent's id in place                        */
static inline int uf_find_impl(uf_t *uf, int p)
{
	uf_elem_t *elem;
	if(p < 0 || p >= uf->elem_cnt)
		return -1;
	elem = uf->elems;
	while(p != elem[p].id)
	{
		elem[p].id = elem[elem[p].id].id; /*Path Compression by Halving*/
		p = elem[p].id;
	}
	return p;	
}
/* Merge two components containing elements 'p' & 'q'    *
 * Weighted-Quick Union impl to avoid tall trees         *
 * Link root of the smaller tree to root of larger tree  */
static inline bool uf_union_impl(uf_t *uf, int p, int q)
{
	uf_elem_t *elem = uf->elems;
	int i = uf_find_impl(uf, p);
	int j = uf_find_impl(uf, q);
	/* Check if both elements are present first */
	if(i < 0 || j < 0) return false;
	/* Check if both elements are the same */
	if(i == j)         return false;
	/* If both elements are already in same component, return */
	if (i == j)        return true;
	/* Make root of smaller rank point to root of larger rank */
	if     (elem[i].rank < elem[j].rank) elem[i].id = j;
	else if(elem[i].rank > elem[j].rank) elem[j].id = i;
	else{
		elem[j].id = i;
		elem[i].rank++;
	}
	uf->cmp_cnt--;
	return true;
}
/* Two elements are connected i.e., in the same component *
 * if they have the same parent. Solved using find()      */
static inline bool uf_is_connected_impl(uf_t *uf, int p, int q)
{
	return (uf_find_impl(uf, p) == uf_find_impl(uf, q));
}
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
static inline uf_t *uf_create(int cnt)
{
	uf_t *uf = uf_create_impl(cnt);
	uf_init_impl(uf, cnt);
	return uf;
}
static inline void uf_destroy(uf_t *uf)
{return uf_destroy_impl(uf);}
/* Returns the number of Elements present   */
static inline unsigned int uf_elem_count(uf_t *uf)
{ return uf->elem_cnt; }
/* Returns the number of Components present   */
static inline unsigned int uf_cmp_count(uf_t *uf)
{ return uf->cmp_cnt; }
/* Locate root of component or return -1 if not present  */
static inline int uf_find(uf_t *uf, int p)
{ return uf_find_impl(uf, p); }
static inline bool uf_union(uf_t *uf, int p, int q)
{ return uf_union_impl(uf, p, q); }
static inline bool uf_is_connected(uf_t *uf, int p, int q)
{ return uf_is_connected_impl(uf, p, q); }
static inline void uf_iterator(uf_t *uf, void(*fn)(uf_elem_t *x))
{uf_iterator_impl(uf, fn);}
/*--------------------API Functions(End)--------------------*/
#endif //_UNION_FIND_DS_
