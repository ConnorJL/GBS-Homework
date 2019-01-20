//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/ioctl.h>
//#include <sys/socket.h>
//#include <sys/poll.h>
//#include <ctype.h>
//#include <arpa/inet.h>
//#include <poll.h>
//#include <unistd.h>
//#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
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
#include <ctype.h>

#define MYPORT 44445

// TODO: USE POLL for all Connections at once with timeout 10 and for each conn with timeout 5

struct connection { int sd; struct sockaddr_in sin ; struct timeval timeout ; };

int main (int argc, char *argv[], char *envp[]) {
//    char                buffer [0x10000];
//    struct sockaddr_in  sin;
//    struct sockaddr_in  sender;
//    struct sockaddr_in6   sender;
//    struct sockaddr_in6   addr;
//    int                 sd;
//    int                 len, i, on = 1;
//    socklen_t           sender_len;


    int    len, rc, on = 1;
    int    listen_sd = -1, new_sd = -1;
    int    end_server = 0, compress_array = 0;
    int    close_conn;
    char   buffer[1024];
    struct sockaddr_in   addr;
    int    timeout;
    struct pollfd conns[10];
    struct pollfd fds[10];
    int    nfds = 1, current_size = 0, i, j;

    if (argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        exit(-1);
    }
    if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Connot create socket");
    }
    if (setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
                    (char *)&on, sizeof(on)) == -1) {
        perror("setsockopt() failed");
        close(listen_sd);
        exit(-1);
    }
    if (ioctl(listen_sd, FIONBIO, (char *)&on) == -1) {
        perror("ioctl() failed");
        close(listen_sd);
        exit(-1);
    }

