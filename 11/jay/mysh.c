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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

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

	list_t *userInputElements;
	list_t *processes = list_init();
	pid_t childPID;
	char *programName;

	char *userInput = malloc(maxSize * sizeof(char));
//	char *userInput = "echo < hh <\n";	/////////DEBUG

	while (1) {

		//Prompt User to input command
		fprintf(stdout, "%s $ ", getenv("PWD"));
		fflush(stdout);

		if (fgets(userInput, maxSize, stdin) == NULL)
			return -1;

		//Terminate Prompt
		if (strcmp(userInput, "exit\n") == 0)
			break;

		//Transform shell command into list of command elements
		userInputElements = parseUserInput(userInput);

		////////////////////////////////////////
		///// START: ADDING CD SUPPORT /////////
		////////////////////////////////////////

		if (strcmp(userInputElements->first->data, "cd") == 0) {

			switch (list_size(userInputElements)) {
			case 1: //only cd without parameter

				fprintf(stdout, "%s\n", getenv("PWD"));
				fflush(stdout);
				break;

			case 2: //cd with one parameter
				;
//				char **parameters = list_to_array(userInputElements);
				int check;
				check = chdir(userInputElements->first->next->data);
				if (check < 0) { //Error
					fprintf(stdout, "%s\n", strerror(errno));
					fflush(stdout);
				} else  //change dir successfull
					check = setenv("PWD", getcwd(NULL, 0), 1);
				break;

			default: //cd with more parameter -> ERROR

				fprintf(stdout, "%s\n", "Usage: cd <dir>");
				fflush(stdout);
				break;

			}

			continue;
		}

		////////////////////////////////////////
		///// STOP: ADDING CD SUPPORT //////////
		////////////////////////////////////////

		childPID = fork();
//		childPID = 0;	/////////DEBUG

		switch (childPID) {
		case 0: //child process
			;
			char **parameters = list_to_array(userInputElements);
			programName = parameters[0];

			//Adding I/O-FlowControl
			int i = 1;
			char *parameter = parameters[i];
			int src_fd = 0, dst_fd = 1;
			int alreadyReading = 0; 	//Has the < or > been used only once?
			int alreadyWriting = 0;

			while (parameter != NULL) {

				if (strcmp(parameter, "<") == 0) {
					if (alreadyReading) {
						printf("-mysh: error: use < only once\n");
						return -1;
					}
					close(0);
					if (parameters[i + 1] != NULL) {
						src_fd = openat(AT_FDCWD, parameters[i + 1], O_RDONLY);
						if (src_fd == -1) {
							printf("-mysh: error: file %s could not be found\n",
									parameters[i + 1]);
							return -1;
						}
						alreadyReading++;
					}
				}

				if (strcmp(parameter, ">") == 0) {
					if (alreadyWriting) {
						printf("-mysh: error: use > only once\n");
						return -1;
					}
					close(1);
					if (parameters[i + 1] != NULL) {
						dst_fd = openat(AT_FDCWD, parameters[i + 1],
						O_WRONLY | O_CREAT | O_TRUNC, 0666);

						if (dst_fd == -1) {
							printf("-mysh: error: file %s could not created\n",
									parameters[i + 1]);
							return -1;
						}
					}
					alreadyWriting++;
				}

				i++;
				parameter = parameters[i];
			}

			// Cut of Flow Control from parameters
			i = 1;
			parameter = parameters[i];
			while (parameter != NULL) {
				if (strcmp(parameter, "<") == 0 || strcmp(parameter, ">") == 0) {

					parameters[i] = NULL;
					break;
				}
				i++;
				parameter = parameters[i];
			}

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
			return -1;
		case -1:
			//ERROR
			return -1;
			break;
		default:
			//parent process
			list_append(processes, (char *) &childPID);
			wait(NULL);
			fflush(stdout);
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

	free(userInput);
	list_finit(processes);

	return 1;
}

