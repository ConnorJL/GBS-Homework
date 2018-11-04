/*
 * shellsim.c
 *
 *  Created on: 03.11.2018
 *      Author: jobrm
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	char cmd[256];
	char *checkc;
	char checki;

	while(1){

		printf("bash: ");
		checkc = fgets(cmd, 256, stdin);

	    if(cmd[strlen(cmd) - 1] == '\n'){
	      cmd[strlen(cmd) - 1] = '\0';
	    }

		if(strcmp(checkc, "s") == 0 || strncmp(cmd, "exit", 4) == 0){
			break;
		}

		checki = system(cmd);

		if(checki == -1){
			break;
		}

	}
	return 1;
}
