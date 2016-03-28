all : make_my_files

make_my_files : server peer_utils peer peer_2

peer_utils : peer_utils.c
	gcc peer_utils.c -c -o peer_utils.o

peer : peer.c
	gcc peer.c peer_utils.o -o peer

peer_2 : peer_2.c
	gcc peer_2.c peer_utils.o -o peer_2

server : server.c
	gcc server.c -o server
