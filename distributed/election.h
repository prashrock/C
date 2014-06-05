#ifndef _ELECTION_API_
#define _ELECTION_API_
#include <stdio.h>

#define INTEGER_DIV_CEIL(_divisor)  ((_divisor) - 1)

/* Boyer Moore Majority Algorithm to find Majority element in a *
 * given input array. The majority element occurs half or more  *
 * times in the array. If two elements qualify for majority,i.e *
 * only 2 elements occur n/2 times each, then 1st is picked     *
 * Time Complexity = O(n), Space Complexity = O(1)              *
 */
int boyer_moore_majority_vote(const int arr[], int n, int *result)
{
	int candidate, count = 0, i;
	/* First Pass */
	for(i = 0; i < n; i++)
	{
		if(count == 0)         	candidate = arr[i];
		if(candidate == arr[i]) count++;
		else                    count--;
	}
	/* Second Pass to verify if candidate is really majority or *
	 *  simply one with non-zero count (occuring towards end)   */
	count = 0;
	for(i = 0; i < n; i++)
		if(candidate == arr[i]) count++;
	/* For odd number of elements, integer division will cause  *
	 * error when determining n/2. For ex, 7/2 = 3,             *
	 * therefore do count < ceil(n/2)                           */
	if(count < ((n + INTEGER_DIV_CEIL(2)) / 2))
		return false;
	else
	{
		*result = candidate;
		return true;
	}
}

#endif //_ELECTION_API_
