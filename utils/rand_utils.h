#ifndef _RAND_UTILS_API_
#define _RAND_UTILS_API_
#include <stdio.h>
#include <stdbool.h>       /* bool, true, false */
#include <stdlib.h>        /* srand(), rand()  */
#include <time.h>          /* get current time - time() */

static inline void srand_cur_time()     {return srand(time(NULL));}
static inline int  rand_int(int limit)  {return rand() % limit;}
#endif //_RAND_UTILS_API_
