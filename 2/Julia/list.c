#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"


list_t *list_init (){
    list_t *list = malloc(sizeof(2 * sizeof(struct list_elem *)));

    if(list == NULL){
      return NULL;
    }

    list->first = NULL;
  	list->last = NULL;
    return list;
}


struct list_elem *list_insert (list_t *list, char *data){
    if(list == NULL){
      return NULL;
    }

    struct list_elem *elem = malloc(sizeof(struct list_elem *) + sizeof(char *));
    elem->data = data;
    if(list->first != NULL){
      elem->next = list->first;
    } else {
      list->last = elem;
    }
    list->first = elem;

    return elem;
}


struct list_elem *list_append (list_t *list, char *data){
  if(list == NULL){
    return NULL;
  }

  struct list_elem *elem = malloc(sizeof(struct list_elem *) + sizeof(char *));
  elem->data = data;
  elem->next = NULL;

  if(list->last != NULL){
    list->last->next = elem;
  } else {
    list->first = elem;
  }

  list->last = elem;
   return elem;
}


int list_remove (list_t *list, struct list_elem *elem){
  if(list == NULL){
    return -1;
  }

  struct list_elem *current = list->first;

  if(current == NULL){
    return -1;
  } else if(current == elem){
    list->first = current->next;
    free(current);
    return 0;
  } else {
    struct list_elem *prev = current;
    current = current->next;

    while(current->next != NULL){
      if(current == elem){
        prev->next = current->next;
        free(current);
        return 0;
      }
      prev = current;
      current = current->next;
    }
    if(current == elem){
      prev->next = NULL;
      list->last = prev;
      free(current);
      return 0;
    }
  }
  return -1;
}


void list_finit (list_t *list){
  if(list == NULL){
    return;
  }

  struct list_elem *current = list->first;
  if(current == NULL){
    free(current);
    return;
  } else {
    struct list_elem *prev = current;
    current = current->next;

    free(prev);
    while(current->next != NULL){
      prev = current;
      current = current->next;
      free(prev);

    }
    free(current);
    free(list);
  }
}


void list_print (list_t *list, void (*print_elem) (char *)){
  if(list == NULL){
    return;
  }

  struct list_elem *current = list->first;
  int i = 1;
  while (current != NULL) {
      printf("%d:", i);
      print_elem(current->data);

      current = current->next;
      i++;
  }
}

struct list_elem *list_find (list_t *list, char *data, int (*cmp_elem) (const char *, const char *)){
  if(list == NULL){
    return NULL;
  }

  struct list_elem *current = list->first;
  while (current != NULL) {
      if(cmp_elem(current->data, data) == 0){
        return current;
      }
      current = current->next;
  }
  return NULL;
}
