#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>


void hexdump(int sd, struct sockaddr *sin, int sender_len, char *buffer, int length) {
    int count = 0;
    int done = 0;
    char buf[1024 * 80];
    buf[0] = '\0';
    //printf("Test: %.2x\n", buffer[0]);
    // Looping over lines
    while(done == 0) {
        sprintf(buf, "%s%.6x : ", buf, count);
        for(int i = 0; i<16; i++) {
            if(done == 0) {
                sprintf(buf, "%s%.2x ", buf, (unsigned char) buffer[count]);
            }
            else {
                sprintf(buf, "%s   ", buf);
            }
            if(count == length) {
                done = 1;
            }
            count++;
        }
        count = count - 16;
        sprintf(buf, "%s  ", buf);
        for(int i = 0; i<16; i++) {
            if(count == length) {
                done = 2;
            }
            if(buffer[count] >= 0x20 && buffer[count] <=0x7e) {
                sprintf(buf, "%s%c", buf, buffer[count]);
            }
            else {
                sprintf(buf, "%s.", buf);
            }
            if(done == 2) {
                break;
            }
            count++;
        }
        sprintf(buf, "%s\n", buf);


//        write(socketfd, buf, sizeof(buf));
    }
    int send_stat = sendto(sd, buf, strlen(buf), 0, sin,
                           sender_len);
    if (send_stat < 0) {
        perror("write() to socket Error in hd\n");;
        exit(-1);
    }
}

//void hexdump(FILE *buf, char *buffer, int length) {
//    int count = 0;
//    int done = 0;
//    //printf("Test: %.2x\n", buffer[0]);
//    // Looping over lines
//    while(done == 0) {
//        fprintf(buf, "%.6x : ", count);
//        for(int i = 0; i<16; i++) {
//            if(done == 0) {
//                fprintf(buf, "%.2x ", (unsigned char) buffer[count]);
//            }
//            else {
//                fprintf(buf, "   ");
//            }
//            if(count == length) {
//                done = 1;
//            }
//            count++;
//        }
//        count = count - 16;
//        fprintf(buf, "  ");
//        for(int i = 0; i<16; i++) {
//            if(count == length) {
//                done = 2;
//            }
//            if(buffer[count] >= 0x20 && buffer[count] <=0x7e) {
//                fprintf(buf, "%c", buffer[count]);
//            }
//            else {
//                fprintf(buf, ".");
//            }
//            if(done == 2) {
//                break;
//            }
//            count++;
//        }
//        fprintf(buf, "\n");
//    }
//}