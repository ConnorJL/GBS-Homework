#include<stdio.h>
#include <stdlib.h>
#include<string.h>

int main() {
	printf("Oct\tDec\tHex\tChar\n");
	for (int i = 0; i < 128; i++) {
		printf("%03o\t%-3d\t%02x\t%c\n", i, i, i, (char)i);
		}
	return 0;
}