/*
 * wildcard.c
 *
 *  Created on: 19.01.2019
 *      Author: jobrm
 */
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#include "wildcard.h"

char *parseFullFilename(char *wildpath) {
	struct dirent **namelist;
	char *element;
	char *result = (char *) malloc(sizeof(char));
	result[0] = '\0';
	int oldsize;
	int newsize;
	int n;
	int i = 0;

	n = scandir(getcwd(NULL, 0), &namelist, NULL, alphasort);
	if (n < 0)
		perror("scandir");
	while (i < n) {
		element = strdup(namelist[i]->d_name);
		free(namelist[i++]);
		if (element[0] == '.') //skip . .. and files starting with .
			continue;
		oldsize = strlen(result);
		newsize = oldsize + strnlen(element, 256) + 2; // +2 because of terminating NULL char and space bewtween elements
		result = (char *) realloc(result, newsize);
		if (oldsize != 0) //skips the first iteration
			strcat(result, " ");
		strcat(result, element);
	}
	free(namelist);
	return result;
}

char *parseStartFilename(char *wildpath) {
	return NULL;
}

char *parseEndFilename(char *wildpath) {
	return NULL;
}

char *parseMidFilename(char *wildpath) {
	return NULL;
}
