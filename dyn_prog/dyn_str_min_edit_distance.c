/* gcc -g -o dyn_str_min_edit_objs dyn_str_min_edit_objs.c  */
#include <stdio.h>
#include <stdbool.h>       /* bool, true, false */
#include <pthread.h>       /* Pthreads library */
#include <stdlib.h>        /* atoi  */
#include <string.h>        /* memset */
#include <limits.h>        /* ULONG_MAX */
#include "parser.h"        /* Parser API */
#include "multi_thread.h"  /* PThread helper  */
#include "scan_utils.h"    /* input_string */
#include "print_utils.h"   /* print_int_matrix */

#define MAX_STR_LEN (40)

/* Given two strings, determine minimum number of operations    *
 * (insert, del, substitute) to change 1 string to other        *
 * Use Levenshtein distance, ins,del cost = 1, sub cost = 2     */

/* Quadratic time and space complexity O(n*m)                   *
 * n = len(str1), m = len(str2)                                 *
 * Use Memoization + Tabulation to arrive at final #differences *
 * Use below recurrence:                                        *
 * M[i][j] =    (M[i-1][j] + 1  --Add/Delete                    *
 *           min(M[i][j-1] + 1  --Add/Delete                    *
 *              (M[i-1][j-1] + 1 (if str[i] != str[j]) -- Subst *
 *              (            + 0 (if str[i] == str[j]) -- Subst */
unsigned int min_edit_distance(char str1[], int n, char str2[], int m)
{
	int i, str_i;
	int j, str_j;
	/* str1 in row and str2 in column */
	int matrix[n+1][m+1];
	memset(matrix, 0, sizeof(matrix));
	/* First row, pre-initialize with add/del costs of str1 alone  */
	for(i = 0; i <= n; i++)
		matrix[i][0] = i;
	/* First column, pre-initialize with add/del costs of str2 alone */
	for(j = 0; j <= m; j++)
		matrix[0][j] = j;
	/* Calculate all matrix indexes with Dynamic Programming */
	for(i = 1; i <= n; i++)
	{
		for(j = 1; j <= m; j++)
		{
			int min;
			str_i = i-1; str_j = j-1;
			/* Last case of substitute - no cost penality */
			if(str1[str_i] == str2[str_j])
				matrix[i][j] = matrix[i-1][j-1];
			else
			{
				/* Minimum of first 2 cases in add/delete  */
				min = MIN((matrix[i-1][j] + 1), (matrix[i][j-1] + 1));
				/* Minimum of first 2 cases of addd/del and 1st case of subst */
				/* To increase cost of substitute, change below */
				min = MIN(min, (matrix[i-1][j-1] + 1));
				matrix[i][j] = min;
			}
		}
	}
	/* Print the Matrix */
	print_int_matrix(n+1, m+1, matrix);
	return matrix[n][m];
}

void min_edit_distance_input_strings()
{
	int n, m, med;
	char str1[MAX_STR_LEN], str2[MAX_STR_LEN];
	
	printf("Please Enter First String:\n");
	if((n = input_string(str1, MAX_STR_LEN)) == -1){
		printf("Error: First string input failed\n");
		return;
	}

	printf("Please Enter Second String:\n");
	if((m = input_string(str2, MAX_STR_LEN)) == -1){
		printf("Error: Second string input failed\n");
		return;
	}

	printf("Comparing below 2 strings:\n");
	printf("'%s' = %d\n", str1, n);
	printf("'%s' = %d\n", str2, m);
	med = min_edit_distance(str1, n, str2, m);
	printf("Levenshtein min-edit-distance of below 2 strings = %d\n", med);
}

int main()
{
	min_edit_distance_input_strings();
	return 0;
}
