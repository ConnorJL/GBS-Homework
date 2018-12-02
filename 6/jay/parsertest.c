/*
 * parsertest.c
 *
 *  Created on: 01.12.2018
 *      Author: jobrm
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "list.h"
#define maxSize 1024

list_t *parseUserInput(char *userInput, list_t *userInputElements, char *envp[]);

void print_string(char *data) {
	printf("%s\n", data);
}

int main(int argc, char *argv[], char *envp[]) {

	char *userInput = malloc(maxSize * sizeof(char));
//	char *userInput = "gg\n"; //DEBUG TESTER
	list_t *userInputElements = list_init();

	while (1) {

		//Prompt User to input command
		fprintf(stdout, "$ ");
		if (fgets(userInput, 1024, stdin) == NULL)
			return -1;

		//Terminate Prompt
		if (strcmp(userInput, "exit\n") == 0){
			printf("1:%s\n", "exit");
			break;
		}

		parseUserInput(userInput, userInputElements, envp);
		list_print(userInputElements, print_string);
		list_finit(userInputElements);
		userInputElements = list_init();
	}

	free(userInput);
	list_finit(userInputElements);
	return 1;

}
