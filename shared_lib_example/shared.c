#include <stdio.h>

/* Forward declaration */
void static_func();

/* Shared library func calling static library func  */
void shared_func() {
   printf("shared called !!\n");
   static_func();
}
