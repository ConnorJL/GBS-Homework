#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "list.h"

struct thread_data *t_data_init(int num, int prio, int start, int laufzeit) {
    struct thread_data *t_d = (struct thread_data *) malloc(2 * (sizeof(void *)));
    t_d->num = num;
    t_d->prio = prio;
    t_d->start = start;
    t_d->time_runned = 0;
    t_d->end = laufzeit;
    t_d->inQueue = 0;
    return t_d;
}

list_t *list_init() {
    list_t *lst = (list_t *) malloc(2 * (sizeof(void *)));
    lst->first = NULL;
    lst->last = NULL;
    return lst;
}

int list_len(list_t *list) {
    if (list == '\0' || list->first == '\0') {
        return 0;
    }
    if (list->first == list->last) {
        return 1;
    }
    int len = 1;
    struct list_elem *curr = list->first;
    while (1) {
        curr = curr->next;
        if (curr == '\0')
            return len;
        len += 1;
    }
}

void *list_get(list_t *list, int ind) {
    struct list_elem *curr = list->first;
    for (int i = 0; i < ind; i++) {
        if (curr->next == '\0') {
            printf("Warning: ind %d is out of range!", ind);
            return 0;
        }
        curr = curr->next;
    }
    return curr->data;
}

void *list_get_p(list_t *list, int ind) {
    struct list_elem *curr = list->first;
    for (int i = 0; i < ind; i++) {
        if (curr->next == '\0') {
            printf("Warning: ind %d is out of range!", ind);
            return 0;
        }
        curr = curr->next;
    }
    return curr;
}

struct list_elem *list_insert(list_t *list, void *data) {
    struct list_elem *elm = (struct list_elem *) malloc(2 * (sizeof(void *)));
    elm->next = list->first;
    elm->data = data;
    list->first = elm;
    if (list->last == NULL) {
        list->last = elm;
    }
    return elm;
}

struct list_elem *list_append(list_t *list, void *data) {
    struct list_elem *elm = (struct list_elem *) malloc(2 * (sizeof(void *)));
    elm->next = NULL;
    elm->data = data;
    if (list->last == NULL) {
        list->first = elm;
        list->last = elm;
    } else {
        list->last->next = elm;
        list->last = elm;
    }
    return elm;
}

void *list_remove(list_t *list, struct list_elem *elem) {
    char *data = malloc(sizeof(*elem->data));
    memcpy(data, elem->data, sizeof(*elem->data));
    if (list->first == elem) {
        list->first = elem->next;
        if (list->last == elem) {
            list->last = NULL;
        }
    } else {
        struct list_elem *curr = list->first;
        if (curr != NULL) {
            while (1) {
                if (curr->next == elem) {
                    curr->next = curr->next->next;
                    if (curr->next == list->last) {
                        list->last = curr;
                    }
                    break;
                }
                if (curr->next == NULL) {
                    return NULL;
                }
                curr = curr->next;
            }
        }
    }
    free(elem);
    return data;
}

void list_finit(list_t *list) {
    struct list_elem *curr = list->first;
    if (curr != NULL) {
        while (1) {
            if (curr->next == NULL) {
                list->last = NULL;
                list->first = NULL;
                break;
            }
            curr = curr->next;
            free(list_remove(list, list->first));
        }
    }
}

void *list_first(list_t *list) {
    int *r = list->first->data;
//    list_remove(list, list->first);
    return r;
}

void list_print(list_t *list, void (*print_elem)(void *)) {
    struct list_elem *curr = list->first;
    while (1) {
        print_elem(curr->data);
        if (curr->next == NULL) {
            break;
        }
        curr = curr->next;
    }
}

void list_dump(list_t *list, void (*print_elem)(struct list_elem *)) {
    struct list_elem *curr = list->first;
    while (1) {
        print_elem(curr);
        if (curr->next == NULL) {
            break;
        }
        curr = curr->next;
    }
}

struct list_elem *list_find(list_t *list, void *data, int (*cmp_elem)(const void *, const void *)) {
    struct list_elem *curr = list->first;
    if (curr != NULL) {
        while (1) {
            if (cmp_elem(curr->data, data) == 0) {
                return curr;
            }
            if (curr->next == NULL) {
                return NULL;
            }
            curr = curr->next;
        }
    }
    return NULL;
}

void **list_to_array(list_t *list) {
    int llen = list_len(list);
    void ** arr = malloc((llen + 1) * sizeof(void *));
    arr[llen] = NULL;
    for (int i = 0; i < llen; i++) {
        arr[i] = list_get(list, i);
    }
    return arr;
}

struct list_elem list_put (list_t *list, struct list_elem *current, void *data) {
    if (current == list->last) {
        return *list_append(list, data);
    } else {
        struct list_elem *elm = (struct list_elem *) malloc(2 * (sizeof(void *)));
        elm->next = current->next;
        elm->data = data;
        current->next = elm;
        return *elm;
    }
}

//ssh -p 22222 student@localhost