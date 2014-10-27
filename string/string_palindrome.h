#ifndef _STR_PALINDROME_ALG__
#define _STR_PALINDROME_ALG__

#include <stdio.h>
#include <stdlib.h>  /* atoi  */
#include <stdbool.h> /* bool, true, false */
#include <ctype.h>   /* isspace */
#include <string.h>  /* strncasecmp - case-insensitive strcmp*/
#include <assert.h>  /* assert */
#include "compiler_api.h" /* SWAP, print_matrix macro */

/* Check if a given char array is a palindrome                   *
 * Time Complexity  = O(n/2)                                     *
 * Space Complexity = O(1)                                       */
static inline bool is_palindrome_brute(const char * const buf, int len)
{
	int i;                             /* Counters */
	if(buf == NULL || len == 0)        return false;
	else if(len == 1)          	   	   return true;
	for(i = 0; i < len / 2; i++)
		if(buf[i] != buf[len - 1 - i]) return false;
	return true;
}

/* Expand around center to check for palindrome                  *
 * Time Complexity  = O(n/2)                                     *
 * Space Complexity = O(1)                                       */
static inline int expand_around_center(const char * const buf,
										int len, int c1, int c2)
{
	int l = c1;
	int r = c2;
	for(; l >= 0 && r <= len - 1; l--, r++)
		if(buf[l] != buf[r]) break;
	l++;
	return (r-l);
}

/* Substring palindrome = Expand around center approach           *
 * Time Complexity = O(n^2)                                       *
 * Space Complexity = O(1)                                        */
static inline int longest_palindrome_substring(const char * const buf,
											   int len)
{
	int i, j, start = -1, end = -1, pal_len = -1;
	if(buf == NULL || len == 0)   return 0;
	for(i = 0; i < len; i++)
	{
		int new_pal_len;
		/* Palindromic substrings can be of even or odd len  *
		 * When expanding around center, this information is *
		 * not available... So at each position do 2 expands:*
		 * 1st expand will handle odd length sub-str palindr *
		 * 2nd expand will handle even length sub-str palindr*/
		for(j = 0; (j < 2) && (j + i < len); j++)
		{
			new_pal_len = expand_around_center(buf, len, i, i + j);
			if(new_pal_len > pal_len)
			{
				pal_len = new_pal_len;
				start = i + j - (pal_len / 2);
				end = i + j + (pal_len / 2) + (pal_len & 1);
			}
		}
	}
	if(start == -1) start = 0;
	printf("Substring '");
	for(i = start; i < end; i++) printf("%c", buf[i]);
	printf("' of length %d  at pos_idx %d ", pal_len, start);
	printf("is longest Palind Substr\n");
	return pal_len;
}
	
 
#endif //_STR_PALINDROME_ALG__
