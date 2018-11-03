#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"


list_t *list_init (){
    list_t *list = NULL;
    list = (list_t*)malloc(sizeof(list_t));
    return list;
}


struct list_elem *list_insert (list_t *list, char *data){
    struct list_elem *elem = malloc(sizeof(struct list_elem));
    elem->data = data;
    elem->next = list->first;
    list->first = elem;

    return elem;
}


struct list_elem *list_append (list_t *list, char *data){
  struct list_elem *current = list->first;
  while (current != NULL) {
      current = current->next;
   }
   current->data = data;
   return current;
}


int list_remove (list_t *list, struct list_elem *elem){
  struct list_elem *current = list->first;
  while (current != NULL) {
      if(current == elem){
          struct list_elem *tmp;
          tmp = current;
          current = tmp->next;
          free(tmp);
          return -1;
      }
      current = current->next;
   }
   return 0;
}


void list_finit (list_t *list){
  struct list_elem *current = list->first;
  while (current != NULL) {
      struct list_elem *tmp;
      tmp = current;
      current = tmp->next;
      free(tmp);

      current = current->next;
  }

  free(list);
}


void list_print (list_t *list, void (*print_elem) (char *)){
  struct list_elem *current = list->first;
  int i = 1;
  while (current != NULL) {
      printf("%d: ", i);
      print_elem(current->data);

      current = current->next;
      i++;
  }
}

struct list_elem *list_find (list_t *list, char *data, int (*cmp_elem) (const char *, const char *)){
  struct list_elem *current = list->first;
  while (current != NULL) {
      if(cmp_elem(current->data, data) == 0){
        return current;
      }
      current = current->next;
  }
  return NULL;
}
