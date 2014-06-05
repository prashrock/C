#ifndef _BIT_OPS_
#define _BIT_OPS_

#include <stdbool.h> /* bool, true, false */
#include <limits.h>  /* ULONG_MAX, CHAR_BIT - len of char in bits */

/* Points to remember                                                  *
 * = In C, shift more than BITS_PER_LONG-1 is undefined                *
 * = Below Equality can help calc floor((x+y)/2) without overflow      *
 *   (x + y) == ((x & y) + (x | y)) == ((x ^ y) + 2 * (x & y))         *
 * = Generate 0 on success, -1 on failure (any err# x)                 *
 *   (((int) (x | -x)) >> (WORDBITS - 1))                              *
 * = Generate 0 on success, 1 on failure (any err# x)                  *
 *   (((unsigned int) (x | -x)) >> (WORDBITS - 1))                     *
 * = Given 2's complement int (x,y), find minimum and maximum of both  *
 *   integers, without branch, assuming right shift extends sign bit   *
 *   Min(x, y) => x + ( ((y-x)>>(WORDBITS-1)) & (y-x) )                *
 *   Max(x, y) => x - ( ((x-y)>>(WORDBITS-1)) & (x-y) )                *
 * = Given 2's comp int (x,y), implement ternary condition (?) operator*
 *   without branch, assuming rt shift extends sign (not portable in C)*
 *   (a<b)?x=c:x=d => x = ( ( ((a-b) >> (WORDBITS-1)) & (c^d) ) ^ d)   *
 */

#define WORD_BIT (CHAR_BIT * sizeof(unsigned int)) //32
#define HEXIFY_INT(_X_) 0x##_X_##U
#define HEXIFY_LONG(_X_) 0x##_X_##LU
/* This approach can only cover binary to int upto 0xFF */
#define BINARY_TO_HEX_INT(_Y_) (((_Y_&0x00000001U)?1:0)   + \
								((_Y_&0x00000010U)?2:0)   + \
								((_Y_&0x00000100U)?4:0)   + \
								((_Y_&0x00001000U)?8:0)   + \
								((_Y_&0x00010000U)?16:0)  + \
								((_Y_&0x00100000U)?32:0)  + \
	                            ((_Y_&0x01000000U)?64:0)  + \
	                            ((_Y_&0x10000000U)?128:0))

/* Determine if n_ is a power-of-two */
#define IS_POW2(n_) ((n_) != 0 && (((n_) & ((n_) - 1)) == 0))

/* test if x is Even or Odd - Works for all integers (Z)  */
static inline bool is_even(int n) {return ((n & 1) == 0); }
static inline bool is_odd(int n)  {return (!is_even(n));  }
/* Following Bit Ops use 0 based bit-position idx         *
 * and work for all integers (Z - scale)                  *
 * If left shift operand causes value bigger than width of*
 * operand, the operation is undefined (dont do 1 << -1)  */
static inline int bit_set(int n, unsigned int pos_zb)
{return (n |= 1 << pos_zb); }
static inline int bit_unset (int n, unsigned int pos_zb)
{return (n &= ~(1 << pos_zb)); }
static inline int bit_toggle(int n, int pos_zb)
{return (n ^= (1 << pos_zb));}
/* Reverse bits of given input.                           *
 * Works by parallely reversing 1, 2, 4,... bits          *
 * Works for +/- integers                                 */
