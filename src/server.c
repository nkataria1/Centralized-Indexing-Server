#include <stdio.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MSG_SIZE 1024
#define MAX_NUM_THREADS 1024

struct peer_list *head;
static int num_threads = 0;

struct peer_list * search_peer (char *peer_id) {
	struct peer_list *temp;
	temp = head;
	while (temp != NULL ) {
		printf("Checking '%s' : '%s'\n", temp->peer_id, peer_id);
		if (!strcmp(temp->peer_id, peer_id)) {
			return temp;
			break;
		}
		temp = temp->link;
	}
	return NULL;
}

/* Function to search the files and return the peer_id's
 * that have the respective files to the client 
*/
void *search (void *data) {
	struct msg_data  *params = (struct msg_data *)data;
	char *extract, *file_name;
	extract = strtok (params->buffer,":");
	extract = strtok(NULL,":");

	/* Extract the file_name */
	file_name = malloc(sizeof(extract));
	strcpy(file_name, extract);
	int len = strlen(file_name);

	printf("File name is %s Length %d\n",file_name,len);

	struct peer_list *temp;
	struct file_list *temp_list;
	int n;
	temp = head;
	char *buffer = NULL;
	while (temp != NULL ) {
		printf("Checking peer %s\n", temp->peer_id);
		temp_list = temp->list;
		while (temp_list != NULL) {
			if (!strncmp(temp_list->file_name, file_name, len)) {
				if (buffer == NULL) {
					buffer = malloc(sizeof(MAX_MSG_SIZE));
					strcpy(buffer, temp->peer_id);	
				} else {
					strcat(buffer, "\n");
					strcat(buffer, temp->peer_id);
				}
			}
			temp_list = temp_list->link;
		}
		temp = temp->link;
	}
	n = send(params->client_fd, buffer, (int)strlen(buffer),0);
	printf("number of bytes written %d",n);
	num_threads--;
	return 0;
}

void *deregister(void *data) {
        struct msg_data  *params = (struct msg_data *)data;
        char *extract, *peer_id;
        extract = strtok (params->buffer,":");
        extract = strtok(NULL,":");
        struct peer_list *temp;
        temp = head;

        /* Extract the peer_id */
        peer_id = malloc(sizeof(extract));
        strcpy(peer_id, extract);
        while (temp != NULL ) {
                printf("Checking '%s' : '%s'\n", temp->peer_id, peer_id);
                if (!strcmp(temp->peer_id, peer_id)) {
			if (temp == head) {
				head = temp->link;
				free(temp);
			} else {
				temp->peer_id = temp->link->peer_id;
				temp->link = temp->link->link;
				temp->params = temp->link->params;
				free(temp->link);
			}	
                        break;
                }
                temp = temp->link;
        }
        return 0;
}

/* Function invoked to register files with the server */
void *registry (void *data) {
	struct msg_data  *params = (struct msg_data *)data;
	struct peer_list *peer;
	struct file_list *list;
	int n;
	char *extract, *peer_id, *file_name;
	char *buffer = malloc(sizeof(1024));
	extract = strtok (params->buffer,":");
	extract = strtok(NULL,":");
	peer_id = malloc(strlen("255.255.255.255:99999"));
	strcpy(peer_id, extract);

	/* Extract the peer_id */
	extract = strtok (NULL, ":");
	char *port = malloc (sizeof(char[6]));
	strcpy(port, extract);
	strcat(peer_id, ":");
	strcat(peer_id, port);

	peer = search_peer (peer_id);
	if (peer == NULL) {
		printf ("New peer registered %d\n", params->client_fd);
		peer = (struct peer_list *) malloc (sizeof(struct peer_list));

		/* Take a copy of peer_id */
		peer->peer_id = malloc(strlen(peer_id));
		strcpy(peer->peer_id, peer_id);
		peer->peer_id[strlen(peer_id)] = '\0';

		peer->list = NULL;
	}	

	/* Extract the files from the request */

	extract = strtok(NULL,":");
	char *temp = strtok(extract," ");

	while(temp != NULL) {
		file_name = malloc(sizeof(temp));
		strncpy(file_name, temp, (int)strlen(temp));
		file_name[strlen(temp)] = '\0';

		printf("Registring file %s\n", file_name);	
		list = (struct file_list *) malloc (sizeof(struct file_list));
		list->file_name = malloc(strlen(file_name));
		strncpy(list->file_name, file_name, (int)strlen(file_name));
		if (list->file_name == NULL) {
			printf("Couldnt register a file\n");
			continue;
		}
		list->link = peer->list;
		peer->list = list;
		temp = strtok(NULL, " ");
	}
	peer->link = head;
	head = peer;

	printf("Registered peer %s\n", head->peer_id);
	sprintf(buffer, "Registered files %s", head->peer_id);
	send(params->client_fd, buffer, strlen(buffer), 0);
	close(params->client_fd);
	num_threads--;
	return 0;
}

