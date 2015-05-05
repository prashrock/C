#ifndef _MATH_PERMUTE_
#define _MATH_PERMUTE_
#include <stdio.h>
#include <stdbool.h>       /* bool, true, false */
#include <stdlib.h>        /* atoi, qsort  */
#include <limits.h>        /* ULONG_MAX */
#include "compiler_api.h"  /* SWAP */

/* Qsort helper function */
static inline int permute_char_comp(const void *a, const void *b)
{ return ( *(char *)a - *(char *)b ); }

/* Algorithm from E. W. Dijkstra's 'A Discipline of Programming'(p:71)*
 * to compute the next permutation without replacement when starting  *
 * with a lexically ordered input sequence.                           *
 * Note: this logic can handle repetitions correctly                  */
static inline void get_next_permutation(char *value, int n)
{
	int i = n - 1; /* i tracks the smaller value                      */
	int j = n;     /* j tracks the larger value                       */
	while(value[i-1] >= value[i])   i--;
	while(value[j-1] <= value[i-1]) j--;
	SWAP(value[i-1], value[j-1]);
	i++;
	j = n;
	while(i < j) {
		SWAP(value[i-1], value[j-1]);
		i++;
		j--;
	}
}
/* For lexically permutation sequence, check if we are done           */
static inline bool has_next_permutation(char *value, int n)
{
	int i;
	for(i = 0; i < n; i++) {
		if(value[i] < value[i + 1])
			return true;
	}
	return false;
}
/* Use Dijkstra's lexical permutation algorithm shown above           *
 * Use quicksort to create the initial lexical sort order             *
 * For a distinct n input string, #permutations = nPn = n!            *
 * For an input string with repetitions, this is an arrangement       *
 * problem, where #permutations = n! / (n1! n2! n3!) where n1, n2,..  *
 * are the number of occurences of each digit/letter                  */
static inline void lexical_permute_no_replacement(char *value, int n,
					void (*fn)(char *value), bool print)
{
	int tot = 0;
	if(print)
		printf("Below are the permutations for %s:\n", value);
	/* Sort the input elements first */
	qsort(value, n, sizeof(char), permute_char_comp);
	/* First permutation is the output of sort */
	tot++;
	if(print) printf("\t%s\n", value);
	if(fn)    fn(value);
	while(has_next_permutation(value, n)) {
		tot++;
		get_next_permutation(value, n);
		if(print) printf("\t%s\n", value);
		if(fn)    fn(value);	
	}
	printf("Total number of permutations = %d\n", tot);
}

/* Heap permute: Permute input array with repetitions.                *
  from: A.Levitin, Introduction to Design&Analysis of Algorithms(179) */
static inline unsigned heap_permute(char *value, int n,
									void (*fn)(char *value), bool print)
{
	int i;
	unsigned tot = 1;       /* Keeps track of total #permutations     */
	if (n == 1) {
		if(print) printf("\t%s\n", value);
		if(fn)    fn(value);
	}
	else
	{
		tot = 0;
		for (i = 0; i < n; i++) {
			tot += heap_permute(value, n - 1, fn, print);
			if (n % 2 == 1) SWAP(value[0], value[n-1]); /* Odd case   */
			else            SWAP(value[i], value[n-1]); /* Even case  */				
		}
	}
	return tot;
}
#endif //_MATH_PERMUTE_
