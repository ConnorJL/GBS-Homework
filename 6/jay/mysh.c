/*
 * mysh.c
 *
 *  Created on: 02.12.2018
 *      Author: jobrm
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "list.h"
#define maxSize 1024

list_t *parseUserInput(char *userInput);

int main(int argc, char *argv[], char *envp[]) {

	char *userInput = malloc(maxSize * sizeof(char));
//	char *userInput = "ls\n";
	list_t *userInputElements;
	list_t *processes = list_init();
	pid_t childPID;
	char *programName;

	while (1) {

		//Prompt User to input command
		fprintf(stdout, "$ ");
		if (fgets(userInput, maxSize, stdin) == NULL)
			return -1;

//Terminate Prompt
		if (strcmp(userInput, "exit\n") == 0) {
//			printf("1:%s\n", "exit");
			break;
		}

		userInputElements = parseUserInput(userInput);

		int size = list_size(userInputElements);
		char *resultArray[size + 1];
		resultArray[size] = NULL;
		list_to_array(userInputElements, resultArray);

		switch ((childPID = fork())) {
		case 0:		//child process
			programName = resultArray[0];
			//Is it a dynamic Path or a total Path to Programm
			if (strchr(programName, '/') == NULL) {
				//Parse dynamic Path by searching the environment
				char *allPaths = getenv("PATH");
				int sizeOfAllPaths = strlen(allPaths);
				char *totalPath = malloc(4 * sizeOfAllPaths);
				totalPath = strtok(allPaths, ":");
				strcat(totalPath, "/");
				strcat(totalPath, programName);
				while (totalPath != NULL) {
					execve(totalPath, resultArray, envp);
					totalPath = strtok(NULL, ":");
					strcat(totalPath, "/");
					strcat(totalPath, programName);
				}
			} else
				execve(programName, resultArray, envp);
			printf("-mysh: %s: command not found\n", programName);
			break;
		case -1:	//ERROR
			return -1;
			break;
		default:	//parent process
			list_append(processes, (char *) &childPID);
			break;
		}
	}

	//Wait for every child Process
	int wstatus;
	int numOfProcesses = list_size(processes);
	for (int i = 0; i <= numOfProcesses; i++) {
		if ((wait(&wstatus)) == -1)
			return -1;
	}

	list_finit(processes);

	return 1;
}