void *deregister_file (void *data) {
	struct msg_data  *params = (struct msg_data *)data;
	struct peer_list *peer;
	struct file_list *list;
	int n;
	char *extract, *peer_id, *file_name;
	char *buffer = malloc(sizeof(1024));
	extract = strtok (params->buffer,":");
	extract = strtok(NULL,":");
	peer_id = malloc(strlen("255.255.255.255:99999"));
	strcpy(peer_id, extract);

	/* Extract the peer_id */
	extract = strtok (NULL, ":");
	char *port = malloc (sizeof(char[6]));
	strcpy(port, extract);
	strcat(peer_id, ":");
	strcat(peer_id, port);

	extract = strtok(NULL,":");
	char *temp = strtok(extract," ");

	peer = search_peer(peer_id);
	if (peer != NULL) {
		list = peer->list;
		while (list != NULL) {
			if (!strcmp(list->file_name, file_name)) {
				if (list->file_name) free (list->file_name);
				list->file_name = malloc(strlen(list->link->file_name));
				strncpy(list->file_name, list->link->file_name, strlen(list->link->file_name));
				free(list->link);
				break;
			}
			list = list->link;
		}
	} else {
		printf("No such peer exists");
	}
			
	num_threads--;
	return 0;
}

int main (int argc, char *argv[]) {

	static int num_threads = 0;
	struct msg_data *data;
	data = malloc(sizeof(struct msg_data));
	int server_id, client_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_storage server_str;
	socklen_t server_addr_size;
	int flag;

	server_id = socket(PF_INET, SOCK_STREAM, 0);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

	bind(server_id, (struct sockaddr *) &server_addr, sizeof(server_addr));

	flag  = listen(server_id,10);

	if (flag != 0) {
		printf("Listening\n");
	}
	server_addr_size = sizeof(server_str);

	while(1) {
		client_fd = accept(server_id, (struct sockaddr *) &server_str, &server_addr_size);

		char buffer[1024];
		pthread_t thread;
		memset(buffer, '\0', (int)strlen(buffer));
		recv (client_fd, buffer, MAX_MSG_SIZE, 0);
		printf("Received message from client %s\n",buffer);
		data->buffer = malloc(strlen(buffer));
		strcpy(data->buffer, buffer);
		data->client_fd = client_fd;
		if (strstr(buffer, "lookup")) {
			if (num_threads == MAX_NUM_THREADS) {	
				printf("Max Thread Size Reached.. WAIT\n");
				continue;
			}
			printf("Spawning a thread for search request\n");
			num_threads++;
			pthread_create(&thread, NULL, search, (void *)data);
		} else if (strstr(buffer, "register")) {
			if (num_threads == MAX_NUM_THREADS) {	
				printf("Max Thread Size Reached.. WAIT\n");
				continue;
			}
			printf("Spawning a thread for register request\n");
			num_threads++;
			pthread_create(&thread, NULL, registry, (void *)data);
		} else if (strstr(buffer, "deregister_file")) {
			if (num_threads == MAX_NUM_THREADS) {	
				printf("Max Thread Size Reached.. WAIT\n");
				continue;
			}
			printf("Spawning a thread for deregister request\n");
			num_threads++;
			pthread_create(&thread, NULL, deregister_file, (void *)data);
		} else if (strstr(buffer, "deregister_peer")) {
			if (num_threads == MAX_NUM_THREADS) {	
				printf("Max Thread Size Reached.. WAIT\n");
				continue;
			}
			printf("Spawning a thread for deregister request\n");
			num_threads++;
			pthread_create(&thread, NULL, deregister, (void *)data);
		}
	}
}
