#include<stdio.h>
#include <stdlib.h>
#include<string.h>

int main() {
	char inp[256];
	int errcode = read(0, inp, 256);
	inp[strcspn(inp, "\n")] = 0;
	write(1, "Hallo: ", 7);
	write(1, inp, strlen(inp));
	write(1, " -- ", 4);
	int counte = 0;
	int stillValid = 1;
	for (int i = 0; i < strlen(inp); i++) {
		if ((int)inp[i] >= (int)'a' && (int)inp[i] <= (int)'z') {
			inp[i] += (int)'A' - (int)'a';
		}
		if (stillValid == 1 && (int)inp[i] >= (int)'A' && (int)inp[i] <= (int)'Z') {
			inp[i] += 13;
			if ((int)inp[i] > (int)'Z') {
				inp[i] -= 26;
				}
			counte += 1;
			}
		else {
			stillValid = 0;
		}
	}
	write(1, inp, counte);
	write(1, "\n", 1);
	return 0;
}