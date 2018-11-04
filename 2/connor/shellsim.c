#include <stdio.h>

main(int argc, char const *argv[]) {
    char s[256];
    while(1) {
        fgets(s, 255, stdin);
        if(strncmp(s,"exit", 4) == 0) {
            return 0;
        }
        else {
            system(s);
        }
    }
}
