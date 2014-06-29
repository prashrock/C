// gcc -Wall -g -lpthread -o linux_popen_ls linux_popen_ls.c
#include <stdio.h>
#include <stdlib.h>                  /* EXIT_FAILURE */
#include "../utils/scan_utils.h"     /* popen API */

/* Below program uses popen to list contents of CWD   */
int main()
{
	char *cmd = "ls -lha";
	if(exec_cmd_print_output(cmd) == false)
		exit(EXIT_FAILURE);
	return 0;
}


