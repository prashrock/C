/* gcc -g -o ipc_sender ipc_sender.c  */
#include <stdio.h>
#include <stdbool.h>          /* bool, true, false */
#include <pthread.h>          /* Pthreads library */
#include <string.h>           /* memset */
#include <sys/socket.h>       /* socket/bind sys_call */
#include <sys/un.h>           /* AF_LOCAL/AF_UNIX socket - sockaddr_un */
#include "ipc_unix_stream_socket_api.h" /* Socket API's */

#define SOCK_ADDR  "/tmp/dgram_sock"

/* Client */
void unidirectional_stream_socket_sender_test()
{
	int n = 10;
	char c = 'h';
	int csock;
	struct sockaddr_un saddr_st;
	if(create_unix_stream_socket(SOCK_ADDR, &csock, &saddr_st) == false)
		goto err_handle;
	if(connect_unix_stream_socket(csock, &saddr_st) == false)
		goto err_handle;
	while(n--)
	if(send(csock, &c, sizeof(c), 0) <= 0) {
		perror("Error - client send()::");
		goto err_handle;
	}

err_handle:
	destroy_unix_stream_socket(csock);
}

/* Server */
void unidirectional_stream_socket_receiver_test()
{
	int n = 10;
	char c = 'a';
	int csock = -1, ssock = -1, recv_len;
	struct sockaddr_un caddr_st, saddr_st;
	if(create_unix_stream_socket(SOCK_ADDR, &ssock, &saddr_st) == false)
		goto err_handle;
	if(bind_unix_stream_socket(ssock, &saddr_st) == false)
		goto err_handle;
	/* Block to receive 1 client connection */
	if(listen_unix_stream_socket(ssock,
								 UNIX_STREAM_SOCKET_NUM_CLIENTS) == false)
		goto err_handle;
	if(accept_unix_stream_socket(ssock, &csock, &caddr_st) == false)
		goto err_handle;
	while(n--)
	{
	if((recv_len = recv(csock, &c, sizeof(c), 0)) <= 0) {
		perror("Error - server recv()::");
		goto err_handle;
	}

	printf("%d. Server got '%c'\n", 10-n, c);
	}
err_handle:
	if(csock != -1) destroy_unix_stream_socket(csock);
	if(ssock != -1) destroy_unix_stream_socket(ssock);
}
	
int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("Usage : %s <server/client>\n", argv[0]);
		return -1;
	}
	if(strncmp(argv[1], "server", strlen("server")) == 0)
		unidirectional_stream_socket_receiver_test();
	else if(strncmp(argv[1], "client", strlen("client")) == 0)
		unidirectional_stream_socket_sender_test();
	return 0;
}
