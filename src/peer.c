#include "peer_utils.h"

/* Usage peer <file> <server_ip> <server_port> */
int main (int argc, char *argv[]) {
	struct peer_data *data;
	data = (struct peer_data *)malloc(sizeof(struct peer_data));
	data->file_name = argv[1];
	data->my_ip = "127.0.0.1";
	data->my_port = "9000";
	data->server_ip = argv[2];
	data->server_port = argv[3];
	pthread_t thread;
	pthread_create(&thread, NULL, obtain, (void *)data);
	pthread_join(thread, NULL);
	return 0;
}
