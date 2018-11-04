/*
 * lt.c
 *
 *  Created on: 03.11.2018
 *      Author: jobrm
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"

int main(int argc, char **argv) {

	list_t *li;
	struct list_elem *li_el;
	if((li = list_init()) == NULL){
		perror("Cannot allocate memory");
		exit(-1);
	}

//	char argv[11][256] = {{"./lt"},{"-i"},{"Happy"},{"-i"},{"GBS"},{"-i"},{"everyone"},{"-r"},{"GBS"},{"-a"},{"Now"}};
//	int argc = 11;


	int i = 1;
	char *cmd = argv[i];
	char *args = argv[i+1];

	while(i < argc && cmd != NULL && args != NULL){

		if(cmd[0] != '-'){
			i++;
			continue;
		}

		switch(cmd[1]){
			case 'a':
				list_append(li, args);
				break;
			case 'i':
				list_insert(li, args);
				break;
			case 'r':
				li_el = list_find(li,args,strcmp);
				list_remove(li, li_el);
				break;
		}
		i += 2;
		cmd = argv[i];
		args = argv[i+1];
	}

	list_print(li, print_string);





//	list_t *li;
//
//	if((li = list_init()) == NULL){
//		perror("Cannot allocate memory");
//		exit(-1);
//	}
//	if(list_append(li, "erstes") == NULL ||
//		list_append(li, "zweites") == NULL ||
//		list_append(li, "drittes") == NULL ){
//		perror("Cannot allocate memory");
//		exit(-1);
//	}
//	if(list_insert(li, "erstes") == NULL ||
//		list_insert(li, "zweites") == NULL ||
//		list_insert(li, "drittes") == NULL ){
//		perror("Cannot allocate memory");
//		exit(-1);
//	}
//	list_print(li, print_string);
//	list_finit(li);
//	list_print(li, print_string);
//	return 1;
}

