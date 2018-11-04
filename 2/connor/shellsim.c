#include <stdio.h>

main(int argc, char const *argv[]) {
    char s[256];
    while(1) {
        if(fgets(s, 255, stdin) == NULL {
            return -1;
        }
        if(strncmp(s,"exit", 4) == 0) {
            return 0;
        }
        else {
            system(s);
        }
    }
}
