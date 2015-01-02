#ifndef _STR_HELPER_ALG__
#define _STR_HELPER_ALG__

#include <stdio.h>
#include <stdlib.h>  /* atoi  */
#include <stdbool.h> /* bool, true, false */
#include <ctype.h>   /* isspace */
#include <string.h>  /* strncasecmp - case-insensitive strcmp*/
#include <assert.h>  /* assert */
#include "compiler_api.h" /* SWAP, print_matrix macro */

/* fflush() cannot be used for STDIN. Implementing a custom way  *
 * to flush STDIN when Application *KNOWS* that STDIN is not     *
 * empty. If STDIN is empty, this call will block                */
static inline void my_flush_stdin()
{
	char buf[BUFSIZ]; //8K buf to clear input stream
	/* Example code to input 'i' characters & set 'i' correctly  */
	//while(((inp[i++] = getchar()) != '\n') && i < sizeof(inp));
	//sl--;
	fgets(buf, sizeof(buf), stdin);
}

/* State Machine based approach to detect even/odd string length *
 * Input = str ('0' terminated string)                           *
 * Output= true if even (0 characters is considered even)        *
 * Time Complexity  = O(n)                                       *
 * Space Complexity = O(1)                                       */
static inline bool my_is_strlen_even(const char *str)
{
even:
	if (*str++ == '\0') return true;
	else goto odd;

odd:
	if (*str++ == '\0') return false;
	else goto even;
}

/* If small occurs in big, return its first occurence, else NULL *
 * This approach calculates both strlen in advance (O(m) + O(n)) *
 * and is more optimal when small is also a big string because   *
 * search is complete when less than small elements left in big  *
 * In such a case, time complexity ~ O(m)                        *
 * Time Complexity  = O(mn)                                      *
 * Space Complexity = O(1)                                       */
static inline const char *my_strstr_with_strlen(const char * const big,
										 const char * const small)
{
	int i, j;                               /* Counters */
	int bl, sl;                             /* String lengths */
    /* Corner case, NULL pointer or empty strings */
	if(!big || !*big || !small || !*small)
		return NULL;
	bl = strlen(big);
	sl = strlen(small);
	if(bl < sl)
		return NULL;
	for(i = 0; i < bl; i++)
	{
		/* Optimization = When less than sl elements left in big, quit */
		if(bl - i < sl) break; 
		for(j = 0; j < sl; j++)
			if(big[i + j] != small[j]) break;
		if(j == sl) return &big[i];
	}
	return NULL;
}

/* If small occurs in big, return its first occurence, else NULL *
 * When strlen(small) is insignificant, this approach is better  *
 * as we do not iterate through big twice = strlen + search      *
 * Time Complexity  = O(mn)                                      *
 * Space Complexity = O(1)                                       */
const char *my_strstr_without_strlen(const char *big, const char *small)
{
	int i, j;                               /* Counters */
    /* Corner case, NULL pointer or empty strings */
	if(!big || !*big || !small || !*small)
		return NULL;
		
	for(i = 0; big[i] != '\0'; i++)
	{
		for(j = 0; small[j] != '\0'; j++)
			if(big[i + j] != small[j]) break;
		if(small[j] == '\0') return &big[i];
	}
	return NULL;
}

/* Reverse a given char array (logic applies to other datatype)  *
 * Time Complexity  = O(n/2)                                     *
 * Space Complexity = O(1)                                       */
static inline void my_strrev(char * const buf, int len)
{
	int i;                         /* Counters */
	if(buf == NULL || len <= 1) return;
	for(i = 0; i < len/2; i++)
		SWAP(buf[i], buf[len - 1 - i]);
}

/* Reverse a given char array with specific left/right bounds    *
 * Note: Same logic can be extended to other datatypes           *
 * Note: This is a wrapper around my_strrev()                    */
static inline void
my_strrev_with_bounds(char * const buf, int left, int right)
{
	if(right > left)
		my_strrev(&buf[left], (right - left + 1));
}

/* Trim trailing spaces                                          *
 * Time Complexity  = O(n)                                       *
 * Space Complexity = O(1)                                       */
static inline int str_rtrim(char * const buf, int len)
{
	int i, last_char_pos = len - 1;
	if(len == 0) return len; 
	for(i = last_char_pos; buf[i] == ' ';) i--;
	if(i != last_char_pos)
	{
		i = i + 1;
		buf[i] = buf[len];
		len = i;
	}
	return len;
}

/* Trim leading spaces - copy all non-space characters back      *
 * Time Complexity  = O(n)                                       *
 * Space Complexity = O(1)                                       */
