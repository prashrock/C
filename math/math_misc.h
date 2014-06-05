#ifndef _MATH_MISC_
#define _MATH_MISC_
#include <stdio.h>
#include <stdbool.h> /* bool, true, false */
#include <limits.h>  /* ULONG_MAX, CHAR_BIT - len of char in bits */
#define DY_STACK_OBJ_TYPE int
#define STACK_PRINT_TYPE(_data) printf("%1d", (_data));
#include "dy_stack.h" /* Dynamic Stack API */

/* Calculate absolute integer given + or -ve integer                 *
 * Calculate abs_x = !(x) + 1                                        *
 * Use the property that -1 >> n will still be -1                    *
 */
int abs(int n)
{
	int neg_val = n >> (sizeof(n) * CHAR_BIT - 1); /* Either 0 or 0xFFFFFFFF */
    /* For -ve numbers, XOR(0xFF..) handles NOT and - (-0xFFF) = +1   */
	return (n ^ neg_val) - neg_val;
}


/* Calc x^y or x^-y                                 *
 * Time Complexity = O(lg n)                        *
 * Space Complexity= O(1)                           *
 * Recursive Divide & Conquer approach              *
 * Since left/right half are doing the same work,   *
 * Recurse on only 1 side, saving both on number of *
 * recursive calls and multiply operations          */
double power(double x, int n) 
{
	double half;
	if (n == 0) 
		return 1;
  
	//Avoid duplicate multiplies
	half = power(x, n / 2);
  
	if (n % 2 == 0)
		return half * half;
	else if (n > 0) /* Conquer step +y*/
		return half * half * x;
	else            /* Conquer step -y*/
		return half * half / x;
}


#endif //_MATH_MISC_
