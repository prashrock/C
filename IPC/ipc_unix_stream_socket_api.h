#ifndef _IPC_UNIX_STREAM_SOCKET_API_
#define _IPC_UNIX_STREAM_SOCKET_API_
#include <stdio.h>
#include <stdbool.h>          /* bool, true, false */
#include <pthread.h>          /* Pthreads library */
#include <string.h>           /* memset */
#include <stddef.h>           /* offsetof */
#include <unistd.h>           /* unlink */
#include <sys/socket.h>       /* socket/bind sys_call */
#include <sys/un.h>           /* AF_LOCAL/AF_UNIX socket - sockaddr_un */

/* How many clients connected to one server */
#define UNIX_STREAM_SOCKET_NUM_CLIENTS  1

/* NOTES: (About AF_UNIX family of sockets)                            *
 * AF_UNIX sockets are bi-directional (advantage over pipe/msg_queue)  *
 * Client bind() unlike AF_INET, where krnl auto-binds sock to rnd port*
 * write()/send() - will block if receiver buffer is full.             */


/* Since sockaddr_un contains a path, its size is not constant         */
static inline size_t get_sockaddr_un_size(struct sockaddr_un *addr)
{
	return offsetof(struct sockaddr_un, sun_path) + strlen(addr->sun_path);
}

/* Create a UNIX Stream socket @ given path (need write permissions) and*
   unlink's the path (path permissions restrict socket usage)          */
static inline bool create_unix_stream_socket(char *path, int *sock,
											struct sockaddr_un *sock_st)
{
	if(!path || !sock || !sock_st){
		printf("Error - invalid parameters to create_unix_stream_socket\n");
		return false;
	}
	
	/* Create UNIX Domain Socket(IPC Socket) - works within one OS*
	 * Acts as a two-way FIFO (Pipe) using socket interface       *
	 * Each client using socket has independant connection to srvr*
	 * Using STREAM instead of STREAM for reliability & pkt order  */
	if((*sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror("Error - create_unix_stream_socket() - socket::");
		return false;
	}
		
	/* Initialize sockaddr_un structure */
	sock_st->sun_family = AF_UNIX;
	/* sockaddr_un.sun_path is 108, path should be less than 108B */
	strncpy(sock_st->sun_path, path, sizeof(sock_st->sun_path));

	return true;
}

/* Server/Client calls this function to bind on AF_UNIX socket     *
 * Unlike AF_INET, where OS automatically binds to a random port   *
 * AF_UNIX family sockets needs to explicitly call bind            */
static inline bool bind_unix_stream_socket(int sock,
										  struct sockaddr_un *sock_st)
{
	int len;
	if(sock < 0 || !sock_st){
		printf("Error - invalid parameters to bind_unix_stream_socket\n");
		return false;
	}

	len = get_sockaddr_un_size(sock_st);

	/* Path file will continue to exist, even after the server exits *
	 * presence of this file will prevent re-binding upon restart    */
	unlink(sock_st->sun_path);
	
	if(bind(sock, (struct sockaddr *)sock_st, len) < 0){
		perror("Error - bind_unix_stream_socket() - bind::");
		return false;
	}
	return true;
}
	
/* Server calls this function to start listening on socket  */
static inline bool listen_unix_stream_socket(int sock, int cnt)
{
	/* No check on sock/sock_st as its already done in bind  */
	if (listen(sock, cnt) < 0) {
		perror("Error - listen_unix_stream_socket() - listen::");
		return false;
	}
	return true;
}

/* Server calls this function to accept client connection   */
static inline bool accept_unix_stream_socket(int ssock, int *csock,
										  struct sockaddr_un *csock_st)
{
	unsigned int len = get_sockaddr_un_size(csock_st);
	if(!csock || !csock_st){
		printf("Error - invalid parameters to accept_unix_stream_socket\n");
		return false;
	}
	/* No check on ssock as its already done in bind  */
	if ((*csock = accept(ssock, (struct sockaddr *)csock_st, &len)) < 0) {
		perror("Error - accept_unix_stream_socket() - accept::");
		return false;
	}
	return true;
}

/* Client calls this function to bind socket + connect to server    */
static inline bool connect_unix_stream_socket(int sock,
										  struct sockaddr_un *sock_st)
{
	/* No check on sock/sock_st as its already done in bind  */
	int len = get_sockaddr_un_size(sock_st);
	if (connect(sock, (struct sockaddr *)sock_st, len) < 0) {
		perror("Error - connect_unix_stream_socket() - connect::");
		return false;
	}
	return true;
}

static inline bool destroy_unix_stream_socket(int sock)
{
	close(sock);
	return true;
}

#endif //_IPC_UNIX_STREAM_SOCKET_API_
