#include <stdio.h>
#include <stdlib.h>  /* atoi  */
#include <stdbool.h> /* bool, true, false */
#include <ctype.h>   /* isspace */
#include "parser.h"  /* Parser API */

void handle_CLI(void (*single_char_CLI)(const char *c),
				void (*single_char_CLI_with_num)(const char *c),
				void (*multi_char_CLI)(const char *c))
{
	int exit_condition = 0;
	char inp[200];
	do
	{
		if(!exit_condition)
			printf("Command>");
		
		const char *s;
		if(fgets(inp, sizeof(inp), stdin) == NULL)
		{
			printf("Input error, exiting\n");
			break;
		}
	
		/* Remove leading spaces */
		for(s  = inp; (*s != '\n') && isspace(*s); s++);

        /* Quit on Blank Line */
		if( *s == '\n')
		{	
			printf("Blank Line entered, sure you want to quit [y|n]:");
			exit_condition = 1;
		}
		else
		{
			/* Single character commands */
			if(*(s+1) == '\n')
			{
				if(exit_condition)
				{
					if(*s == 'y' || *s == 'Y')
						break;
					else if(*s == 'n' || *s == 'N')
						exit_condition = 0;
				}
				else if(single_char_CLI)
					single_char_CLI(s);
			}
            /* Single character commands with numbers is default */
			else if(*(s+1) >= '0' && *(s+1) <= '9')
			{
				if(single_char_CLI_with_num)
					single_char_CLI_with_num(s);
			}
            /* Multi-Word commands come here */
			else if(multi_char_CLI)
			{
				multi_char_CLI(s);
			}
		}
	}while(1);
}
