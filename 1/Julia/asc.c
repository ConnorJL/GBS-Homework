#include<stdio.h>

int main() {
	printf("%-s\t%-s\t%-s\t%-s\n", "Oct", "Dec", "Hex", "Char");

  for (int i = 0; i < 127+1; i++) {
			printf("%03o\t%-d\t%02x\t%-c\n", i, i, i, i);
  }

}
