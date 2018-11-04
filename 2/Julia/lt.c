#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

//print_elem for list_print
void print_string (char *data){
  printf("%s\n", data);
}

int main(int argc, char **argv) {
  list_t *li;
  struct list_elem *elem;
  if((li = list_init ()) == NULL) {
    perror ("Cannot allocate memory" ); exit (1);
  }

  for(int i=1; i < argc; i = i+2){

    if(strncmp(argv[i],"-a", 2) == 0){
      list_append(li, argv[i+1]);
    } else if (strncmp(argv[i],"-i", 2) == 0){
      list_insert(li, argv[i+1]);
    } else if (strncmp(argv[i],"-r", 2) == 0){
      elem = list_find(li,argv[i+1],strcmp);
      list_remove(li, elem);
    } else {
      break;
    }

  }

  list_print (li , print_string);
  exit(0);

/*
  if(list_append (li , "erstes") == NULL || list_append (li , "zweites") == NULL || list_append (li, "drittes")==NULL) {
    perror ("Cannot allocate memory" );
    exit(1);
  }

  list_print (li , print_string);
  list_finit (li);
  //list_remove(li, li->first);
  list_print (li , print_string);
  exit(0);
*/
}