static inline int str_ltrim(char * const buf, int len)
{
	int i, j, ret_len = len;
	if(len == 0) return len;
	for(i = 0; buf[i] == ' ';) i++;
	if(i)
	{
		ret_len = len - i;
		for(j = 0; i <= len; i++, j++)
			buf[j] = buf[i];
	}
	return ret_len;
}

/* Trim leading/trailing spaces and inter-word extra spaces      *
 * Time Complexity  = O(2n)                                      *
 * Space Complexity = O(1)                                       */
static inline int str_trim(char * const buf, int len)
{
	int i, j;

	len = str_ltrim(buf, len);    /* 1) Trim Leading Spaces */
	len = str_rtrim(buf, len);    /* 2) Trim Trailing Spaces */
	if(len == 0) return len;
    /* 3) Trim inter-word extra Spaces */
	for(i = 0, j = 0; i < len; i++)
	{
		if(buf[i] == ' ' && buf[i+1] == ' ')
			continue;
		else
			buf[j++] = buf[i];
	}
	if(i != j)
	{
		buf[j] = buf[i];    /* Copy null terminating char */
		len = j;            /* Update str length */
	}
	return len;
}

/* Rotate given array to the right by 'k' positions by removing  *
 * right-most 'k' elements and adding them back to the left.     *
 * To achieve rotation without using extra space, use the below  *
 * approach from Programming Pearls:                             *
 * (http://www.cs.bell-labs.com/cm/cs/pearls/)                   *
 * a) Reverse the entire string                                  *
 * b) Reverse from the beginning upto 'k-1' position             *
 * c) Reverse from 'k' till the end of the string (n-1)          *
 * Time Complexity = O(2n)                                       *
 * Space Complexity = O(1)                                       */
static inline void str_rotate_right(char * const buf, int k)
{
	int n = strlen(buf);
	/* optimization 1 - Avoid rotations beyond String Length */
	/* optimization 2 - Modulo also ensures that if k = n, no rotations done */
	k = k % n;
	if(k == 0) return;
	my_strrev_with_bounds(buf, 0, n-1);
	my_strrev_with_bounds(buf, 0, k-1);
	my_strrev_with_bounds(buf, k, n-1);
}

/* Reverse words in char array and trim spaces                   *
 * Time Complexity  = O(3n)                                      *
 * Space Complexity = O(1)                                       */
static inline void str_word_rev(char * const buf, int len)
{
	int st, sp;              /* Counters */
	if(buf == NULL || len <= 1) return;
	/* 1) Trim extra spaces */
	len = str_trim(buf, len);
    /* 2) Tokenize and reverse words, delims = space   */
	for(st = 0, sp = 0; st <= len; st++)
	{
		if(buf[st] == ' ' || buf[st] == '\0'){
			if(st != sp)     /* We have a word ! */
				my_strrev(&buf[sp], st - sp);
			sp = st + 1;
		}
	}
    /* 3) Reverse Entire string */
	my_strrev(buf, len);
}

/* Dynamic Programming -> longest Common Substring                *
 * Given 2 input strings of length n and m                        *
 * Use DP Tabulation approach                                     *
 * Time Complexity  = O(mn)                                       *
 * Space Complexity = O(mn)                                       */
int str_longest_common_substring(const char *const s1,  const int n,
							 const char * const s2, const int m)
{
	int mat[n][m];
	int lcs_length = 0;
	int s1_lcs_pos = 0, s2_lcs_pos = 0;
	int i, j;
	memset(&mat, 0, sizeof(int) * n * m);
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < m; j++)
		{
			if(s1[i] == s2[j])
			{
				if(i == 0 || j == 0)
					mat[i][j] = 1;
				else {
					mat[i][j] = mat[i-1][j-1] + 1;
					if(mat[i][j] > lcs_length) {
						lcs_length = mat[i][j];
						s1_lcs_pos = i + 1 - lcs_length;
						s2_lcs_pos = j + 1 - lcs_length;
					}
				}
			} //outer-if
			else
				mat[i][j] = 0;
		} //inner-for
	} //outer-for
	if(lcs_length)
	{
		printf("LCS of length %d found in zero-based pos:\n", lcs_length);
		printf("position %d @ '%s'\n",s1_lcs_pos, s1);
		printf("position %d @ '%s'\n",s2_lcs_pos, s2);
		print_matrix(n, m, mat);
	}
	else
		printf("No Longest Common Sub-String match found\n");
	return lcs_length;
}

#endif //_STR_HELPER_ALG__
