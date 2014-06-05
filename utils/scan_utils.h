#ifndef _SCAN_UTILS_API_
#define _SCAN_UTILS_API_
#include <stdio.h>
#include <stdbool.h>       /* bool, true, false */
#include <stdlib.h>        /* atoi  */
#include "compiler_api.h"  /* unlikely */

static inline bool input_integer(int *n)
{
	char c[50];
	int i = 0;
	while((c[i++] = getchar()) != '\n') if(i > 40) break;
	c[i] = '\0';
	*n =  atoi(c);
	if(*n == 0 && c[0] != '0') return false;
	else                       return true;
}

static inline bool input_unsigned_integer(unsigned int *n)
{
	char c[50];
	int i = 0;
	while((c[i++] = getchar()) != '\n') if(i > 40) break;
	c[i] = '\0';
	*n =  atoi(c);
	if(*n == 0 && c[0] != '0') return false;
	else                       return true;
}

#endif //_SCAN_UTILS_API_
