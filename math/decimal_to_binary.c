#include <stdio.h>
#include <stdbool.h>
#include "stack.h"


/* Can handle only positive numbers */
void decimal_to_binary(int num)
{
	bool negative = false;
	printf("Converting %u into binary\n", num);
	/* Handle negative number condition */
	if(num < 0)
	{
		negative = true;
		num *= -1;
	}
	/* Keep pushing num modulo two into stack */
	while(num)
	{
		if(full())
		{
			printf("Error: stack size %d not enough\n", MAXSTACK);
			break;
		}
		push(num % 2);
		num = num / 2;
	}
	/* Handle negative number condition */
	if(negative)
	{
		int i = 0;
		for()
	}
	while(!empty())
		printf("%1d", pop());
	printf("\n");
}

int main(int argc, char **argv)
{
  if(argc != 2)
  {
    printf("Usage : %s <integer>\n", argv[0]);
    return -1;
  }
  int num = atoi(argv[1]);
  decimal_to_binary(num);
}
