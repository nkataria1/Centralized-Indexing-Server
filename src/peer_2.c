#include "peer_utils.h"

/* Usage peer <file> <server_ip> <server_port> */
int main (int argc, char *argv[]) {
	struct peer_data *data;
	data = (struct peer_data *)malloc(sizeof(struct peer_data));
	data->my_ip = "127.0.0.1";
	data->my_port = "9001";
	data->file_name = "sample_file.txt1 sample_file.txt2";
	data->server_ip = argv[1];
	data->server_port = argv[2];
	printf("data port %d", atoi(data->server_port));
	pthread_t thread;
	pthread_create(&thread, NULL, register_peer, (void *)data);
	pthread_create(&thread, NULL, server, (void *)data);
	pthread_join(thread, NULL);
	return 0;
}
