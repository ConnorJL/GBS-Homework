#include <stdio.h>
#include <stdlib.h>
#include "list.h"


void print_string(char* data) {
    static int count = 1;
    printf("%i:%s\n", count, data);
    count++;
}

int main(int argc, char const *argv[]) {
    list_t *li;

    if ((li = list_init()) == NULL) {
        perror ("Cannot allocate memory");
        exit(-1);
   }
    for(int i=1; i<argc; i++) {

        if(strcmp(argv[i], "-a") == 0) {
            i++;
            if(i>=argc) {
                continue;
            }
            list_append(li, argv[i]);
        } 

        else if(strcmp(argv[i], "-i") == 0) {
            i++;
            if(i>=argc) {
                continue;
            }
            list_insert(li, argv[i]);
        }
        else if(strcmp(argv[i], "-r") == 0) {
            i++;
            if(i>=argc) {
                 continue;
            }
            struct list_elem* ele = list_find(li, argv[i], strcmp);
            if(ele != NULL) {
                list_remove(li, ele);
            }
        }
    }

    list_print(li, print_string);
    exit(0);  

//
//    if(list_append(li, "erstes") == NULL ||
//    		list_append(li, "zweites") == NULL ||
//    		list_append(li, "drittes") == NULL ){
//        		perror("Cannot allocate memory");
//    		exit(-1);
//    	}
//    	
//    	list_print(li, print_string);
//        printf("Test\n");
//    	return 1;
}


