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
#include "list.h"

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

int getRandInRange(int min, int max) {
	int random = rand();
	return min + ( random / (RAND_MAX / (max-min + 1) + 1));
}

int main(int argc, char **argv) {

	int counter, count_to = 10, opt;
	int childcount = 1;
	int random_mode = 0;
	int pid, i, state;

	list_t *li;
	struct list_elem *li_el;
	if((li = list_init()) == NULL){
		perror("Cannot allocate memory");
		exit(-1);
	}

	//Parsing Opt Codes
    while ((opt = getopt(argc, argv, "k:n:r")) != -1) {
        switch (opt) {
        case 'k':
        	count_to = atoi(optarg);
            break;
        case 'n':
        	childcount = atoi(optarg);
            break;
        case 'r':
			random_mode = 1;
			break;
//        default: /* '?' */
//            exit(EXIT_FAILURE);
        }
    }

    //Threads creating loop
	i = childcount;
    while(childcount > 0){

    	if(random_mode){
    		counter = getRandInRange(count_to/2, 1.5*count_to);
    	}
    	else{
    		counter = count_to;
    	}

		if((pid = fork()) == 0){
			list_append(li, pid);
			printTime("Start: ");
			countTo(counter);
			printTime("Ende: ");
			exit(0);
		}

		childcount--;
    }

	//Wait for every child Process
	childcount = i;
	while(childcount > 0){
		pid = wait(&state);;
		li_el = list_find(li,pid,NULL);
		list_remove(li, li_el);
		childcount--;
	}

    list_finit(li);
	return 1;

//	//Wait for every child Process
//	li_el = list_get(li);
//    while(li_el != NULL){
//    	pid = li_el->data;
//    	waitpid(pid, NULL, 0);
//    	free(li_el);
//    	li_el = list_get(li);
//    }

}
