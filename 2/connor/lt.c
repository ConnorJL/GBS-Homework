#include <stdio.h>
#include <stdlib.h>
#include "list.h"


void print_string(char* data) {
    static int count = 1;
    printf("%i:%s\n", count, data);
    count++;
}

int main(int argc, char const *argv[]) {
    printf("Test");
    list_t *li;

    if ((li = list_init()) == NULL) {
        perror ("Cannot allocate memory");
        exit(-1);
    }

    if (list_append(li, "erstes") == NULL ||
        list_append(li, "zweites") == NULL ||
        list_append(li, "drittes") == NULL) {
            perror ("Cannot allocate memory");
            exit(-1);
    }
    list_print(li, print_string);
    exit(0);
}