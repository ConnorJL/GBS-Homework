/*
 * rotup.c
 *
 *  Created on: 26.10.2018
 *      Author: jobrm
 */

#include <unistd.h>
#include <stdio.h>
#define stringsize 256

int main(int argc, char **argv) {

	char userinput[stringsize];
	int count = 0;

	read(0, userinput, stringsize);

	write(1, "Hallo: ", 7);

	//Count user input chars
	while(userinput[count] != '\n' && userinput[count] != '\0'){
		count++;
	}

	write(1, userinput, count);
	write(1, " -- ", 4);

	//Make all chars UPPERCASE
	for(int i = 0; i < stringsize; i++){
		if(userinput[i] >= 'a' && userinput[i] <= 'z'){
			userinput[i] = userinput[i] - ('a' - 'A');
		}else if(userinput[i] < 'A' || userinput[i] > 'Z'){
			userinput[i] = '\0';
			break;
		}
	}

	//Apply ROT13-Shift
	count = 0;
	for(int i = 0; userinput[i] != '\0'; i++, count++){
		userinput[i] = (((userinput[i] - 65) + 13) % 26) + 65;
	}
	write(1, userinput, count);
	write(1, "\n", 1);
	return 0;
}

