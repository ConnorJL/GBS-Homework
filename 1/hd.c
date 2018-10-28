#include <stdio.h>
#include <string.h>

void hexdump(FILE *output, char *buffer, int length);


int main(int argc, char const *argv[])
{
    for(int i=1; i<argc; i++) {
        hexdump(stdout, argv[i], strlen(argv[i]));
        printf("\n");
    }
    return 0;
}
