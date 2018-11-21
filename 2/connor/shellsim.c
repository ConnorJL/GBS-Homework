#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    char input[256];
    while(1) {
        if(fgets(input, 255, stdin) != NULL) {
            if(strncmp(input, "exit\n") == 0) {
                return 0;
            }
            else {
                system(input);
            }
        }
    }
}
