#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>
#include <errno.h>

#include "list.h"


// TODO: Per connection timeouts, list stuff
int main(int argc, char const *argv[]) {
    char buffer[2048];
    int timeout = 100000;
    int ret;
    int nfds = 1;
    int current_size;
    int new_sd = -1;
    int len;
    int on = 1;
    int running = 1;
    int close_connection;


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
        ret = poll(fds, nfds, timeout);

        if(ret == 0) {
            printf("Timeout!\n");
            break;
        }

        current_size = nfds;
        for(int i=0; i<current_size; i++) {
            if(fds[i].revents == 0) {
                // Nothing happened at this connection
                continue;
            }

            if(fds[i].fd == server_socket) {
                // We're accepting new connections
                do {
                    int new_sd = accept(server_socket, NULL, NULL);
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
                        }
                        else {
                            // Error in the connection
                            perror("Error from recv!");
                            close_connection = 1;
                            break;
                        }
                        
                    }

                    if(ret == 0) {
                        // Connection closed by client
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
                    compress_connections = 1;
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

    return 0;
}
