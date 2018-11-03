#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

//print_elem for list_print
void print_string (char *data){
  printf("%s\n", data);
}

//cmp_elem for list_find
int cmp_elem(const char *data1, const char *data2){
  return strcmp(data1, data2);
}

int main(int argc , char *argv [] , char *envp []) {
  list_t *li;
  if((li = list_init ()) == NULL) {
    perror ("Cannot allocate memory" ); exit (1);
  }

  if(list_append (li , "erstes") == NULL || list_append (li , "zweites") == NULL || list_append (li, "drittes")==NULL) {
    perror ("Cannot allocate memory" );
    exit(1);
  }

  list_print (li , print_string);
  exit(0);
}
