//gcc -Wall -g -lpthread -o str_test str_test.c
#include <stdio.h>         /* printf */
#include <pthread.h>       /* Pthreads library */
#include <string.h>        /* strerror, stricmp */
#include "parser.h"        /* Parser API */
#include "multi_thread.h"  /* PThread helper  */

#define DQ_OBJ_TYPE char
#define QUEUE_PRINT_TYPE(_data) printf("%c", (_data));
#include "dy_queue.h"      /* Dynamic Queue API */
#include "string_api.h"    /* String utilities */
#include "string_palindrome.h"/* Brute, DY, Manacher's Algo */

static void streven_test()
{
	dq_t *dq_str = dq_init();
	char c;
	printf("Enter the string to test(Press enter when done):\n");
	while((c = getchar()) != '\n') dq_enq(dq_str, c);
	dq_enq(dq_str, '\0');
	printf("Input '%s' has length = %d\n",
		   dq_str->elems, dq_count(dq_str)-1);
	//dq_print_stats(&dq_stats);

	if(my_is_strlen_even(dq_str->elems) == true)
		printf("Verdict = Even Steven\n");
	else
		printf("Verdict = Uneven\n");
	while(dq_deq(dq_str, &c));
	dq_destroy(dq_str);
}

static void strrev_test()
{
	dq_t *dq_str = dq_init();
	int len;
	char c;
	printf("Enter the string to reverse(Press enter when done):\n");
	while((c = getchar()) != '\n') dq_enq(dq_str, c);
	dq_enq(dq_str, '\0');
	len = dq_count(dq_str)-1;
	printf("Input '%s' has length = %d\n", dq_str->elems, len);

	my_strrev(dq_str->elems, len);
	printf("Reverse String = '%s'\n", dq_str->elems);
	dq_destroy(dq_str);
}

static void str_words_rev_test()
{
	dq_t *dq_str = dq_init();
	int len;
	char c;
	printf("Enter the string to reverse(Press enter when done):\n");
	while((c = getchar()) != '\n') dq_enq(dq_str, c);
	dq_enq(dq_str, '\0');
	len = dq_count(dq_str)-1;
	printf("Input '%s' has length = %d\n", dq_str->elems, len);

	str_word_rev(dq_str->elems, len);
	printf("Word Reverse String = '%s'\n", dq_str->elems);
	dq_destroy(dq_str);
}

static void str_is_palindrome_test()
{
	dq_t *dq_str = dq_init();
	int len;
	char c;
	printf("Enter the string to test(Press enter when done):\n");
	while((c = getchar()) != '\n') dq_enq(dq_str, c);
	dq_enq(dq_str, '\0');
	len = dq_count(dq_str)-1;
	printf("Input '%s' has length = %d\n", dq_str->elems, len);

	if(is_palindrome_brute(dq_str->elems, len))
		printf("Whole String '%s' is Palindrome\n", dq_str->elems);
	else
		printf("Whole String '%s' is NOT Palindrome\n", dq_str->elems);

	longest_palindrome_substring(dq_str->elems, len);
	dq_destroy(dq_str);
}

static void strfind_test()
{
	char c;
	dq_t *big = dq_init();
	dq_t *small = dq_init();
	const char *match_pos;
	/* Use getchar() for Big String as we can handle any size buffer with DY queue */
	printf("Enter Searchable string (big) (Press enter when done):\n");
	while((c = getchar()) != '\n') dq_enq(big, c);
	printf("Enter Search string (small) (Press enter when done):\n");
	while((c = getchar()) != '\n') dq_enq(small, c);
	/* Pad Big and Small strings */
	dq_enq(big, '\0'); dq_enq(small, '\0');
	printf("Big '%s' length = %d\n", big->elems, dq_count(big)-1);
	printf("Small '%s' length = %d\n", small->elems, dq_count(small)-1);
	//dq_print_stats(&dq_stats);

	if((match_pos = my_strstr_without_strlen(big->elems, small->elems)) != NULL)
		printf("Verdict = Found first match of '%s' at position %ld\n",
			   small->elems, match_pos-big->elems+1);
	else
		printf("Verdict = '%s' not found\n", small->elems);
	
	while(dq_deq(big, &c));  	/* Clear Big Q */
	while(dq_deq(small, &c));  	/* Clear Small Q */
	dq_destroy(big); dq_destroy(small);
}

static void strtrim_test()
{
	dq_t *dq_str = dq_init();
	int len;
	char c;
	printf("Enter the string to trim(Press enter when done):\n");
	while((c = getchar()) != '\n') dq_enq(dq_str, c);
	dq_enq(dq_str, '\0');
	len = dq_count(dq_str)-1;
	printf("Input '%s' has length = %d\n", dq_str->elems, len);

	len = str_trim(dq_str->elems, len);
	printf("Trimmed '%s' has length = %d\n", dq_str->elems, len);
	dq_destroy(dq_str);
}

static void str_lcs_test()
{
	char c;
	dq_t *s1 = dq_init();
	dq_t *s2 = dq_init();

	/* Use getchar() for any size buffer with DY queue */
	printf("Enter First String (Press enter when done):\n");
	while((c = getchar()) != '\n') dq_enq(s1, c);
	printf("Enter Second String (Press enter when done):\n");
	while((c = getchar()) != '\n') dq_enq(s2, c);

	/* Pad both strings */
	dq_enq(s1, '\0'); dq_enq(s2, '\0');
	printf("S1 '%s' length = %d\n", s1->elems, dq_count(s1)-1);
	printf("S2 '%s' length = %d\n", s2->elems, dq_count(s2)-1);

	str_longest_common_substring(s1->elems, dq_count(s1)-1,
							 s2->elems, dq_count(s2)-1) ;
	
	while(dq_deq(s1, &c));  	
	while(dq_deq(s2, &c));  	
	dq_destroy(s1); dq_destroy(s2);
}

static void print_help_string()
{
	printf("\n\nPress Enter to exit or follow below commands\n");
	printf("\t        ? - Print this text again\n");
	printf("\t  strlen* - Calculate String Length\n");
	printf("\t streven* - Check if Input String Length is Even/Odd\n");
	printf("\t strtrim* - Trim lead/trail & inbetween extra spaces\n");
	printf("\t strfind* - Find substring in bigger string\n");
	printf("\t  strrev* - Reverse given String\n");
	printf("\t wordrev* - Reverse words of given String\n");
	printf("\t is_pali* - Check if a String is Palindrome\n");
	printf("\t     lcs* - Longest Common Substring given 2 strings\n");
	printf("\t NOTE - * CMDS take optional loop_count, eg strlen 2\n");
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
	if(strncmp(c, "strlen", strlen("strlen")) == 0)
	{
		printf("strlen called\n");
	}
	else if(strncmp(c, "streven", strlen("streven")) == 0)
	{
		while(lp--) streven_test();
	}
	else if(strncmp(c, "strtrim", strlen("strtrim")) == 0)
	{
		while(lp--) strtrim_test();
	}
	else if(strncmp(c, "strfind", strlen("strfind")) == 0)
	{
		while(lp--) strfind_test();
	}
	else if(strncmp(c, "strrev", strlen("strrev")) == 0)
	{
		while(lp--) strrev_test();
	}
	else if(strncmp(c, "wordrev", strlen("wordrev")) == 0)
	{
		while(lp--) str_words_rev_test();
	}
	else if(strncmp(c, "is_pali", strlen("is_pali")) == 0)
	{
		while(lp--) str_is_palindrome_test();
	}
	else if(strncmp(c, "lcs", strlen("lcs")) == 0)
	{
		while(lp--) str_lcs_test();
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
