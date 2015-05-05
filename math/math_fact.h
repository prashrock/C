#ifndef _MATH_FACT_
#define _MATH_FACT_
#include <stdio.h>
#include <stdbool.h> /* bool, true, false */
#include <stdlib.h>  /* atoi  */
#include <limits.h>  /* ULONG_MAX */

/* Calc n! without recursion                       *
 * Time Complexity = O(n)                          *
 * Space Complexity = O(1)                         *
 * Take care of overflow during the process        */
static inline bool my_fact(int n, unsigned long *fact)
{
	*fact = 1;
	if (n == 0) return true;
	while(n) {
		unsigned long old_fact = *fact;
		(*fact) = (*fact) * n--;
		if((*fact) < old_fact) return false;
	}
	return true;
}

/* Calc trailing zeros in a factorial               *
 * Every trailing zero comes from a multiple of 10  *
 * Every 10 has 2 factors = 2 and 5. In a factorial *
 * Every other number is a multiple of 2, so number *
 * of 10's is determined by                         *
 * = #5's + #25's + #125's + etc                    */
static inline int count_trailing_zeros_in_factorial(int n)
{
	int cnt = 0, fpow;
	if(n == 5) return 1;
	for(fpow = 5; (n/fpow >= 1); fpow *= 5) {
		cnt += n/fpow;
	}
	return cnt;
}

#endif //_MATH_FACT_
