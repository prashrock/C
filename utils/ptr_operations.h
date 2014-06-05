#ifndef _PTR_ARITHMETIC_OPS_
#define _PTR_ARITHMETIC_OPS_

#include <stdint.h>   /* uintptr_t */

/* Round an unsigned int down to a given power-of-two value    *
 * It is assumed that align is a power-of-two value            */
#define ALIGN_FLOOR(val, align) \
	(typeof(val))((val) & (~((typeof(val))((align) - 1))))
static inline uintptr_t ptr_align_floor(uintptr_t ptr, uintptr_t align)
{
	return (ptr & ~(align - 1));
}


/* Round an unsigned int up to a given power-of-two value      *
 * It is assumed that align is a power-of-two value            */
static inline uintptr_t ptr_align_ceil(uintptr_t ptr, uintptr_t align)
{
	return ptr_align_floor(ptr + (align - 1), align);
}

#endif //_PTR_ARITHMETIC_OPS_
