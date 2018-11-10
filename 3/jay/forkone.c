/*
 * forkone.c
 *
 *  Created on: 10.11.2018
 *      Author: jay
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

void printTime(char *msg){

	struct timeval *time_in_secs = malloc(sizeof(struct timeval *));
	char *date;

	if(gettimeofday(time_in_secs, NULL) != 0)
		return;
	date = ctime(&time_in_secs->tv_sec);
	printf("%s%s", msg, date);

	free(time_in_secs);
}

void countTo(int count_to){
	pid_t pid, ppid, pidCode;
	pid_t i = 1;
	pid = getpid();
	ppid = getppid();
	while(count_to >= i){
		printf("%d %d %d\n", pid, ppid, i);
		sleep(1);
		i++;
	}
	pidCode = ((int) pid + count_to) % 100;
	printf("Exit-Code: %d\n", pidCode);
}


int main(int argc, char **argv) {

	int count_to;
	int state;

	if(argc >= 2)
		count_to = atoi(argv[1]);
	else
		count_to = 10;

	printTime("Start: ");

	if(fork() == 0){
		countTo(count_to);
		exit(0);
	}
	else{
		wait(&state);
	}

	printTime("Ende: ");

	return 1;

}
