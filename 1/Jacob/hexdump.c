/*
 * hexdump.c
 *
 *  Created on: 27.10.2018
 *      Author: jobrm
 */

#include <unistd.h>
#include <stdio.h>

void hexdump(FILE *output, char *buffer, int length){

	//Iterate over buffer
	for(int i = 0; i <= length; i += 16){

		//write buffer-offset to *output
		fprintf(output, "%06d : ", (i/16*10));

		//write hexvalues to *output
		for(int k = 0; k < 16 && i + k <= length; k++){
			if(buffer[i+k] != '\0')
				fprintf(output, "%02x ", (unsigned char) buffer[i+k]);
			else{
				fprintf(output, "00 ");
				k++;
				for(; k < 16; k++)
					fprintf(output, "   ");
				break;
			}
		}
		fprintf(output, "  ");
		//write printable chars to *output
		for(int k = 0; k < 16 && i + k <= length; k++){
			if(buffer[i+k] >= 32 && buffer[i+k] <= 126)
				fprintf(output, "%c", buffer[i+k]);
			else
				fprintf(output, ".");
		}
		//End of line
		fprintf(output, "\n");
	}
	//End of dump
	fprintf(output, "\n");
}
