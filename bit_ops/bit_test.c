/* gcc -g -o bit_test bit_test.c  */
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
#include "bitmap.h"           /* Bitmap set/clear */
#include "bit_ops.h"          /* is_pow2, get_lsb */
#include "string_api.h"       /* String utilities */
#include "scan_utils.h"       /* input_integer */

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

static void lsb_test()
{
	unsigned int n;
	printf("Enter the number to get/clear LSB:\n");
	input_unsigned_integer(&n);
	print_integer_in_binary(n);
	printf("LSB position = %d\n", bit_get_lsb_pos(n)+1);
	printf("LSB val = %d, pos = %d\n",
		   bit_get_lsb_val(n), bit_get_lsb_pos(n)+1);
	n = bit_clear_lsb(n);
	print_integer_in_binary(n);
}

static void msb_test()
{
	unsigned int n;
	printf("Enter the number to get/clear MSB:\n");
	input_unsigned_integer(&n);
	print_integer_in_binary(n);
	printf("MSB val = %d, pos = %d\n",
		   bit_get_msb_val(n), bit_get_msb_pos(n)+1);
	n = bit_clear_msb(n);
	print_integer_in_binary(n);
}

static void bit_reverse_test()
{
	int n;
	printf("Enter the number to reverse:\n");
	input_integer(&n);
	print_integer_in_binary(n);
	printf("Reversing number:\n");
	print_integer_in_binary(bit_rev(n));
}

static void dec2bin_test()
{
	int  x;
	printf("Enter a Positive/Negative Integer:\n");
	input_integer(&x);
	print_integer_in_binary(x);
}

static void count_bits_set_test()
{
	int  x;
	printf("Enter an Integer:\n");
	input_integer(&x);
	print_integer_in_binary(x);
	printf("Population count i.e., # of 1's:\n");
 	printf("\tHamming Weight approach = %d\n", bit_pop(x));
 	printf("\tKernighan approach = %d\n", bit_population_count2(x));
	printf("Leading zero count:\n");
	printf("\tMethod1 count = %d\n", bit_nlz(x));
	printf("\tMethod2 count = %d\n", bit_num_leading_zeros2(x));
	printf("Trailing zero count:\n");
	printf("\tMethod1 count = %d\n", bit_ntz(x));
	printf("\tMethod2 count = %d\n", bit_num_trailing_zeros2(x));
	printf("Bit Parity = %u\n", bit_parity(x));
}

static void ispow2_test()
{
	int  x;
	printf("Enter an Integer:\n");
	input_integer(&x);
	if(bit_is_pow2(x))
		printf("%d is a power-of-two\n", x);
	else
		printf("%d is NOT a power-of-two\n", x);
	printf("Nearest pow2 for %d = %d\n", x,
		   bit_align_up_nearest_pow2(x));
}

static void log2_test()
{
	int  x;
	printf("Enter an Integer:\n");
	input_integer(&x);
	print_integer_in_binary(x);
	printf("Floor(log2(%d)) = %d\n", x, bit_floor_log2(x));
	printf(" Ceil(log2(%d)) = %d\n", x, bit_ceil_log2(x));
}

static void snoob_test()
{
	int  x;
	printf("Enter an Integer:\n");
	input_integer(&x);
	print_integer_in_binary(x);
	printf("Next big number with %d ones is:\n", bit_pop(x));
	print_integer_in_binary(bit_next_biggest_number_same_num_ones(x));
	printf("Next small number with %d ones is:\n",bit_pop(x));
	print_integer_in_binary(bit_next_smallest_number_same_num_ones(x));
}

static void subsets_test()
{
	int  x, cnt;
	printf("Enter an Integer:\n");
	input_integer(&x);
	print_integer_in_binary(x);
	printf("Generating all possible in-place subsets:\n");
	cnt = bit_inplace_permutate(x, NULL);
	printf("Total %d subsets found for %d\n", cnt, x);
}

static void arrangement_test()
{
	int  x, cnt;
	printf("Enter an Integer:\n");
	input_integer(&x);
	print_integer_in_binary(x);
	printf("Generating all possible arrangements of x:\n");
	cnt = bit_generate_arrangements(x, NULL);
	printf("Total %d arrangements found for %d\n", cnt, x);
}

static void print_help_string()
{
	printf("\n\nPress Enter to exit or follow below commands\n");
	printf("\t         ?  - Print this text again\n");
	printf("\t       lsb* - Get and Clear LSB given an integer\n");
	printf("\t       msb* - Get and Clear MSB given an integer\n");
	printf("\t   reverse* - Reverse bits of given integer\n");
	printf("\t   dec2bin* - Print Integer in binary form\n");
	printf("\t countbits* - Print # Set/Unset bits in given integer\n");
	printf("\t    ispow2* - Check if integer is a power-of-two\n");
	printf("\t      log2* - Calculate log2(given_integer)\n");
	printf("\t     snoob* - Print next big/small # with same # ones\n");
	printf("\t   subsets* - Print all subsets of given integer\n");
	printf("\t   arrange* - Print all arrangements of given integer\n");
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
	if(strncmp(c, "lsb", strlen("lsb")) == 0)
	{
		while(lp--) lsb_test();
	}
	else if(strncmp(c, "msb", strlen("lsb")) == 0)
	{
		while(lp--) msb_test();
	}
	else if(strncmp(c, "reverse", strlen("reverse")) == 0)
	{
		while(lp--) bit_reverse_test();
	}
	else if(strncmp(c, "dec2bin", strlen("dec2bin")) == 0)
	{
		while(lp--) dec2bin_test();
	}
	else if(strncmp(c, "countbits", strlen("countbits")) == 0)
	{
		while(lp--) count_bits_set_test();
	}
	else if(strncmp(c, "ispow2", strlen("ispow2")) == 0)
	{
		while(lp--) ispow2_test();
	}
	else if(strncmp(c, "log2", strlen("log2")) == 0)
	{
		while(lp--) log2_test();
	}
	else if(strncmp(c, "snoob", strlen("snoob")) == 0)
	{
		while(lp--) snoob_test();
	}
	else if(strncmp(c, "subsets", strlen("subsets")) == 0)
	{
		while(lp--) subsets_test();
	}
	else if(strncmp(c, "arrange", strlen("arrange")) == 0)
	{
		while(lp--) arrangement_test();
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
	bit_toggle(1, 0);
	is_bit_set(1, 0);
	bit_get_msb_val(1);
	bit_get_rightmost_zero_pos(1);
	return 0;
}


