/* gcc -g -o math_test math_test.c  */
#include <stdio.h>
#include <stdbool.h>          /* bool, true, false */
#include <pthread.h>          /* Pthreads library */
#include <stdlib.h>           /* atoi  */
#include <string.h>           /* memset */
#include <limits.h>           /* ULONG_MAX */
#include "parser.h"           /* Parser API */
#include "multi_thread.h"     /* PThread helper  */

#define DQ_OBJ_TYPE char
#define QUEUE_PRINT_TYPE(_data) printf("%c", (_data));
#include "dy_queue.h"         /* Dynamic Queue API */
#include "print_utils.h"      /* print_int_matrix */
#include "math_fact.h"        /* Math Factorial   */
#include "math_prime.h"       /* Math Prime  */
#include "math_misc.h"        /* abs(), pow()  */
#include "math_matrix.h"      /* Matrix functions */
#include "math_permute.h"     /* Permutation functions */
#include "string_api.h"       /* String utilities */
#include "scan_utils.h"       /* input_integer */

#define MAX_PERMUTATION_DIGITS 10

/* Use a Dynamic Queue to handle input stream */
static inline long input_long()
{
	char c;
	long n;
	dq_t *q = dq_init();
	while((c = getchar()) != '\n') dq_enq(q, c);
	dq_enq(q, '\0');
	n = atol(q->elems);
	while(dq_deq(q, &c));
	dq_destroy(q);
	return n;
}

static void fact_test()
{
	int  n;
	unsigned long fact;
	printf("Enter an integer to calculate its factorial\n");
	input_integer(&n);
	/* Get to Testing factorial now */
	if(my_fact(n, &fact))
		printf("%d! = %lu\n", n, fact);
	else
		printf("Error: %d! Overflowed, Max(%dB)=%lu\n",
			   n, (int)sizeof(fact), ULONG_MAX);
	printf("%d! has %d trailing zeros\n", n,
		   count_trailing_zeros_in_factorial(n));
}

static void pow_test()
{
	double x;
	int  y;
	printf("Enter the base integer:\n");
	input_integer(&y);
	x = (double) y;
	printf("Enter the exponent integer:\n");
	input_integer(&y);
	/* Get to Testing power now */
	printf("%f^%d=%f\n", x, y, power(x, y));
}

static void prime_test()
{
	unsigned long n;
	printf("Enter the number to be tested for prime:\n");
	n = (unsigned long) input_long();
	/* Get to Testing Prime now */
	if(check_prime_brute(n))
		printf("%lu is prime\n", n);
	else
		printf("%lu is NOT prime\n", n);
}

static void prime_factors_test()
{
	unsigned long n;
	printf("Enter the number whose prime factors are needed:\n");
	n = (unsigned long) input_long();
	/* Get to Printing Prime now */
	print_prime_factors(n);
}


static void abs_test()
{
	int  x;
	printf("Enter the +/- Integer:\n");
	input_integer(&x);
	printf("Abs of %d = %d\n", x, abs(x));
}

static void gcd_lcm_test()
{
	int  x, y;
	printf("Enter the First Positive Integer:\n");
	input_integer(&x);
	printf("Enter the Second Positive Integer:\n");
	input_integer(&y);
	printf("GCD of '%d' and '%d' = %d\n", x, y, euclidean_gcd(x, y));
	printf("LCM of '%d' and '%d' = %d\n", x, y, lcm(x, y));
}

static void spiral_matrix_test(int n, int m)
{
	int mat[n][m];
	printf("Generating a spiral matrix(%d x %d):\n", n, m);
	create_spiral_matrix(n, m, mat);
	print_int_matrix(n, m, mat);
}

static void spiral_matrix_test_wrapper()
{
	int n, m;
	printf("Enter the Number of rows(n):\n");
	input_integer(&n);
	printf("Enter the Number of columns(m):\n");
	input_integer(&m);
	return spiral_matrix_test(n, m);
}

static void permutate_test()
{
	unsigned n;
	char str1[MAX_PERMUTATION_DIGITS + 1];
	char str2[MAX_PERMUTATION_DIGITS + 1];
	printf("Enter the number/string to permutate:\n");
	input_string(str1, sizeof(str1)-1);
	strncpy(str2, str1, sizeof(str2)-1);
	printf("1) Permutation without repetition:\n");
	lexical_permute_no_replacement(str1, strlen(str1), NULL, true);
	printf("\n2) Permutation with repetition:\n");
	n = heap_permute(str2, strlen(str2), NULL, true);
	printf("Total number of permutations = %d\n", n);
}

static void print_help_string()
{
	printf("\n\nPress Enter to exit or follow below commands\n");
	printf("\t          ?  - Print this text again\n");
	printf("\t        pow* - Calculate pow(x,y)\n");
	printf("\t       fact* - Calculate n! and print # trailing zeros\n");
	printf("\t        abs* - Print Absolute value of given +/- integer\n");
	printf("\t  testprime* - Test if inputted number is prime\n");
	printf("\t  primefact* - Print all prime factors of a number\n");
	printf("\t    gcd_lcm* - Calculate LCM and GCD of 2 numbers\n");
	printf("\t spiral_mat* - Generate a spiral matrix\n");
	printf("\t  permutate* - Generate all permutations of given num\n");
	printf("\t NOTE - * CMDS take optional loop_count, eg pow 2\n");
}

static void multi_char_CLI(const char *c)
{
	int lp = 1;
	const char *space;
	if((space = my_strstr_with_strlen(c, " ")))
	{
		lp = atoi(space + 1);
		if(lp == 0) lp = 1;
		printf("lp = %d\n", lp);
	}
	if(strncmp(c, "pow", strlen("pow")) == 0)
	{
		while(lp--) pow_test();
	}
	else if(strncmp(c, "fact", strlen("fact")) == 0)
	{
		while(lp--) fact_test();
	}
	else if(strncmp(c, "abs", strlen("abs")) == 0)
	{
		while(lp--) abs_test();
	}
	else if(strncmp(c, "testprime", strlen("testprime")) == 0)
	{
		while(lp--) prime_test();
	}
	else if(strncmp(c, "primefact", strlen("primefact")) == 0)
	{
		while(lp--) prime_factors_test();
	}
	else if(strncmp(c, "gcd_lcm", strlen("gcd_lcm")) == 0)
	{
		while(lp--) gcd_lcm_test();
	}
	else if(strncmp(c, "spiral_mat", strlen("spiral_mat")) == 0)
	{
		while(lp--) spiral_matrix_test_wrapper();
	}
	else if(strncmp(c, "permutate", strlen("permutate")) == 0)
	{
		while(lp--) permutate_test();
	}
}

static void single_char_CLI(const char *c)
{
	switch( *c )
	{
	case '?':
		print_help_string();
		break;
	default:
		break;
	}
}

static void single_char_CLI_with_num(const char *c)
{
	//int x = atoi(c+1);
	switch(*c)
	{
	case 't': case 'T':
		//handle_multi_threaded_test(x, queue_test_thread, false);
		printf("No Thread usage yet\n");
		break;
	default:
		break;
	}
}

int main()
{
	print_help_string();
	handle_CLI(single_char_CLI, single_char_CLI_with_num, multi_char_CLI);
	return 0;
}
