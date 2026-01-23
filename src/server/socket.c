#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>




int srv_StartListen(){
	int server_fd, new_socket;
	ssize_t valread;
	struct sockaddr_in address;
	int opt = 1;
	socklen_t addrlen = sizeof(address);
	char buffer[1024] = {0x1};
	char* hello = "Hello from Server";

	if((server_fd= socket(AF_INET, SOCK_STREAM,0)) < 0){
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}

	return 1;
}
