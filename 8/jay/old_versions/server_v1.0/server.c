/*
 * server.c
 *
 *  Created on: 13.12.2018
 *      Author: jobrm
 */

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 44445
#define SIZE 1024

#define handle_error(msg) \
		do { perror(msg); exit(-1); } while (0)

void echoservice(int socketfd){
	char input[SIZE];

	while(1){
		bzero(input, SIZE);

		read(socketfd, input, sizeof(input));

		write(socketfd, input, sizeof(input));
	}
}

int main(int argc, char **argv) {

	int socketfd, connectionfd, check;
	struct sockaddr_in my_addr, peer_addr;
	socklen_t peer_addr_size;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd == -1)
		handle_error("socket() Error\n");

	bzero(&my_addr, sizeof(my_addr));

	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(PORT);

	check = bind(socketfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
	if (check != 0)
		handle_error("bind() Error\n");

	check = listen(socketfd, 5);
	if (check != 0)
		handle_error("listen() Error\n");

	peer_addr_size = sizeof(peer_addr);

	connectionfd = accept(socketfd, (struct sockaddr *)&peer_addr, &peer_addr_size);
	if(connectionfd < 0)
		handle_error("accept() Error\n");

	echoservice(connectionfd);

	return 1;
}







