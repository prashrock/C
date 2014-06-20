#ifndef _SCAN_UTILS_API_
#define _SCAN_UTILS_API_
#include <stdio.h>
#include <stdbool.h>        /* bool, true, false */
#include <stdlib.h>         /* atoi  */
#include <limits.h>         /* INT_MAX */
#include "compiler_api.h"   /* unlikely */
#include "terminal_utils.h" /* termios_init()  */

#define MAX_INT_DIGITS (10) /* 4 Billion = 10 Digits */

/* Disabled buffered IO - i.e., dont wait for '\n' to    *
 * read a char. After input, enable buffered IO back     */
static inline char getch()
{
	char ch;
	termios_init(true);
	ch = fgetc(stdin);
	termios_reset();
	return ch;
}
	
/* Expects a dynamic container if max_len == -1           *
 * If single char is requested, automatically use TERMIO  *
 * settings to not wait for \n                            *
 * Otherwise inputs max_len-1 characters and appends (\0) *
 * Returns -1 upon failure and strlen upon success        */
static inline int input_generic_api(char c[], int max_len)
{
	int i = 0;
	if(max_len == -1) max_len = INT_MAX;
	else if(max_len == 1){
		c[0] = getchar();
		return max_len;
	}
	while((c[i++] = getchar()) != '\n') if(i > max_len-1) break;
	c[i] = '\0';
	if(c[0] == 0 || c[0] == '\n') return -1;
	else                          return i;
}

static inline bool input_integer(int *n)
{
	char c[MAX_INT_DIGITS+1];
	int i = input_generic_api(c, MAX_INT_DIGITS+1);
	if(i > 0) {
		*n =  atoi(c);
 		return true;
	}
	else return false;
}

/* Use generic input API and trim '\n' in the end       *
 * This implementation is preferable when upper bound   *
 * of input size is known in advance.                   *
 * If input size upper bound is unknown, Dynamic Queue  *
 * API is a better API (ex: "str_lcs_test")             */
static inline int input_string(char c[], int max_len)
{
	int len = input_generic_api(c, max_len);
	if(len > 0 && c[len-1] == '\n'){
		c[len-1] = c[len];
		len--;
	}
	return len;
}
static inline bool input_unsigned_integer(unsigned int *n)
{
	return input_integer((int *) n);
}

#endif //_SCAN_UTILS_API_
