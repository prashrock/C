//gcc -Wall -g -lpthread -o linux_daemon_example linux_daemon_example.c
#include <stdio.h>    //printf(3)
#include <stdlib.h>   //exit(3)
#include <syslog.h>   //syslog(3), openlog(3), closelog(3)
#include "../utils/terminal_utils.h"

int
main()
{
	if( (daemonize("mydaemon","/tmp/", NULL, NULL, NULL)) == false ) {
		fprintf(stderr,"error: daemonize failed\n");
		exit(EXIT_FAILURE);
	}

	syslog(LOG_ERR, "hiya - test syslog"); //LOG_NOTICE
	printf("hello worldy\n\n");            //Test STDOUT
	closelog();
	return(EXIT_SUCCESS);
}

