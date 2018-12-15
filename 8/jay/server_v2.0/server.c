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
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <errno.h>

#define PORT 44445
#define SIZE 1024
#define MAX_CONNECTIONS 10
#define SERVER_TIMEOUT 10000 	//10 seconds
#define CLIENT_TIMEOUT 5 	//5 seconds

#define handle_error(msg) \
		do { perror(msg); exit(-1); } while (0)

struct connection {
	int sd;
	struct sockaddr_in sin;
	struct timeval timeout;
};

void echoservice(int socketfd) {
	char input[SIZE];
	int sizeread;

	bzero(input, SIZE);

	sizeread = read(socketfd, input, sizeof(input));
	if (sizeread < 0)
		handle_error("read() from socket Error\n");
	if (sizeread >= 1024)
		input[1024] = '\0';

	input[sizeread] = '\0';

	if(write(socketfd, input, sizeof(input)) < 0)
		handle_error("write() to socket Error\n");;
}

int is_timedout(struct connection conn) {
	int check;
	struct timeval *current_time = malloc(sizeof(struct timeval *));
	struct timeval *time_difference = malloc(sizeof(struct timeval *));

	check = gettimeofday(current_time, NULL);
	if (check == -1)
		handle_error("gettimeofday() Error\n");

	timersub(current_time, &(conn.timeout), time_difference);

	check = time_difference->tv_sec >= CLIENT_TIMEOUT ? 1 : 0;

	free(current_time);
	free(time_difference);

	return check;
}

int main(int argc, char **argv) {

	int socketfd, remote_socketfd, check, counter;
	struct sockaddr_in server_addr, peer_addr;
	socklen_t peer_addr_size;
	struct pollfd pfds[MAX_CONNECTIONS];
	struct connection conn_logs[MAX_CONNECTIONS];
	int num_of_pfds, server_shutdown = 0, client_shutdown = 0;

	printf("launching echo server");

	socketfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (socketfd == -1)
		handle_error(" socket() Error\n");
	else
		printf(" success socket()\n");

	bzero(&server_addr, sizeof(server_addr)); //Clear

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	check = bind(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (check != 0) {
		close(socketfd);
		handle_error(" bind() Error\n");
	} else
		printf(" success bind()\n");

	check = listen(socketfd, 10);
	if (check != 0) {
		close(socketfd);
		handle_error(" listen() Error\n");
	} else
		printf(" success listen()\n");

	pfds[0].fd = socketfd;
	pfds[0].events = POLLIN;

	while (1) {

		peer_addr_size = sizeof(peer_addr);

		printf(" Starting poll()\n");
		check = poll(pfds, num_of_pfds, SERVER_TIMEOUT);
		if (check < 0) {
			printf("  poll() Error\n");
			break;
		} else if (check == 0) {
			printf("  poll() timed out, shutting down...\n");
			break;
		}

//		counter = check;
		for (int i = 0; i < MAX_CONNECTIONS; i++) {

			if (pfds[i].revents == 0) //nothing happed, skip
				continue;

			//Adding new connection
			if (pfds[i].fd == socketfd) {
				printf("   processing server socket access request\n");

				while (1) {
					remote_socketfd = accept(socketfd, NULL, NULL);	//(struct sockaddr *) &peer_addr,	&peer_addr_size);
					if (remote_socketfd < 0) {
						if (errno != EWOULDBLOCK) {
							perror("   accept() Error");
							server_shutdown = 1;
						}
						break;
					}
					gettimeofday(&conn_logs[num_of_pfds].timeout, NULL);
					pfds[num_of_pfds].fd = remote_socketfd;
					pfds[num_of_pfds].events = POLLIN;
					num_of_pfds++;

					printf("   added new remote socket\n");
				}
			} else { //processing existing connection
				printf("   processing remote socket access request\n");
				client_shutdown = 0;

				if(is_timedout(conn_logs[i])){
					client_shutdown = 1;
					//do client shutdown maybe here??
				}else
					echoservice(pfds[i].fd);



			}

		}

	}

	peer_addr_size = sizeof(peer_addr);

	remote_socketfd = accept(socketfd, (struct sockaddr *) &peer_addr,
			&peer_addr_size);
	if (remote_socketfd < 0)
		handle_error("accept() Error\n");
	else
		printf("success accept()\n");

	echoservice(remote_socketfd);

	close(socketfd);
	printf("success close()\n");

	return 1;
}

