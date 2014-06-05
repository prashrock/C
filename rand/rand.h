#ifndef _RAND_API_
#define _RAND_API_

#include <time.h>          /* time() */
#include <stdlib.h>        /* rand() */

static inline void init_rand()   { srand(time(NULL)); }
static inline int get_rand_int() { return rand(); }
#endif //_RAND_API_
