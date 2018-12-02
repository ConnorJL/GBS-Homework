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

list_t *parseUserInput(char *userInput) {

	list_t *userInputElements = list_init();
	int userInputIndex = 0;
	int lastIndex = strlen(userInput);
	char *currentElement = malloc(maxSize * sizeof(char));
	char *envVarName = malloc(maxSize * sizeof(char));
	char *envVarContent = malloc(maxSize * sizeof(char));
	int elementNumber = 0;
	int elementIndex = 0;
	int encState = normal; 	//Encapsulated State

	//Add Output Info
//	sprintf(currentElement, "%d:", elementNumber + 1);

	//Skip Empty Input
	if (lastIndex <= 1)
		return userInputElements;

	//Split String Elements
	while (userInputIndex <= lastIndex) {

		switch (userInput[userInputIndex]) {
		case '"': 		//toggle Encapsulate
			if (encState == normal)
				encState = encapsulatedA;
			else if (encState == encapsulatedA)
				encState = normal;
			else { 		//Already Encapsulated by '
				currentElement[elementIndex] = userInput[userInputIndex];
				elementIndex++;
			}
			userInputIndex++;
			break;
		case '\'': 		//toggle Encapsulate
			if (encState == normal)
				encState = encapsulatedB;
			else if (encState == encapsulatedB)
				encState = normal;
			else { 		//Already Encapsulated by "
				currentElement[elementIndex] = userInput[userInputIndex];
				elementIndex++;
			}
			userInputIndex++;
			break;
		case '$':		//Parse Environment Variable
			userInputIndex++;
			int counter = 0;
			while (isupper(userInput[userInputIndex])) {
				envVarName[counter] = userInput[userInputIndex];
				counter++;
				userInputIndex++;
			}
			if (0 < counter) {
				envVarContent = getenv(envVarName);
				counter = strlen(envVarContent);
				currentElement = realloc(
						currentElement,	//Adapt Memory size to new Stringsize
						(maxSize + counter + strlen(currentElement))
								* sizeof(char));
				strcat(currentElement, envVarContent); //append VarContent to Output
				elementIndex += counter;
			}
			break;
		case '\\': 		//Ignore control symbols
			userInputIndex++;
			currentElement[elementIndex] = userInput[userInputIndex];
			userInputIndex++;
			elementIndex++;
			break;
		case '\n':		 //Parsing last Element
			if (encState == normal) {
				currentElement[elementIndex] = '\0';
				if (list_append(userInputElements,
						strdup(currentElement)) == NULL)
					return userInputElements; //ERROR
				//End of Parsing
				return userInputElements;
			} else {		//Wrongly Encapsulated
				list_finit(userInputElements);
				userInputElements = list_init();
				if (list_append(userInputElements,
						"ERROR - Wrong encapsulation") == NULL)
					return userInputElements; //ERROR
				return userInputElements;
			}
		case ' ':
			if (elementIndex == 0 && elementNumber == 0) { //If there are Spaces before first element
				while (userInput[userInputIndex] == ' ') //Skip Spaces or null-chars
					userInputIndex++;
				break;
			}
			if (encState == normal) {		//End of current element
				//Terminate and Insert current Element into Elements-List
				currentElement[elementIndex] = '\0';
				if (list_append(userInputElements,
						strdup(currentElement)) == NULL)
					return userInputElements; //ERROR

				elementNumber++;
				elementIndex = 0; 	//Reset Counter to 2 (0 & 1 is OutputInfo)

				while (userInput[userInputIndex] == ' ') //Skip Spaces or null-chars
					userInputIndex++;

				if (userInput[userInputIndex] == '\n') //If there are Spaces after last element
					return userInputElements;
//				if (userInputIndex < lastIndex)
//					//Add Output Info for next Element
//					sprintf(currentElement, "%d:", elementNumber + 1);
			} else {		//Normal char, copy to element
				currentElement[elementIndex] = userInput[userInputIndex];
				userInputIndex++;
				elementIndex++;
			}
			break;
		default: 			//Normal char, copy to element
			currentElement[elementIndex] = userInput[userInputIndex];
			userInputIndex++;
			elementIndex++;
			break;
		}
	}

	return userInputElements;
}
