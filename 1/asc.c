#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("Oct\tDec\tHex\tChar\n");
    for(int i=0; i<128; i++) {
        printf("%.3o\t%-3d\t%.2x\t%c\n", i, i, i, i);
    }
    return 0;
}
