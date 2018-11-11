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

  //list for child_pid
  list_t *li = list_init();
  struct list_elem *li_el;
	if(li == NULL){
		perror("Cannot allocate memory");
		exit(-1);
	}

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

  //produce child processes
  for(int i = 1; i <= N; i++){
    int max = K;
    int count = 1;

    if(r == 1){
      srand(time(NULL));
      double r = (1.5*K) - (K/2) + 1;
      max = (K/2) + (int)(r * rand()/(RAND_MAX+1.0));
    }

    pid_t pid = fork();
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
    } else {
      pid_t child_pid = pid;
      pid_t *child_pointer = &child_pid;
      list_insert(li, child_pointer);
    }
  }

  //list_print(li, print_string);
  //return exit codes and remove child_pid

  for(int i = 1; i <= N; i++){
    //save child pid in list
    pid_t child_pid = wait(&exit_code);
    pid_t *child_pointer = &child_pid;
    li_el = list_find(li, child_pointer, NULL);
		list_remove(li, li_el);

    //return exit codes
    if (WIFEXITED(exit_code)){
      printf("Exit-Code: %d\n", WEXITSTATUS(exit_code));
    }
  }


  //print endtime
  time(&end_time);
  printf("Ende: %s", ctime(&end_time));

  list_finit(li);
  return 0;

}
