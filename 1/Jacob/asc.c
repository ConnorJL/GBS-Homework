/*
 * asc.c
 *
 *  Created on: 26.10.2018
 *      Author: jobrm
 */

#include <stdio.h>

int main(int argc, char **argv) {

	//Column Descriptor
	printf("Oct\tDec\tHex\tChar\n");

	//Coding Table Loop
	for(int i = 0; i <= 127; i++){
		//evading unprintable chars
//		if(i<33){
//			printf("%1$03o\t%1$d\t%1$02x\t \n", i);
//		}else{
			printf("%1$03o\t%1$d\t%1$02x\t%1$c\n", i);
//		}

	}

	return 0;
}

