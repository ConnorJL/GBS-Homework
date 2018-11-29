#include <stdio.h>
#include <string.h>

#include "list.h"


list_t* parse(char* s);

void print_list(list_t *t) {
    struct list_elem *current = t->first;
    int i = 1;
    while (current != NULL) {
        printf("%i:%s\n", i, current->data);
        current = current->next;
        i++;
    }
}

int main(int argc, char const *argv[], char *envp[]) {
    char s[1024];

    while(1) {
        printf("$ ");
        if(fgets(s, 1024, stdin) != NULL) {
            list_t* list = parse(s);
            print_list(list);
            if(strcmp(s, "exit\n") == 0) {
                return 0;
            }
        }
    }
    
    return 0;
}
