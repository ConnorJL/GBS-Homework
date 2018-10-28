#include<stdio.h>
#include <stdlib.h>
#include<string.h>

void hexdump (FILE *output, char *buffer, int length) {
	int off = 0;
	while(1) {
		if (strlen(buffer) + 1 > off) {
			fprintf(output, "%06x :", off);
			char inhex[length * 3];
			for (int i = 0; i < length; i++) {
				if (strlen(buffer) + 1 > off + i) {
					int tmp = buffer[off + i];
					if (tmp < 0) {
						tmp = tmp & 0xFF;
					}
					//printf(" %d", tmp);
					fprintf(output, " %02x", tmp);
				} else {
					fprintf(output, "   ");
				}
			}
			fprintf(output, "   ");
			for (int i = 0; i < length; i++) {
				if (strlen(buffer) + 1 > off + i) {
					if ((int)buffer[off + i] < 32 || (int)buffer[off + i] > 126) {
						fprintf(output, ".");
					} else {
						fprintf(output, "%c", buffer[off + i]);
					}
				} else {
					//fprintf(output, " ");
				}
			}
			fprintf(output, "\n");
			off = off + length;
			}
		else {
			fprintf(output, "\n");
			break;
			}
		}
}