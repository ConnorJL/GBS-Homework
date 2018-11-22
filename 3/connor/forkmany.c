#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "list.h"

int main(int argc, char const *argv[])
{
    int k = 10;
    int n = 1;
    int r = 0;

    int c;
    while( (c=getopt(argc, argv, "k:n:r")) != -1) {
        switch(c) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'n':
                n = atoi(optarg);
                break;
            case 'r':
                r = 1;
                break;
        }
    }


    list_t* list = list_init();
    pid_t pid;
    time_t now;
    time(&now);
    printf("Start: %s", ctime(&now));

    for(int i=0; i<n; i++) {
        pid = fork();
        if(pid == 0) {
            break;
        }
        else {
            list_append(list, &pid);
        }
    }

    if(pid == 0) {
        // We are a child process
        pid_t self = getpid();
        pid_t parent = getppid();

        if(r == 1) {
            time_t t;
            srand((unsigned) time(&t));
            k = k/2 + ( rand() / (RAND_MAX / (k*1.5-k/2 + 1) + 1));
        }

        for(int i = 1; i<=k; i++) {
            printf("%i %i %i\n", self, parent, i);
            sleep(1);
        }
        return (self+k) % 100;
    }


    else {
        // We are the parent process
        int stat;
        for(int i=0; i<n; i++) {
            wait(&stat);
            int status = WEXITSTATUS(stat);
            printf("Exit-Code: %i\n", status);
            time(&now);
        }

        printf("Ende: %s", ctime(&now));
    }

    return 0; 
}
