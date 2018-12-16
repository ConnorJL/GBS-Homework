#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <poll.h>
#include <errno.h>

#include "list.h"


// TODO: Per connection timeouts, list stuff
int main(int argc, char const *argv[]) {
    char buffer[2048];
    int servertimeout = 10;
    int timeout = 5;
    int stop = 0;
    int ret;
    int nfds = 1;
    int current_size;
    int new_sd = -1;
    int len;
    int on = 1;
    int count;
    int running = 1;
    int close_connection;

    list_t *li;
	struct list_elem *li_el;
	if ((li = list_init()) == NULL) {
		perror("Cannot allocate memory");
		exit(-1);
	}

    list_append(li, servertimeout);
    li_el = li->first;

    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
    ioctl(server_socket, FIONBIO, (char *) on);

    struct sockaddr_in server_adress;
    server_adress.sin_family = AF_INET;
    server_adress.sin_port = htons(44445);
    server_adress.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(server_socket, (struct sockaddr *) &server_adress, sizeof(server_adress));

    listen(server_socket, 10);


    struct pollfd fds[11];
    memset(fds, 0, sizeof(fds)); // Set all to NULL
    fds[0].fd = server_socket; // Initialize our main socket
    fds[0].events = POLLIN;

    while(running) {
        // Check if something happened
        ret = poll(fds, nfds, 2000);

        if(ret == 0) {
        li_el = li->first;
        count = -1;
            while(li_el != NULL){
                li_el->data = (li_el->data) - 1;

                if((li_el->data) == 0){
                    if(li_el == li->first){
                        printf("Server Timeout!\n");
                        stop = 1;
                        break;
                    } else {
                        list_remove(li, li_el);
                        close(fds[count].fd);
                        fds[count].fd = -1;
                        printf("Client %i Timeout\n", count);
                    }
                }
            count++;
            li_el = li_el->next;
            }

            if(stop == 1){
                break;
            }
            
        } else {
            current_size = nfds;
            li_el = li->first;
            for(int i=0; i<current_size; i++) {
                li_el = li_el->next;

                if(fds[i].revents == 0) {
                // Nothing happened at this connection
                continue;
                }

                if(fds[i].fd == server_socket) {
                // We're accepting new connections
                do {
                    int new_sd = accept(server_socket, NULL, NULL);
                    list_append(li, timeout);
                    if(new_sd < 0) {
                        if(errno == EWOULDBLOCK) {
                            // We accepted all connections, move on
                            break;
                        }
                        else {
                            perror("Error in accept!");
                            running = 0;
                            break;
                        }
                    }
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                    } while(new_sd != -1);
                }

                else {
                    close_connection = 0;
                    while(1) {
                        ret = recv(fds[i].fd, buffer, sizeof(buffer), 0);

                        if(ret < 0) {
                            if(errno == EWOULDBLOCK) {
                                // Can't read without blocking
                                break;
                            } else {
                                // Error in the connection
                                perror("Error from recv!");
                                close_connection = 1;
                                break;
                            }

                        }

                        if(ret == 0) {
                            // Connection closed by client
                            perror("Connection closed by client!");
                            close_connection = 1;
                            break;
                        }
                        
                        //Data received
                        len = ret;
                        // Echo stuff back
                        ret = send(fds[i].fd, buffer, len, 0);

                        if(ret < 0) {
                            perror("Error from send!");
                            close_connection = 1;
                            break;
                        }
                    }

                    // Connection wants to be closed, close it and readjust list afterwards
                    if(close_connection) {
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        list_remove(li, li_el);
                    }

                    li_el->data = timeout;
                    li->first->data = servertimeout;
                }
            }
        }
    }

    // Clean up files
    for(int i=0; i < nfds; i++) {
        if(fds[i].fd >= 0) {
            close(fds[i].fd);
        }
    }
    list_finit(li);

    return 0;
}
