#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char cmd[256];

  printf("User: ");
  fgets(cmd, sizeof(cmd), stdin);

  while(strncmp(cmd, "exit", 4) != 0){

    if(cmd[strlen(cmd) - 1] == '\n'){
      cmd[strlen(cmd) - 1] = '\0';
    }

    system(cmd);

    printf("User: ");
    fgets(cmd, sizeof(cmd), stdin);
  }
}
