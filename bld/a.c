#include <stdio.h>
int main()
{
	int abc;
	void *x = NULL;
	x = ((char *)&abc) + 2;
	printf("%p\n", x);
	return 0;
}
