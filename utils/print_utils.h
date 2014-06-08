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

static inline void print_int_matrix(int n, int m, int mat[n][m])
{
	int i, j;
	/* Print the Matrix */
	printf("\n");
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
			printf ("%3d ", mat[i][j]);
		printf("\n");
	}
	printf("\n");
}

#endif //_PRINT_UTILS_API_
