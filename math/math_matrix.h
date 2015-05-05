#ifndef _MATH_MATRIX_
#define _MATH_MATRIX_
#include <stdio.h>
#include <stdbool.h> /* bool, true, false */
#include <stdlib.h>  /* atoi  */
#include <limits.h>  /* ULONG_MAX */


/* Create a nrow x ncol Spiral Matrix. Eg. a 4x4 spiral     *
 * matrix is shown below:                                   *
 * 1    2    3    4                                         *
 * 12  13   14    5                                         *
 * 11  16   15    6                                         *
 * 10   9    8    7                                         *
 * Time Complexity = O(n * m). Space Complexity = O(1)      */
static inline void create_spiral_matrix(int nrow, int ncol,
										int mat[nrow][ncol])
{

	int left = 0, right = ncol-1, top = 0, bottom = nrow-1;
	int val = 1;
	memset(mat, 0, sizeof(mat)); /* Initialize matrix first */
	while(left <= right && top <= bottom)
	{
		int i;
		/* 1) Go from left to right */
		for(i = left; i <= right; i++)   mat[top][i] = val++;
		/* 2) Go from top to bottom */
		for(i = top+1; i <= bottom; i++) mat[i][right] = val++;
		/* 3) Go from right to left */
		for(i = right-1; i >= left; i--) mat[bottom][i] = val++;
		/* 4) Go from bottom to top */
		for(i = bottom-1; i > top; i--)  mat[i][left] = val++;
		left++; right--;
		top++; bottom--;
	}
}

#endif //_MATH_MATRIX_
