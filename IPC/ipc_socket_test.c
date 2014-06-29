/* gcc -Wall -g -o ipc_sender ipc_sender.c  */
#include <stdio.h>
#include <stdbool.h>          /* bool, true, false */
#include <pthread.h>          /* Pthreads library */
#include <string.h>           /* memset */
#include <sys/socket.h>       /* socket/bind sys_call */
#include <sys/un.h>           /* AF_LOCAL/AF_UNIX socket - sockaddr_un */
#include "parser.h"           /* Parser API */
#include "multi_thread.h"     /* PThread helper  */
#include "scan_utils.h"       /* input_generic_api */
#include "print_utils.h"      /* Tee output to file */
#include "ipc_unix_stream_socket_api.h" /* Socket API's */

#define SOCK_ADDR  "/tmp/stream_sock"

/* Client - Terminal where input is entered */
static void unidirectional_stream_socket_sender()
{
	char c = 'h';
	int csock;
	struct sockaddr_un saddr_st;
	if(create_unix_stream_socket(SOCK_ADDR, &csock, &saddr_st) == false)
		goto err_handle;
	if(connect_unix_stream_socket(csock, &saddr_st) == false)
		goto err_handle;

	printf("Client::\n");
	while(1)
	{
		//if(input_generic_api(&c, 1) <= 0)
		//	break;
		if((c = getch()) <= 0)
			continue;
		if(send(csock, &c, sizeof(c), 0) <= 0) {
			perror("Error - client send()::");
			goto err_handle;
		}
	}

err_handle:
	destroy_unix_stream_socket(csock);
}

/* Server - terminal where Client output is echoed  */
static void unidirectional_stream_socket_receiver()
{
	char c = 'a';
	int csock = -1, ssock = -1, recv_len;
	struct sockaddr_un caddr_st, saddr_st;

	if(create_unix_stream_socket(SOCK_ADDR, &ssock, &saddr_st) == false)
		goto err_handle;
	if(bind_unix_stream_socket(ssock, &saddr_st) == false)
		goto err_handle;
	printf("Server (waiting for 1 client)::\n");
	/* Block to receive 1 client connection */
	if(listen_unix_stream_socket(ssock,
								 UNIX_STREAM_SOCKET_NUM_CLIENTS) == false)
		goto err_handle;
	if(accept_unix_stream_socket(ssock, &csock, &caddr_st) == false)
		goto err_handle;
	printf("Info - Client connected, echo the client\n");
	while(1)
	{
		fflush(stdout);
		if((recv_len = recv(csock, &c, sizeof(c), 0)) <= 0) {
			if(recv_len < 0){
				perror("Error - server recv()::");
				goto err_handle;
			}
			else break;
		}
		putchar(c);
	}
	printf("Client left.\n");
	fflush(stdout);
err_handle:
	if(csock != -1) destroy_unix_stream_socket(csock);
	if(ssock != -1) destroy_unix_stream_socket(ssock);
}

static void print_usage(char *prog_name)
{
	printf("Usage : %s <server/client> [log]\n", prog_name);
}

int main(int argc, char **argv)
{
	bool log = false;
	term_clear_screen();
	term_move_cursor_to_top_left();
	if(argc == 3 && strncmp(argv[2], "log", strlen("log")) == 0)
		log = true;
	if(argc < 2 || argc > 3)
	{
		print_usage(argv[0]);
		return -1;
	}

	/* Server sticks around forever, handling 1 client at a time */
	if(strncmp(argv[1], "server", strlen("server")) == 0)
	{
		FILE *tee_file;
		if(log && open_tee_stdout_to_file("server_log.txt", &tee_file) == false)
			return -1;
		while(1){
			term_clear_screen();
			term_move_cursor_to_top_left();
			unidirectional_stream_socket_receiver();
		}
		if(log)
		{
			printf("\nKill server manually -- popen(tee) does not return\n");
			close_tee_stdout_to_file(tee_file);
		}
	}
	else if(strncmp(argv[1], "client", strlen("client")) == 0)
		unidirectional_stream_socket_sender();
	else
		print_usage(argv[0]);
	return 0;
}

