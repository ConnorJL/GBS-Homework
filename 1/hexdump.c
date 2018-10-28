#include <stdio.h>

void hexdump(FILE *output, char *buffer, int length) {
    int count = 0;
    int done = 0;
    //printf("Test: %.2x\n", buffer[0]);
    // Looping over lines
    while(done == 0) {
        fprintf(output, "%.6x : ", count);
        for(int i = 0; i<16; i++) {
            if(done == 0) {
                fprintf(output, "%.2x ", (unsigned char) buffer[count]);
            }
            else {
                fprintf(output, "   ");
            }
            if(count == length) {
                done = 1;
            }
            count++;
        }
        count = count - 16;
        fprintf(output, "  ");
        for(int i = 0; i<16; i++) {
            if(count == length) {
                done = 2;
            }
            if(buffer[count] >= 0x20 && buffer[count] <=0x7e) {
                fprintf(output, "%c", buffer[count]);
            }
            else {
                fprintf(output, ".");
            }
            if(done == 2) {
                break;
            }
            count++;
        }
        fprintf(output, "\n");
    }
}