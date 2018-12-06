/*
 * parser.c
 *
 *  Created on: 01.12.2018
 *      Author: jobrm
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "list.h"
#define maxSize 1024
#define normal 0
#define encapsulatedA 1  	// encapsulated by "
#define encapsulatedB 2		// encapsulated by '

void clearStringRange(char *string, int from, int to) {
	for (int i = from; i < to; i++)
		string[i] = '\0';
}

void initString(char *string) {
	string[0] = '\0';
}

void appendChar(char* s, char c) {
	int len = strlen(s);
	s[len] = c;
	s[len + 1] = '\0';
}

void appendString(list_t* l, char *string) {
	if (string[0] == '\0')
		return;

	list_append(l, strdup(string));
	initString(string);
}

list_t *parseUserInput(char *userInput) {

	list_t *userInputElements = list_init();
	int userInputIndex = 0;
	int lastIndex = strlen(userInput);
	int memoryNeeded = (maxSize * sizeof(char));
	char c;
	char *currentElement = (char *) malloc(memoryNeeded);
	char *envVarName = (char *) malloc(memoryNeeded);
	char *envVarContent = (char *) malloc(memoryNeeded);
	initString(currentElement);
	initString(envVarName);
	initString(envVarContent);
//	int elementNumber = 0;
	int encState = normal; 	//Encapsulated State

	//Skip Empty Input
	if (lastIndex < 1)
		return userInputElements;

	//Split String Elements
	while (userInputIndex <= lastIndex) {

		c = userInput[userInputIndex];

		switch (c) {
		case '"': 		//toggle Encapsulate A
			if (encState == normal)
				encState = encapsulatedA;
			else if (encState == encapsulatedA)
				encState = normal;
			else { 		//Already Encapsulated by '
				appendChar(currentElement, c);
			}
			userInputIndex++;
			break;
		case '\'': 		//toggle Encapsulate B
			if (encState == normal)
				encState = encapsulatedB;
			else if (encState == encapsulatedB)
				encState = normal;
			else { 		//Already Encapsulated by "
				appendChar(currentElement, c);
			}
			userInputIndex++;
			break;
		case '$':		//Parse Environment Variable
			userInputIndex++;
			int counter = 0;
			//Parse Variable Name
			while (isupper(userInput[userInputIndex])
					|| userInput[userInputIndex] == '_') {
				appendChar(envVarName, userInput[userInputIndex]);
				counter++;
				userInputIndex++;
			}
			//Get Variable Content
			envVarContent = strdup(getenv(envVarName));

			if (0 < counter && envVarContent != NULL) {
					memoryNeeded += strlen(envVarContent) * sizeof(char) - counter - 1;
					currentElement = (char *) realloc(currentElement, memoryNeeded);
					strcat(currentElement, envVarContent); //append VarContent to Output
			}
			initString(envVarName);
			initString(envVarContent);
			break;
		case '\\': 		//Ignore control symbols
			userInputIndex++;
			appendChar(currentElement, userInput[userInputIndex]);
			userInputIndex++;
			break;
		case '\n':		 //Parsing last Element
			if (encState == normal) {
				appendString(userInputElements, currentElement);
				free(currentElement);
				free(envVarName);
				free(envVarContent);
				return userInputElements;		//--------End of Parsing------------
			} else {		//Wrongly Encapsulated
				list_finit(userInputElements);
				printf("ERROR - Wrong encapsulation\n");
				free(currentElement);
				free(envVarName);
				free(envVarContent);
				return NULL;		//--------End of Parsing------------
			}
		case ' ':
			if (encState == normal) {		//End of current element

				while (userInput[userInputIndex] == ' ') //Skip Spaces
					userInputIndex++;
				if(strlen(currentElement) == 0)
					break;
				//Insert current Element into Elements-List
				appendString(userInputElements, currentElement);
				memoryNeeded = (maxSize * sizeof(char));

			} else {		//Normal char, copy to element

				appendChar(currentElement, c);
				userInputIndex++;

			}
			break;
		default: 			//Normal char, copy to element

			appendChar(currentElement, c);
			userInputIndex++;
			break;

		}
	}
//	free(envVarContent);
//	free(envVarName);
//	free(currentElement);
	return userInputElements;
}
