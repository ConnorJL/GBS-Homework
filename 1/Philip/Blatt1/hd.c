#include<stdio.h>
#include"hexdump.c"

int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		hexdump(stdout, argv[i], 16);
		//if (i+1 != argc) {
		//	fprintf(stdout, "\n");
		//}
	}
	char *inp1 = "Grundlagen Betriebssystem und Systemsoftware";
	char *inp2 = "Test";
	char *inp3 = "";
	char *inp4 = "Viel zu langer Text mit Random Sachen wie zB dem Code: hexdump(stdout, inp1, 16);\nreturn 0;";
	//hexdump(stdout, inp1, 16);
	return 0;
}