//    sin.sin_family      = AF_INET;
//    printf("Setting PORT %d\n", MYPORT);
//    sin.sin_port        = htons(MYPORT);
//    sin.sin_addr.s_addr = INADDR_ANY;
//    if (bind(sd, (struct sockaddr *) &sin, sizeof (sin)) == -1) {
//        perror("Bind failed");
//        exit(-1);
//    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    memcpy(&addr.sin_addr, &in6addr_any, sizeof(struct in6_addr));
    addr.sin_port        = htons(MYPORT);
    printf("Set port %d.\ns", MYPORT);
    if (bind(listen_sd,
              (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Bind failed");
        exit(-1);
    }

    /*************************************************************/
    /* Set the listen back log                                   */
    /*************************************************************/
    rc = listen(listen_sd, 1024);
    if (rc < 0)
    {
        perror("listen() failed");
        close(listen_sd);
        exit(-1);
    }

    /*************************************************************/
    /* Initialize the pollfd structure                           */
    /*************************************************************/
    memset(fds, 0 , sizeof(fds));

    /*************************************************************/
    /* Set up the initial listening socket                        */
    /*************************************************************/
    fds[0].fd = listen_sd;
    fds[0].events = POLLIN;
    /*************************************************************/
    /* Initialize the timeout to 10s. If no                */
    /* activity after 10s this program will end.           */
    /* timeout value is based on milliseconds.                   */
    /*************************************************************/
    timeout = (10 * 1000);

    /*************************************************************/
    /* Loop waiting for incoming connects or for incoming data   */
    /* on any of the connected sockets.                          */
    /*************************************************************/
    do
    {
        /***********************************************************/
        /* Call poll() and wait 10s for it to complete.      */
        /***********************************************************/
        printf("Waiting on poll()...\n");
        rc = poll(fds, nfds, timeout);

        /***********************************************************/
        /* Check to see if the poll call failed.                   */
        /***********************************************************/
        if (rc < 0)
        {
            perror("  poll() failed");
            break;
        }

        /***********************************************************/
        /* Check to see if the 10s time out expired.          */
        /***********************************************************/
        if (rc == 0)
        {
            printf("  poll() timed out.  End program.\n");
            break;
        }

        // check for each sd if it has timed out
        current_size = nfds;
        for (int j = 0; j < 10; j++) {
            if(fds[j].fd != conns[j].fd) {
                if (fds[j].fd == -1) {
                    conns[j].fd = -1;
                } else {
                    conns[j].fd = fds[j].fd;
//                    int exists = 0;
//                    for (int jj = 0; jj < 10; jj++) {
//
//                    }
                }
            };
        }
        for (int j = 0; j < current_size; j++) {
            rc = poll(conns[j], nfds, timeout);
        }

        /***********************************************************/
        /* One or more descriptors are readable.  Need to          */
        /* determine which ones they are.                          */
        /***********************************************************/
        current_size = nfds;
        for (i = 0; i < current_size; i++)
        {
            /*********************************************************/
            /* Loop through to find the descriptors that returned    */
            /* POLLIN and determine whether it's the listening       */
            /* or the active connection.                             */
            /*********************************************************/
            if(fds[i].revents == 0)
                continue;

            /*********************************************************/
            /* If revents is not POLLIN, it's an unexpected result,  */
            /* log and end the server.                               */
            /*********************************************************/
            if(fds[i].revents != POLLIN)
            {
                printf("  Error! revents = %d\n", fds[i].revents);
                end_server = 1;
                break;

            }
            if (fds[i].fd == listen_sd)
            {
                /*******************************************************/
                /* Listening descriptor is readable.                   */
                /*******************************************************/
                printf("  Listening socket is readable\n");

                /*******************************************************/
                /* Accept all incoming connections that are            */
                /* queued up on the listening socket before we         */
                /* loop back and call poll again.                      */
                /*******************************************************/
                do
                {
                    /*****************************************************/
                    /* Accept each incoming connection. If               */
                    /* accept fails with EWOULDBLOCK, then we            */
                    /* have accepted all of them. Any other              */
                    /* failure on accept will cause us to end the        */
                    /* server.                                           */
                    /*****************************************************/
                    new_sd = accept(listen_sd, NULL, NULL);
                    if (new_sd < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("  accept() failed");
                            end_server = 1;
                        }
                        break;
                    }

                    /*****************************************************/
                    /* Add the new incoming connection to the            */
                    /* pollfd structure                                  */
                    /*****************************************************/
                    printf("  New incoming connection - %d\n", new_sd);
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    nfds++;

                    /*****************************************************/
                    /* Loop back up and accept another incoming          */
                    /* connection                                        */
                    /*****************************************************/
                } while (new_sd != -1);
            }

                /*********************************************************/
                /* This is not the listening socket, therefore an        */
                /* existing connection must be readable                  */
                /*********************************************************/

            else
            {
                printf("  Descriptor %d is readable\n", fds[i].fd);
                close_conn = 0;
                /*******************************************************/
                /* Receive all incoming data on this socket            */
                /* before we loop back and call poll again.            */
                /*******************************************************/

                do
                {
                    /*****************************************************/
                    /* Receive data on this connection until the         */
                    /* recv fails with EWOULDBLOCK. If any other         */
                    /* failure occurs, we will close the                 */
                    /* connection.                                       */
                    /*****************************************************/
                    rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                    if (rc < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("  recv() failed");
                            close_conn = 1;
                        }
                        break;
                    }

                    /*****************************************************/
                    /* Check to see if the connection has been           */
                    /* closed by the client                              */
                    /*****************************************************/
                    if (rc == 0)
                    {
                        printf("  Connection closed\n");
                        close_conn = 1;
                        break;
                    }

                    /*****************************************************/
                    /* Data was received                                 */
                    /*****************************************************/
                    len = rc;
                    printf("  %d bytes received\n", len);

                    /*****************************************************/
                    /* Echo the data back to the client                  */
                    /*****************************************************/
                    rc = send(fds[i].fd, buffer, len, 0);
                    if (rc < 0)
                    {
                        perror("  send() failed");
                        close_conn = 1;
                        break;
                    }

                } while(1);

                /*******************************************************/
                /* If the close_conn flag was turned on, we need       */
                /* to clean up this active connection. This            */
                /* clean up process includes removing the              */
                /* descriptor.                                         */
                /*******************************************************/
                if (close_conn)
                {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = 1;
                }


            }  /* End of existing connection is readable             */
        } /* End of loop through pollable descriptors              */

        /***********************************************************/
        /* If the compress_array flag was turned on, we need       */
        /* to squeeze together the array and decrement the number  */
        /* of file descriptors. We do not need to move back the    */
        /* events and revents fields because the events will always*/
        /* be POLLIN in this case, and revents is output.          */
        /***********************************************************/
        if (compress_array)
        {
            compress_array = 0;
            for (i = 0; i < nfds; i++)
            {
                if (fds[i].fd == -1)
                {
                    for(j = i; j < nfds; j++)
                    {
                        fds[j].fd = fds[j+1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
        }

    } while (end_server == 0); /* End of serving running.    */

    /*************************************************************/
    /* Clean up all of the sockets that are open                 */
    /*************************************************************/
    for (i = 0; i < nfds; i++)
    {
        if(fds[i].fd >= 0)
            close(fds[i].fd);
    }
}




//    while (1) {
//
//        sender_len = sizeof(sender);
//        if ((len = recvfrom(sd, buffer, sizeof(buffer), 0,
//                (struct sockaddr *) &sender, &sender_len)) == -1) {
//            perror("Receiving from socket failed");
//            exit(-1);
//        }
//        printf("Received packet from %s:%d size %d\n",
//               inet_ntoa (sender.sin_addr), ntohs(sender.sin_port), len);
//        if (len > 0) {
//            for (i = 0; i < len; i++) {
//                if (isalpha(buffer[i]))
//                    buffer[i] ^= 0x20;
//                else if (isdigit(buffer[i]))
//                    buffer[i] = (9 - (buffer[i] - '0')) + '0';
//            }
//            if (sendto(sd, buffer, len, 0, (struct sockaddr *) &sender, sender_len) <= 0) {
//                perror("Failed to send response back to client");
//                continue;
//            }
//        }
//    }
//    exit(0);
//}