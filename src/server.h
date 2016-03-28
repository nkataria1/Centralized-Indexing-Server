#ifndef __SERVER_H_
#define __SERVER_H_

/* Optional parameters */
struct opt_params_t {
	int bandwidth;
	int demand;
};

/* List of files that a peer has */
struct file_list {
	char *file_name;
	struct file_list *link;
};

/* List of peers */
struct peer_list {
	struct file_list *list;
	char *peer_id;
	struct opt_params_t *params;
	struct peer_list *link;
};

struct msg_data {
	/* client fd used by server thread for replying */
	int client_fd;
	/* Message received from the client */
	char *buffer;
};

/* Function to search the files and return the peer_id's 
 * that have the respective files to the client 
*/
void *search (void *data);

/* Function invoked to register files with the server */
void *registry (void *data);

/* Function to register a peer */
void *deregister (void *data);

/* Function to register a file */
void *deregister_file (void *data);

#endif
