#ifndef _PRINT_UTILS_API_
#define _PRINT_UTILS_API_
#include <stdlib.h>        /* atoi  */
#include "compiler_api.h"  /* unlikely */

static inline void print_int_array(const int *arr, int n)
{
	int i;
	if(unlikely(arr == NULL)) return;
	else
		for(i = 0; i < n; i++)
			printf("%d, ", arr[i]);
	printf("\n");
}

#endif //_PRINT_UTILS_API_
