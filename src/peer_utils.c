#include "peer_utils.h"

/* Start a server for myself */
void *server (void *data) {
	struct peer_data  *params = (struct peer_data *)data;
	int server_fd, client_fd, num_bytes = 0, flag;
	char buff[MAX_MSG_SIZE];
	struct sockaddr_in serv_addr;
	struct sockaddr_storage server_str;
	socklen_t server_addr_size;
	FILE *fp;

	/* Step 1 : Create a socket and get the fd */
	server_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		printf("Couldnt create the socket \n");
		goto exit;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(params->my_port));
	serv_addr.sin_addr.s_addr = inet_addr(params->my_ip);
	memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));

	/* Step 2 : Bind to the fd */
	flag = bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (flag == -1) {
		printf("Couldnt bind to the socket \n");
		goto exit;
	}

	/* Step 3 : Start listening to the connection */
	flag = listen(server_fd, MAX_CONN_LISTEN);

	if (flag != 0) {
		printf("Error listening to the connection\n");
		goto exit;
	}

	server_addr_size = sizeof(server_str);
	while(1) {
		/* Start the while loop for accepting file transfer requests */
		client_fd = accept(server_fd, (struct sockaddr *) &server_str, &server_addr_size);
		char *buffer_new = malloc(MAX_MSG_SIZE);
		recv (client_fd, buffer_new, MAX_MSG_SIZE, 0);
		printf("Received request for file %s\n", buffer_new);

		fp = fopen(buffer_new,"r+");

		if(fp == NULL)
		{
			printf("Could not open the file\n");
			goto exit;
		}

		while(1)
		{
			num_bytes = fread(buff, 1, MAX_MSG_SIZE, fp);

			if(num_bytes > 0)
			{
				printf("Sending bytes %d\n", num_bytes);
				write(client_fd, buff, num_bytes);
			}

			if (num_bytes < 1024) // Usually when there is error in reading or data is done
			{
				if (feof(fp))
					printf("End of file\n");
				if (ferror(fp))
					printf("Error reading\n");
				break;
			}
		}
	}
exit:
	if (server_fd > 0) close(server_fd);
	if (fp != NULL) fclose(fp);
	return 0;
}

void *register_peer (void *data) {
	struct peer_data  *params = (struct peer_data *)data;
	int server_fd;
	char *buffer_send = malloc(MAX_MSG_SIZE);
	char *buffer_recv = malloc(MAX_MSG_SIZE);
	struct sockaddr_in serv_addr;
	socklen_t serv_server_server_addr_size;
	int flag;

	printf("Connecting to the server\n");
	/* Step 1: Create the socket */
	server_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		printf("Error creating the socket\n");
		goto exit;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(params->server_port));
	serv_addr.sin_addr.s_addr = inet_addr(params->server_ip);
	memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));
	serv_server_server_addr_size =  sizeof(serv_addr);

	sprintf(buffer_send, "register:%s:%d:%s", params->my_ip, atoi(params->my_port), params->file_name);
	/* Step 1: Connect to the server */
	printf("Sending Registration Request %s\n", buffer_send);
	flag = connect(server_fd, (struct sockaddr *)&serv_addr, serv_server_server_addr_size);
	if (flag == -1)  {
		printf("Couldnt connect the server socket\n");
		goto exit;
	}
	send(server_fd, buffer_send, strlen(buffer_send),0);
	/* Check the response */
	recv(server_fd, buffer_recv, 1024,0);
	printf("Registered file %s\n",buffer_recv);
exit:
	if (server_fd > 0) close(server_fd);
	if (buffer_send > 0) free(buffer_send);
	if (buffer_recv > 0) free(buffer_recv);
	return 0;
}

/* Function to lookup and obtain a file from peer */
void *obtain (void *data) {

	struct peer_data  *params = (struct peer_data *)data;
	int sockfd, sockfd_p, n, n_bytes, rc;
	char *buffer_send = malloc(MAX_MSG_SIZE);
	char *buffer_recv = malloc(MAX_MSG_SIZE);
	struct sockaddr_in serv_addr;
	socklen_t addr_size;
	FILE *fp;

	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error creating the socket\n");
		goto exit;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(params->server_port));
	serv_addr.sin_addr.s_addr = inet_addr(params->server_ip);
	addr_size =  sizeof(serv_addr);

	sprintf(buffer_send, "lookup:%s", params->file_name);
	printf("Sending Lookup Request %s\n", buffer_send);

	rc = connect(sockfd, (struct sockaddr *)&serv_addr, addr_size);
	if (rc == -1) {
		printf("Couldn't connect to the server\n");
		goto exit;
	}

	send(sockfd, buffer_send, strlen(buffer_send),0);
	recv(sockfd, buffer_recv, 1024,0);

	if (!strcmp(buffer_recv,"none")) {
		printf("No such file found\n");
		goto exit;
	}

	if (buffer_recv == NULL) {
		printf("No peers have the file\n");
		goto exit;
	}

	printf("Following peers have the file\n");
	printf("%s", buffer_recv);
	close(sockfd);
	sockfd = -1;

	/* Found the peer, now retrieve the file */
	char *temp, *ip_addr, *port;
	temp = strtok(buffer_recv, ":");
	ip_addr = malloc(strlen(temp));
	strcpy(ip_addr, temp);
	if (ip_addr == NULL) {
		printf("Invalid peer ip\n");	
		goto exit;
	}
	ip_addr[strlen(temp)] = '\0';

	temp = strtok(NULL,":");
	port = malloc(strlen(temp));
	strcpy(port, temp);
	if (port == NULL) {
		printf("Invalid peer port\n");
		goto exit;
	}

	struct sockaddr_in serv_file_p;
	socklen_t addr_size_p;
	sockfd_p = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd_p < 0) {
		printf("Error creating the socket to peer\n");
		goto exit;
	}

	serv_file_p.sin_family = AF_INET;
	serv_file_p.sin_port = htons(atoi(port));
	serv_file_p.sin_addr.s_addr = inet_addr(ip_addr);
	addr_size =  sizeof(serv_addr);

	rc = connect(sockfd_p, (struct sockaddr *)&serv_file_p, addr_size);
	if (rc == -1) {
		printf("\nError connecting to peer\n");
		goto exit;
	}

	send(sockfd_p, params->file_name, strlen(params->file_name), 0);

	fp = fopen(params->file_name,"ab"); 
	if (fp == NULL)
	{
		printf("Could not open the file\n");
		goto exit;
	}

	while((n_bytes = read(sockfd_p, buffer_recv, 1024)) > 0)
	{
		printf("Bytes received %d\n",n_bytes);    
		fwrite(buffer_recv, 1, n_bytes, fp);
	}

	if(n_bytes < 0)
	{
		printf("\n Nothing received or is over \n");
	}
exit:
	if (fp != NULL) fclose(fp);
	if (sockfd > 0) close(sockfd);
	if (sockfd_p > 0) close(sockfd_p);
	if (buffer_send) free(buffer_send);
	if (buffer_recv) free(buffer_recv);
	if (port) free (port);
	if (ip_addr) free(ip_addr);
	return 0;
}
