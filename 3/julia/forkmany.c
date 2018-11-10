#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "list.h"

int main(int argc, char *argv[]) {
  int exit_code;
  int param = 0;
  int r = 0;
  int N = 1;
  int K = 10;

  //handle arguments
  while((param = getopt(argc, argv, "n:k:r")) != -1){
    switch (param){
      case 'n':
        N = atoi(optarg);
        break;
      case 'r':
        r = 1;
        break;
      case 'k':
        K = atoi(optarg);
        break;
    }
  }

  //print starttime
  time_t start_time;
  time_t end_time;
  time(&start_time);
  printf("Start: %s", ctime(&start_time));

  //produce working child processes
  for(int i = 1; i <= N; i++){
    int max = K;
    int count = 1;

    if(r == 1){
      srand(time(NULL));
      max = (rand() % (int)(1.5*K)) + (K/2);
    }

    int pid = fork();
    if(pid < 0){
      printf("Error");
      break;
    } else if (pid == 0){
      while(count <= max){
        printf("%u %u %d\n", getpid(), getppid(), count);
        sleep(1);
        count++;
      }
      exit((getpid()+max)%100);
    }
  }

  //return exit codes and save children in list
  list_t *list_of_children = list_init();
	if(list_of_children == NULL){
		perror("Cannot allocate memory");
		exit(-1);
	}
  for(int i = 1; i <= N; i++){
    //save child pid in list
    int child_pid = wait(&exit_code);
    char child_pid_char[10];
    snprintf( child_pid_char, 10, "%d", child_pid);
    char *child_pid_pointer = child_pid_char;
    list_insert(list_of_children, child_pid_pointer);

    //return exit codes
    if (WIFEXITED(exit_code)){
      printf("Exit-Code: %d\n", WEXITSTATUS(exit_code));
    }
  }

  //print endtime
  time(&end_time);
  printf("Ende: %s", ctime(&end_time));

}