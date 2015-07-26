#ifndef _COMPILER_API_MACROS_
#define _COMPILER_API_MACROS_

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Return the offset of a field in a structure --- */
#ifndef offsetof
#define offsetof(TYPE, MEMBER)  __builtin_offsetof (TYPE, MEMBER)
#endif

/* ---- Generic Macro to swap 2 variables --- */
#define SWAP(_x, _y) do {					  \
	typeof(_x) temp_swap_var_ = (_x);         \
	(_x) = (_y);                              \
	(_y) = temp_swap_var_;    				  \
	} while(0)

/* ---------- Compile Time Checks ---------- */
#define COMPILE_TIME_CHECK(condition) \
	((void)sizeof(char[1 - 2*!!(condition)]))

/* ---------- Branch Prediction MACRO's ---------- */
#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

/* --- Mark Var/Fn as used/unused to avoid compiler warning --- */
#define _set_as_used(x) (void)(x)
#define _set_as_unused __attribute__((__unused__))

/* ---------- Determine Type and find MAX/MIN ---------- */
#define MAX(a,b) ({ typeof (a) _a = (a); \
			typeof (b) _b = (b);		 \
			_a > _b ? _a : _b; })
#define MIN(a,b) ({ typeof (a) _a = (a); \
			typeof (b) _b = (b);		 \
			_a > _b ? _b : _a; })

static inline void block_api()
{
	while(1);
}

#ifdef __cplusplus
}
#endif
#endif //_COMPILER_API_MACROS_
