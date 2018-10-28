#include<stdio.h>
#include<string.h>

extern void hexdump (FILE *output, unsigned char *buffer, int length);

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Missing input\n");
  } else {

    for(int i=1; i < argc; i++){
      int size = strlen(argv[i]);
      char input[size+1];
      strcpy(input, argv[i]);
      hexdump(stdout, input, sizeof(input));
    }
  }

  printf("\n");
}
