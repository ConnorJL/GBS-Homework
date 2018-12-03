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

list_t *parseUserInput(char *userInput);

void print_string(char *data) {
	printf("%s\n", data);
}

void print_list(list_t *list) {

	if (list == NULL || list->first == NULL)
		return ;

    struct list_elem *current = list->first;
    int i = 1;
    while (current != NULL) {
        printf("%i:%s\n", i, current->data);
        current = current->next;
        i++;
    }
}

int main(int argc, char *argv[], char *envp[]) {

	char *userInput = malloc(maxSize * sizeof(char));
//	char *userInput = "gg\n"; //DEBUG TESTER
	list_t *userInputElements;

	while (1) {

		//Prompt User to input command
		fprintf(stdout, "$ ");
		if (fgets(userInput, maxSize, stdin) == NULL)
			return -1;

		//Terminate Prompt
		if (strcmp(userInput, "exit\n") == 0){
			printf("1:%s\n", "exit");
			break;
		}

		userInputElements = parseUserInput(userInput);
		print_list(userInputElements);
		list_finit(userInputElements);
		userInputElements = list_init();
	}

	free(userInput);
	list_finit(userInputElements);
	return 1;

}
