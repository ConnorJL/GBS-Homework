/*
 * hd.c
 *
 *  Created on: 27.10.2018
 *      Author: jobrm
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>

void hexdump(FILE *output, char *buffer, int length);

int main(int argc, char **argv) {

	char tempc;
	FILE *output = fopen("hexdump.txt", "w");
	FILE *output2;

	//Generate Hexdump for each argv
	for (int i = 1; i < argc; i++) {
		hexdump(output, argv[i], strlen(argv[i]));
	}
	//save Hexdump to hexdump.txt
	fclose(output);

	//Print Hexdump to console
	output2 = fopen("hexdump.txt", "r");

	tempc = fgetc(output2);
	while(tempc != EOF){
		printf("%c", tempc);
		tempc = fgetc(output2);
	}
	fclose(output2);

	return 0;
}
