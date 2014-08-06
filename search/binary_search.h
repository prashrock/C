#ifndef _BINARY_SEARCH_H_
#define _BINARY_SEARCH_H_
#include <stdio.h>
#include <stdlib.h>         /* atoi  */
#include <stdbool.h>        /* bool, true, false */
#include <ctype.h>          /* isspace */
#include <string.h>         /* memset */
#include <assert.h>         /* assert */
#include <limits.h>         /* INT_MAX */
#include "compiler_api.h"   /* MIN */

/* Find the minimum element in a Sorted Array which was    *
 * rotated around unknown # times. Handle duplicates.      *
 * Impl: Modified Binary Search                            *
 * Time complexity:                                        *
 *      Without duplicates worst-case = O(lg n)            *
 *      With duplicates worst-case    = O(n)               */
static int binary_search_rotated_sorted_array(int arr[], int low, int high)
{
	/* Handle case without rotation and without duplicates */
	if(arr[low] < arr[high])       return arr[low];
	/* Handle bound conditions + Divide-Conquer base case  */
	if(low > high || low == high)  return arr[low];
	/* Find middle element                                 */
	int mid = low + (high - low)/2;
	/* Watch out for duplicate elements -- causes O(n)     */
	if(arr[low] == arr[mid] && arr[high] == arr[mid])
		return MIN(binary_search_rotated_sorted_array(arr, low, mid-1),
				   binary_search_rotated_sorted_array(arr, mid+1, high));
	/* Check if mid is the minimum element */
	if(mid < high && arr[mid] < arr[mid-1])
		return arr[mid];
	/* Decide recursion on left-half or right-half         *
	 * Always chose the side which looks "unsorted"        *
	 * This is the main difference wrt Binary Search       */
	/* Consider - 7 1 2 |3| 4 5 6                          */
	if(arr[mid] < arr[high])
		return binary_search_rotated_sorted_array(arr, low, mid-1);
	/* Consider - 3 4 5 |6| 7 1 2                          */
	return binary_search_rotated_sorted_array(arr, mid+1, high);
}


#endif //_BINARY_SEARCH_H_
