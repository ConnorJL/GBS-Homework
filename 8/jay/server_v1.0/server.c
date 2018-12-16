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
#define PORT2 44446
#define SIZE 1024
#define MAX_CONNECTIONS 10
#define SERVER_TIMEOUT 10000 * 20 	//10 seconds
#define CLIENT_TIMEOUT 5 	//5 seconds

#define handle_error(msg) \
		do { perror(msg); exit(-1); } while (0)

struct connection {
	int sd;
	struct sockaddr_in sin;
	struct timeval timeout;
};

void echoservice(struct connection conn) {
	if (conn.sd < 0)
		return;

	char input[SIZE];
	int sizeread;
	socklen_t sender_len = sizeof(conn.sin);

	bzero(input, SIZE);

	sizeread = recvfrom(conn.sd, input, sizeof(input), 0,
			(struct sockaddr *) &conn.sin, &sender_len);
	if (sizeread < 0)
		handle_error("read() from socket Error\n");
	if (sizeread >= 1024) {
		input[1023] = '\0';
		sizeread = 1023;
	}

	input[sizeread] = '\0';

	sizeread = sendto(conn.sd, input, sizeread, 0, (struct sockaddr *) &conn.sin,
			sender_len);
	if (sizeread < 0)
		handle_error("write() to socket Error\n");;
}

void hexdump(int sd, struct sockaddr *sin, int sender_len, char *buffer, int length);

void hexdumpservice(struct connection conn) {
    if (conn.sd < 0)
        return;

    char input[SIZE];
    int sizeread;
    socklen_t sender_len = sizeof(conn.sin);

    bzero(input, SIZE);

    sizeread = recvfrom(conn.sd, input, sizeof(input), 0,
                        (struct sockaddr *) &conn.sin, &sender_len);
    if (sizeread < 0)
        handle_error("read() from socket Error\n");
    if (sizeread >= 1024) {
        input[1023] = '\0';
        sizeread = 1023;
    }

    input[sizeread] = '\0';

    hexdump(conn.sd, (struct sockaddr *) &conn.sin, sender_len, input, sizeread);

//    sizeread = sendto(conn.sd, input, sizeread, 0, (struct sockaddr *) &conn.sin,
//                      sender_len);
//    if (sizeread < 0)
//        handle_error("write() to socket Error\n");;
}

int is_timedout(struct connection conn) {
	if (conn.timeout.tv_sec == 0)
		return -1;

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

	int socketfd, remote_socketfd, check;
	struct sockaddr_in server_addr;
	struct pollfd pfds[MAX_CONNECTIONS];
	struct connection conn_logs[MAX_CONNECTIONS];
	int server_shutdown = 0, client_shutdown = 0;
	int num_of_pfds = 1, num_of_active_sockets;

	printf("launching echo server\n");

	socketfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	check = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, // | SO_LINGER,
			(char *) &num_of_pfds, sizeof(num_of_pfds));
	if (socketfd == -1 || check == -1)
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

	//set listening fd
	pfds[0].fd = socketfd;
	pfds[0].events = POLLIN;

	//Initialize poll fd's
	for (int i = 1; i < MAX_CONNECTIONS; i++)
		pfds[i].fd = -1;

	while (1) {

		if (server_shutdown)
			break;

		printf(" Starting poll()\n");
		check = poll(pfds, num_of_pfds, SERVER_TIMEOUT);
		if (check < 0) {
			printf("  poll() Error\n");
			break;
		} else if (check == 0) {
			printf("  poll() timed out, shutting down...\n");
			break;
		}

		printf(" Managing active sockets\n");
		num_of_active_sockets = num_of_pfds;
		for (int i = 0; i < num_of_active_sockets; i++) {

			if (pfds[i].revents == 0)
				if (pfds[i].fd != socketfd && is_timedout(conn_logs[i])) {
					printf("   a client timed out\n");
					client_shutdown = 1;
					close(pfds[i].fd);
					pfds[i].fd = -1;
				} else
					continue; //nothing happed, skip
			else { //Adding new connection
				if (pfds[i].fd == socketfd) {
					printf("   processing server socket access request\n");

					while (1) {
						remote_socketfd = accept(socketfd, NULL, NULL);	//(struct sockaddr *) &peer_addr,	&peer_addr_size);
						if (remote_socketfd < 0) {
							if (errno != EWOULDBLOCK) {
								perror("   accept() Error, shutting down...\n");
								server_shutdown = 1;
							}
							break;
						}
						gettimeofday(&conn_logs[num_of_pfds].timeout, NULL);
						conn_logs[num_of_pfds].sd = remote_socketfd;
						pfds[num_of_pfds].fd = remote_socketfd;
						pfds[num_of_pfds].events = POLLIN;
						num_of_pfds++;

						printf("   added new remote socket\n");
					}
				} else { //do echo service
					printf("   processing remote socket access request\n");
					gettimeofday(&conn_logs[i].timeout, NULL); //reset timeout
					hexdumpservice(conn_logs[i]);
				}
			}
		}

		//move active clients to start of the poll file descriptors
		if (client_shutdown) {
			client_shutdown = 0;
			for (int i = 0; i < num_of_pfds; i++) {
				if (pfds[i].fd == -1) {
					for (int k = i; k < num_of_pfds; k++) {
						pfds[k] = pfds[k + 1];
						conn_logs[k] = conn_logs[k + 1];
					}
					i--;
					num_of_pfds--;
				}
			}
		}
	}

	printf(" closing open file descriptors\n");
	for (int i = 0; i < num_of_pfds; i++)
		if (pfds[i].fd >= 0) {
			check = close(pfds[i].fd);
			if (check < 0)
				handle_error("close() Error\n");
		}

	printf("success close()\n");

	return 1;
}

