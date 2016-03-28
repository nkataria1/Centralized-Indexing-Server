#ifndef _PEER_UTILS_H
#define _PEER_UTILS_H
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CONN_LISTEN 10 /* Maximum clients I can listen to */
#define MAX_MSG_SIZE 1024 /* Maximum size of the message */

struct peer_data {
	/* File names */
        char *file_name;
	/* Ip of the indexing server */
        char *server_ip;
	/* Port of the indexing server */
        char *server_port;
	/* My own IP for acting as a server */
        char *my_ip;
	/* My own Port for acting as a server */
        char *my_port;
};

/* Function to lookup and obtain a file from peer */
void *obtain (void *data);

/* Function to start a peer as a server */
void *server (void *data);

/* Function to register a peer as a server */
void *register_peer (void *data);

#endif
