#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int k = atoi(argv[1]);

    time_t now;
    time(&now);
    printf("Start: %s", ctime(&now));

    pid_t pid = fork();

    if(pid == 0) {
        // We are the child process
        pid_t self = getpid();
        pid_t parent = getppid();

        for(int i = 1; i<=k; i++) {
            printf("%i %i %i\n", self, parent, i);
            sleep(1);
        }
        return (self+k) % 100;
    }
    else {
        int stat;
        wait(&stat);
        int status = WEXITSTATUS(stat);
        printf("Exit-Code: %i\n", status);
        time(&now);
        printf("Ende: %s", ctime(&now));
    }



    return 0;
}
