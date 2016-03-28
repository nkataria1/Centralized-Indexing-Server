# Centralized-Indexing-Server
Project goal is to implement a central indexing server that indexes the contents of all of the peers that register with it and also provides search facility to peers. A peer is both a client and a server.

This section describes how to go about using this client/server program:

1.	Goto src folder and run make
2.	After step 1, there would be following binaries/object files generated:

server, peer, peer_2, peer_utils.o

server : The indexing server 
peer : This peer will sending a lookup request to peer_2.
peer_2: This peer will registering its files the server and will start as a server to serve the files to peer.
peer_utils.o : Utilities used by peer and peer_2. This is the file defining “obtain” , “lookup“ and “register” functions. 

3.	Start the server
./server <ip> <port>

NIKATARI-M-X1R5: nikatari$ ./server 127.0.0.1 7891 
Received message from client register:127.0.0.1:9001:sample_file.txt1 sample_file.txt2
Spawning a thread for register request
New peer registered 4
Registring file sample_file.txt1
Registring file sample_file.txt2
Registered peer 127.0.0.1:9001
Received message from client lookup:sample_file.txt
Spawning a thread for search request
File name is sample_file.txt Length 15
Checking peer 127.0.0.1:9001
Number of bytes written 29

4.	Start the peer 2
./peer <server_ip> <server_port> <file_list>
 
NIKATARI-M-X1R5:Project_Final nikatari$ ./peer_2 127.0.0.1 7891 sample_file.txt
data port 7891
Connecting to the server
Sending Registration Request
register:127.0.0.1:9001:sample_file.txt1 sample_file.txt2 Registered file Registered files 127.0.0.1:9001_file.txt2 Received request for file sample_file.txt 
Sending bytes 1024
Sending bytes 1024
Sending bytes 1024
Sending bytes 1024
Sending bytes 1024
Sending bytes 1024
Sending bytes 1024
Sending bytes 1024
Sending bytes 492
End of file

5.	Start peer
./peer <file> <server_ip> <server_port>

NIKATARI-M-X1R5:Project_Final nikatari$ ./peer sample_file.txt 127.0.0.1 7891 
Sending Lookup Request lookup:sample_file.txt
Following peers have the file
127.0.0.1:9001
127.0.0.1:9001Bytes received 1024
Bytes received 1024
Bytes received 1024
Bytes received 1024
Bytes received 1024
Bytes received 1024
Bytes received 1024
Bytes received 1024
Bytes received 492

