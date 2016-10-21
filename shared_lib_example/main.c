#include <stdio.h>

/* Forward declaration */
void shared_func();

int main() {
   printf("Main -> Shared -> Static\n");
   shared_func();
}
