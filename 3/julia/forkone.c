#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  time_t start_time;
  time_t end_time;
  int k;
  int i = 1;
  int exit_code;

  if(argc < 2){
    printf("Missing input, terminate.\n");
    return 0;
  }
  k = atoi(argv[1]);

  time(&start_time);
  printf("Start: %s", ctime(&start_time));

  int pid = fork();
  if(pid < 0){
    printf("Error");
  } else if (pid == 0){
    while(i <= k){
      printf("%u %u %d\n", getpid(), getppid(), i);
      sleep(1);
      i++;
    }
    exit((getpid()+k)%100);
  } else {
    wait(&exit_code);
    if (WIFEXITED(exit_code)){
      printf("Exit-Code: %d\n", WEXITSTATUS(exit_code));
    }

    time(&end_time);
    printf("Ende: %s", ctime(&end_time));
  }
  return 0;
}
