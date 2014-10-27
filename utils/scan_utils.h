#ifndef _SCAN_UTILS_API_
#define _SCAN_UTILS_API_
#include <stdio.h>
#include <stdbool.h>        /* bool, true, false */
#include <stdlib.h>         /* atoi, EXIT_FAILURE */
#include <string.h>         /* strlen */
#include <limits.h>         /* INT_MAX */
#include "compiler_api.h"   /* unlikely */
#include "terminal_utils.h" /* termios_init()  */

#define MAX_INT_DIGITS (10) /* 4 Billion = 10 Digits */

/* Points to remember                                                  *
 * = fgets() vs gets()                                                 *
 *   = gets() does not allow you to specify length of buffer.          *
 *   = gets() will devour newline at end of line.                      *
 *   = fgets() allows specifying Stream + length of buffer (safer)     *
 *   = fgets() will store \n at end of string, should trim manually    *
 * = strtok is not multi-thread(MT) safe or re-entrant safe            *
     = Not all Library calls are thread safe + re-entrancy safe.       *
	 = strtok_r() is re-entrant (safe for concurrent access) + MT safe *
	 = Re-entrant does not imply thread-safe (notes - CS_C_Semantics)  * 
 */


/* Disabled buffered IO - i.e., dont wait for '\n' to    *
 * read a char. After input, enable buffered IO back     */
static inline char getch()
{
	char c = 0;
	int char_int;
	termios_init(true);
	char_int = fgetc(stdin);
	termios_reset();
	if(char_int == EOF)
		printf("Info: getch() got EOF\n");
	else
		c = (char)char_int;
	return c;
}

/* Trims '\n' at end of buffer. Can use with fgets()      */
static inline bool input_trim_newline(char *s)
{
	int len;
	if(s == NULL)  return false;
    len = strlen(s);

    /* First check if there is a newline at end of buffer */
	if (len > 0 && s[len-1] == '\n') {
		s[len-1] = '\0';
		return true;
	}
	else
		return false;
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

/* The popen() call opens a process by creating a pipe,  *
 * forking, and invoking the shell(bourne shell on Linux)*
 * The advantage to using popen() is that it will allow  *
 * one to interrogate the results of the command issued. */
/* Note:                                                 *
 * = popen() is multi-thread safe as per documentation   *
 * = system() is not multi-thread safe                   *
 * = with multiple fork+pclose, wait() might get confused*/
static inline bool exec_cmd_print_output(const char *cmd)
{
	FILE *fpipe;
	char line[256];
	if(!(fpipe = (FILE *) popen(cmd, "r")))
	{
		perror("Error - popen failed::");
		return false;
	}
	printf("Info - Executing '%s' in CWD\n", cmd);
	while(fgets(line, sizeof(line), fpipe))
	{
		printf("%s", line);
	}
	pclose(fpipe);
	return true;
}

#endif //_SCAN_UTILS_API_
