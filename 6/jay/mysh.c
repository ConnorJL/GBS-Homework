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

list_t *parsePath() {
	list_t *pathsList = list_init();
	char *paths = strdup(getenv("PATH"));
	char *path = strtok(paths, ":");

	while (path != NULL) {
		list_append(pathsList, strdup(path));
		path = strtok(NULL, ":");
	}
	free(paths);
	return pathsList;
}

int main(int argc, char *argv[], char *envp[]) {

	char *userInput = malloc(maxSize * sizeof(char));
//	char *userInput = "dd232 ddxyz\n";
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
			break;
		}
		userInputElements = parseUserInput(userInput);
		childPID = fork();
//		childPID = 0;
		switch (childPID) {
		case 0://child process
			;
			char **parameters = list_to_array(userInputElements);
			programName = parameters[0];

			//Is it a dynamic Path or a total Path to Programm
			if (strchr(programName, '/') == NULL) {
				//Parse dynamic Path by searching the environment
				list_t *pathsList = parsePath();

				int numberOfPaths = list_size(pathsList);
				char **pathsArray = list_to_array(pathsList);
				char path[1024];
				for (int i = 0; i < numberOfPaths; i++) {
					snprintf(path, 1024, "%s/%s", pathsArray[i], programName);
					execve(path, parameters, envp);
				}
			} else {

				char* last_slash_ptr = strrchr(programName, '/');
				int last_slash = last_slash_ptr - programName;
				char *progNameOnly = (char*) malloc(
						(strlen(programName) - last_slash) * sizeof(char) + 1);
				progNameOnly = strncpy(progNameOnly, programName + last_slash + 1,
						strlen(programName) - last_slash);
				parameters[0] = progNameOnly;
				execve(programName, parameters, envp);
			}
			printf("-mysh: %s: command not found\n", programName);
			break;
		case -1:	//ERROR
			return -1;
			break;
		default:	//parent process
			list_append(processes, (char *) &childPID);
			wait(NULL);
			break;
		}

		list_finit(userInputElements);

	}

	//Wait for every child Process
	int wstatus;
	int numOfProcesses = list_size(processes);
	for (int i = 0; i < numOfProcesses; i++) {
		if ((wait(&wstatus)) == -1)
			return -1;
	}

	list_finit(processes);

	return 1;
}