static inline unsigned int bit_reverse(register unsigned int n)
{
	n = (((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1));
	n = (((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2));
	n = (((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4));
	n = (((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8));
	return((n >> 16) | (n << 16));
}
/* Below code sets all 1's to the right of the MSB       *
 * in constant time.                                     */
static inline unsigned int
bit_propagate_ones_towards_right(unsigned int n)
{
	n |= n >> 1; /* Shift 1-bits and OR it with itself */
	n |= n >> 2; /* Shift 2-bits and OR it with itself */
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return n;
}
static inline int bit_get_msb_pos(int n)
{
	int pos_zb = 0;
	if(n & 0xFFFF0000) {pos_zb += 16; n >>= 16;}
	if(n &     0xFF00) {pos_zb += 8;  n >>= 8;}
	if(n &       0xF0) {pos_zb += 4;  n >>= 4;}
    if(n &        0xC) {pos_zb += 2;  n >>= 2;}
    if(n &        0x2) {pos_zb += 1;  n >>= 1;}
	if(n || pos_zb) return pos_zb; /* 0 based pos index   */
	else         return -1;
}
static inline int bit_get_msb_val(int n)
{
	n = bit_propagate_ones_towards_right(n);
	return(n & ~(n >> 1));
}
/* # set 1's (Hamming Weight of bitstream) in const time  *
 * The core idea is to do a parallel sum of bits          *
 * 1st statement sums up individual 1's with bitmask 0101 *
 * 2nd sums up 2 bits at a time with bitmask 0011         *
 * This continues, till total sum is reached == bit-count *
 * This approach is better than O(lg n) Kernighan approach*
 * which uses (n &= n - 1) to calculate population count  *
 * Works for +/- integers                                 */
static inline int bit_population_count(unsigned int n)
{
	if(n == 0) return 0;
	n = (n & 0x55555555) + ((n >> 1) & 0x55555555); //Accumulate 1 bit
	n = (n & 0x33333333) + ((n >> 2) & 0x33333333); //Accumulate 2 bits
	n = (n & 0x0F0F0F0F) + ((n >> 4) & 0x0F0F0F0F); //Accumulate 4 bits
	n = (n & 0x00FF00FF) + ((n >> 8) & 0x00FF00FF); //Accumulate 8 bits
	n = (n & 0x0000FFFF) + ((n >>16) & 0x0000FFFF); //Accumulate 16 bits
	return n;
}
/* # set 1's in lg(n) time - Brian W. Kernighan method    *
 * This approach works best in sparcely populated case    *
 * Works for +/- integers                                 */
static inline int bit_population_count2(unsigned int n)
{
	unsigned int i = 0;
	/* Same logic as bit_is_pow2 */
	for(i = 0; n; i++) n &= (n - 1);
	return i;
}
/*  Similar to finding MSB. Works for +/- integers        */
static inline int bit_num_leading_zeros(unsigned int n)
{
	if(n == 0) return WORD_BIT; //32
	else
	{
		int cnt = 0;
		if(n <= 0x0000FFFF) {cnt += 16; n <<= 16;}
		if(n <= 0x00FFFFFF) {cnt +=  8; n <<=  8;}
		if(n <= 0x0FFFFFFF) {cnt +=  4; n <<=  4;}
		if(n <= 0x3FFFFFFF) {cnt +=  2; n <<=  2;}
		if(n <= 0x7FFFFFFF) {cnt +=  1;}
		return cnt;
	}
}
/*  Similar to finding MSB. Works for +/- integers        */
static inline int bit_num_trailing_zeros(unsigned int n)
{
	if(n == 0) return (CHAR_BIT * sizeof(unsigned int)); //32
	else
	{
		int cnt = 0;
 		if((n & 0x0000FFFF) == 0) {cnt += 16; n >>= 16;}
		if((n &     0x00FF) == 0) {cnt +=  8; n >>=  8;}
		if((n &       0x0F) == 0) {cnt +=  4; n >>=  4;}
		if((n &        0x3) == 0) {cnt +=  2; n >>=  2;}
		if((n &        0x1) == 0) {cnt +=  1; n >>=  1;}
		return cnt;
	}
}
/* If bit_pop() is efficient, we can avoid branches in   *
 * in previous implementation:                           *
 * Works for +/- integers                                */
static inline int bit_num_leading_zeros2(unsigned int n)
{
	n = bit_propagate_ones_towards_right(n);
	return (WORD_BIT - bit_population_count(n));
}
/* If bit_pop() is efficient, we can avoid branches in   *
 * in previous implementation:                           *
 * Works for +/- integers                                */
static inline int bit_num_trailing_zeros2(unsigned int n)
{
	//return pop_count(lsb_val - 1)
	return (bit_population_count((n & -n) - 1));
}
/* Next highest number with Same Number Of One Bits(SNOOB)*
 * Locate smallet set 1 bit (LS1B), add it to input to    *
 * create a ripple, XOR ripple with inp to get all 1 bits *
 * Shift 1 bits right until it becomes least signif bits  *
 * (divide by LS1B).                                      *
 * Shift again by 2 to take two compensating bits off.    *
 * Or ones with ripple to get the next highest number     *
 * Works only for + integers                              *
 * Taken from HAKMEM 175 (from Hacker's Delight)          */
static inline unsigned int bit_snoob(unsigned int n)
{
	unsigned int smallest, ripple, ones;
	if(n == 0) return 0;
                                       	// n = xxx0 1111 0000
	smallest = n & -n;                  //     0000 0001 0000
	ripple = n + smallest;              //     xxx1 0000 0000
	ones = n ^ ripple;                  //     0001 1111 0000
	ones = (ones >> 2)/smallest;        //     0000 0000 0111
	return (ripple | ones);             //     xxx1 0000 0111
	//Can avoid division with following:
	//ones = (1 << (pop(ones) - 2)) - 1;
}
/* floor(log2(n)) = MSB_POS(n) - 1                        *
 * log2(0) is undefined, so return -1 for that            *
 * Works only for + integers                              */
static inline unsigned int bit_floor_log2_impl(unsigned int n)
{
	n = bit_propagate_ones_towards_right(n);
	return (bit_population_count(n) - 1);
}

/* ceil and floor are identical if n is a power of two    *
 * Do power_of_two test, and add this result (-(-1))      *
 * log2(0) is undefined, so return -1 for that            *
 * Works only for + integers                              */
static inline unsigned int bit_ceil_log2_impl(unsigned int n)
{
	int y = n & (n - 1);
	y |= -y;
	y >>= (WORD_BIT - 1);
	/* Now y = 0 if n is pow_2 or y = -1 if n not pow_2    */
	return (bit_floor_log2_impl(n) - y);
}

/* 1010_1000 -> 0000_1000 */
static inline int bit_get_lsb_val(int n)
{return (n & -n); }
/* 1010_1111 -> 0001_0000 */
static inline int bit_get_rightmost_zero_val(int n)
{return ((~n) & (n + 1)); }
/* 1010_1111 -> 1010_1110 */
static inline int bit_turnoff_rightmost_bit(int n)
{return (n & (n - 1)); }
/* 1010_0111 -> 1010_1111 */
static inline int bit_turnon_rightmost_bit(int n)
{return (n | (n + 1)); }
/* 1010_1000 -> 1010_1111 */
static inline int bit_turnon_trailing_zeros(int n)
{return (n | (n - 1));}
/* 1010_1111 -> 1010_0000 */
static inline int bit_turnoff_trailing_ones(int n)
{return (n & (n + 1));}
static inline bool bit_is_pow2(unsigned int n)
{return ((n > 1) && ((n-1) & n) == 0);}
/* Round up a number to the nearest power-of-two               *
 * If number is already a power-of-two, return number as is    *
 * Works only for + integers                                   */
static inline unsigned int bit_align_up_nearest_pow2(unsigned int n)
{
	n--;  /*Handle Case when n is already power-of-two*/
	n = bit_propagate_ones_towards_right(n);
	/* After all 1's are set, add 1 to get the next pow-of-two */
	return n + 1 ;
}
/* Align down n to nearest power of b (power-of-two)           *
 * Note - b must be a power-of-two                             *
 * Works only for + integers                                   */
static inline unsigned int bit_align_down(unsigned int n,
										  unsigned int b)
{
	/* Use property -b = ~(b-1).                    *
	 * If b is constant then -b is constant as well */
	return n & -b;
}
/* Align up n to nearest power of b (power-of-two)             *
 * Note - b must be a power-of-two                             *
 * Works only for + integers                                   */
static inline unsigned int bit_align_up(unsigned int n,
										  unsigned int b)
{	return bit_align_down( (n + (b - 1)), b );  }
/* Convert +ve/-ve Integer from Decimal to Binary              */
static inline void print_integer_in_binary(int n)
{
	const int int_bit_count = sizeof(int) * CHAR_BIT;
	unsigned int un = n;
	int sign_bit = 0;
	int i = int_bit_count;
	char str[int_bit_count + 1];
	memset(str, '0', sizeof(str));
	str[i--] = '\0';
		
	/* If n == 0, store '0' and avoid while loop */
	if(n == 0) str[i--] = '0';
	else
	{
		/* If -ve integer, remove MSB, otherwise right shift wont work */
		/* Clear Sign bit(-ve integer) = AND with 0x7FFF_FFFF */
		un &= (((unsigned int)1 << (int_bit_count-1)) - 1);
		sign_bit = ((int)un == n) ? 0 : 1;
		/* Calculate all bits except MSB */
		while(un)
		{
			str[i--] = '0' + (un & 1);
			un >>= 1;
		}
		if(sign_bit)
		{
			str[0] = '1';
			i = -1;
		}
	}
	i++;
	printf("%d in Binary is '%s'\n", n, &str[i]);
}

static inline int bit_ntz(unsigned int n)
{return bit_num_trailing_zeros(n);}
static inline int bit_nlz(unsigned int n)
{return bit_num_leading_zeros(n);}
static inline int bit_pop(unsigned int n)
{return bit_population_count(n);}
static inline unsigned int bit_rev(unsigned int n)
{return bit_reverse(n);}
static inline bool is_bit_set(int n, unsigned int pos_zb)
{return (n & bit_set(0, pos_zb)); }
static inline int bit_get_lsb_pos(int n)
{return bit_get_msb_pos(bit_get_lsb_val(n)); }
static inline int bit_get_rightmost_zero_pos(int n)
{return bit_get_msb_pos(bit_get_rightmost_zero_val(n)); }
static inline int bit_clear_msb(int n) /* Use when no __builtin_clz */
{return bit_unset(n, bit_get_msb_pos(n)); }
static inline int bit_clear_lsb(int n) /* Use when no __builtin_ctz */
{return bit_turnoff_rightmost_bit(n);}
static inline int bit_turn_off_rightmost_bit(int n)
{return bit_turnoff_rightmost_bit(n);}
static inline int bit_floor_log2(int n)
{if(n<=0) return -1; else return bit_floor_log2_impl(n);}
static inline int bit_ceil_log2(int n)
{if(n<=0) return -1; else return bit_ceil_log2_impl(n);}

/* Generate all possible subsets of input:                *
 * Each subset has 0 or more in-place bits turned in input*
 * = All possible in-place permutations with replacement  *  
 * Total number of such subsets = 2^number_of_1s_in_input *
 * For example 6 (110) = 010, 100, 110, 0                 *
 * To achieve this generation, start with n = 0 and:      *
 * Start or'ing 1's complement of input with n            *
 * Each iteration of prev step generates a ripple which is*
 * propagated through all unused bits until we reach all 0*
 * i.e., start with n = 0, and keep repeating             *
 * n = (n | (~x + 1)) & x ->Or n with 1's complement      *
 * n = ((n + ~x)+ 1)  & x ->Replace OR with ADD(Mutual ex)*
 * n = ((n+(-x-1))+1) & x ->Use 2's comp property -x= ~x+1*
 * n =     ( n - x)   & x -> 1 gets cancelled out         *
 * Works for +/- integers                                 */
static inline int bit_inplace_permutate(unsigned x,
										void (*fn)(unsigned x))
{
	int cnt = 0;
	unsigned n = 0;
	do{
		/* Should be n = (n | (~x + 1)) & x; */
		n = (n - x) & x;
		cnt++;
		print_integer_in_binary( (int)n );
		if(fn) fn(n);
	}while(n);
	return cnt;
}
static inline int bit_next_biggest_number_same_num_ones(int n)
{
	if (n <= 0)                 return 0;
	else
	{
		/* Sanity checks needed for snoob() */
		int ret = bit_snoob(n);
		if(ret < 0 || ret < n) return 0;
		else                   return ret;
	}
}
static inline int bit_next_smallest_number_same_num_ones(int n)
{
	if (n <= 0)                 return 0;
	else
	{
		/* Sanity checks needed for snoob() */
		int ret = ~bit_snoob(~n);
		if(ret < 0 || ret > n) return 0;
		else                   return ret;
	}
}
/* Generate all possible arrangements with same # ones    *
 * #Ways to arrange r unique objects in n positions = nPr *
 * Way to arrange 2 bits in 31 (signed int) = 31P2 = 465  *
 * Use snoob() wrappers to generate bigger and smaller #  *
 * with same number of ones                               *
 * Works only for + integers                              */
static inline int bit_generate_arrangements(unsigned x,
										void (*fn)(unsigned x))
{
	int cnt = 0;
	unsigned orig_x = x;
	/* Generate all permutations less than equal to x */
	while(x)
	{
		cnt++;
		print_integer_in_binary( (int)x );
		if(fn)  fn(x);
		x = bit_next_smallest_number_same_num_ones(x);
	}
	/* Generate all permutations greater than x */
	x = bit_next_biggest_number_same_num_ones(orig_x);
	while(x)
	{
		cnt++;
		print_integer_in_binary( (int)x );
		if(fn)  fn(x);
		x = bit_next_biggest_number_same_num_ones(x);
	}
	return cnt;
}


#endif //_BIT_OPS_
