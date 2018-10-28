/*
 * hd.c
 *
 *  Created on: 27.10.2018
 *      Author: jobrm
 */

#include <stdio.h>
#include <string.h>

void hexdump(FILE *output, char *buffer, int length);

int main(int argc, char **argv) {

	//Generate Hexdump for each argv
	for (int i = 1; i < argc; i++) {
		hexdump(stdout, argv[i], strlen(argv[i]));
	}
	return 0;
}
