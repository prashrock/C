#ifndef _PRINT_UTILS_API_
#define _PRINT_UTILS_API_
#include <stdlib.h>        /* atoi  */
#include <unistd.h>        /* popen() */
#include <stdbool.h>       /* bool, true, false */
#include "compiler_api.h"  /* unlikely */

static inline bool open_tee_stdout_to_file(const char *fname, FILE **fptr)
{
	char tee_str[100];
	if(!fname || !fptr) return false;
	snprintf(tee_str, sizeof(tee_str)-1, "tee %s", fname);
	*fptr = popen(tee_str, "w");
	if(dup2(fileno(*fptr), STDOUT_FILENO) < 0) {
		fprintf(stderr, "couldn't redirect output to %s\n", fname);
		return false;
	}
	return true;
}

/* The problem with this approach is popen(tee) does not return */
static inline void close_tee_stdout_to_file(FILE *fptr)
{
	fflush(fptr);
	pclose(fptr);
}

/* Ensure all bytes written to stream are flushed to kernel buffers and *
 * subsequently written to disk before program execution continues      */
static inline void flush_stream(FILE *fptr)
{
	fflush(fptr);
	fsync(fileno(fptr));
}

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

static inline void print_matrix(const int n, const int m, int x[][m])
{
	int i, j;
	printf("\nR/C | ");
	for(i = 0; i < m; i++)
		printf("%4d ", i);
	printf("\n---------------------------------------------------\n");
	for (i = 0; i < n; i++)
	{
		printf("%4d | ", i);
		for (j = 0; j < m; j++)
			printf ("%4d ", x[i][j]);
		printf("\n");
	}
	printf("\n");
}

#endif //_PRINT_UTILS_API_
