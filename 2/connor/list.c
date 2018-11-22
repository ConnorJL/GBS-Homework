#include <stdio.h>
#include <stdlib.h>
#include "list.h"

list_t *list_init() {
    list_t *list = (list_t*) malloc(sizeof(list_t));
    if(list != NULL) {
        list->first = NULL;
        list->last = NULL;
        return list;
    }
    else {
        return NULL;
    }
}

struct list_elem *list_insert(list_t *list, char *data) {
    if(list == NULL) {
        return NULL;
    }
    struct list_elem *new = (struct list_elem*) malloc(sizeof(struct list_elem));
    if(new == NULL) {
        return NULL;
    }

    new->next = list->first;
    new->data = data;

    list->first = new;
    if(list->last == NULL) {
        list->last = new;
    }
    return new;
}

struct list_elem *list_append(list_t *list, char *data) {
    if(list == NULL) {
        return NULL;
    }
    struct list_elem *new = (struct list_elem*) malloc(sizeof(struct list_elem));
    if(new == NULL) {
        return NULL;
    }

    new->next = NULL;
    new->data = data;

    if(list->first == NULL) {
        list->first = new;
        list->last = new;
        return new;
    }
    
    list->last->next = new;
    list->last = new;
    return new;
}

int list_remove(list_t *list, struct list_elem *elem) {
    if(list == NULL) {
        return -1;
    }

    struct list_elem *current = list->first;
    struct list_elem *previous = NULL;


    while(current != NULL) {
        if(current == elem) {
            // First elem of list
            if(previous == NULL) {
                list->first = list->first->next;
                // Only element?
                if(list->last == current) {
                    list->last = list->first;
                }
            }
            // Not first elem
            else {
                previous->next = current->next;
            }
            free(current);
            return 0;
        }
        previous = current;
        current = current->next;
    }
    
    return -1;
}

void list_finit(list_t *list) {
    if(list == NULL) {
        return NULL;
    }
    struct list_elem *current = list->first;
    struct list_elem *next = NULL;


    while(current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(list);
}

struct list_elem *list_find(list_t *list, char *data, int (*cmp_elem) (const char *, const char *)) {
    if(list == NULL) {
        return NULL;
    }
    struct list_elem *current = list->first;

    while(current != NULL) {
        if(cmp_elem(data, current->data) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void list_print(list_t *list, void (*print_elem) (char *)) {
    if(list == NULL || list->first==NULL) {
        return NULL;
    }
    struct list_elem *current = list->first;

    while(current != NULL) {
        print_elem(current->data);
        current = current->next;
    }
}




