#ifndef _MATH_PRIME_
#define _MATH_PRIME_
#include <stdio.h>
#include <stdbool.h> /* bool, true, false */
#include <limits.h>  /* ULONG_MAX */

/* Brute force mechanism to check if a given number is Prime *
 * Time Complexity = O(rt n)                                 */
bool check_prime_brute(unsigned long n)
{
	unsigned long div;
	/* Special case, less than 2 */
	if(n < 2)                     return false;
	else if(n != 2 && n % 2 == 0) return false; 
	/* Now, div=3, check for each odd number till sqrt(n)*/
	for(div = 3; div * div <= n; div+=2)
		if(n % div == 0)
			return false;
	return true;
}

/* Print all the prime factors of a number 'n' including     *
 * repetition.                                               *
 * Time Complexity = O (m) where n is the number of factors  */
void print_prime_factors(unsigned long n)
{
	unsigned long div;
	/* Single check for Even numbers and numbers less than 2 */
	if(n == 0) return;
	printf(" 1");
	for(div = 2; n > 1; div+=2)
	{
		if(check_prime_brute(div) == false) continue;
		while(n % div == 0)
		{
			printf(", %lu", div);
			n /= div;
		}
		/* Check to avoid iterating through even numbers */
		if(div == 2) div = 1;
	}
	printf("\n");
}

/* Calc GCD of 2 + numbers with Euclidean Algorithm *
 * Polynomial time algorithm that computes:         *
 * gcd(a, b) = gcd(b, a mod b)                      */
unsigned int euclidean_gcd(unsigned int a, unsigned int b)
{
	int c;
	if(a == 0)       return b;
	else if(b == 0)  return a;
	if (a < b){ /* We want a > b, so exchange a and b */
		c = a;
		a = b;
		b = c;
	}
	while(b != 0){
		c = a % b;
		a = b;
		b = c;
	}
	return a;
}

/* Calc LCM of 2 + numbers                          *
 * Polynomial time algorithm that computes:         *
 * lcm(a, b) = (a * b) / GCD(a, b)                  */
unsigned int lcm(unsigned int a, unsigned int b)
{
	return (unsigned int)
		(((unsigned long) (a * b)) / euclidean_gcd(a, b));
}
#endif //_MATH_PRIME_
