/*
 * threadit.c
 *
 *  Created on: 17.11.2018
 *      Author: jobrm
 */
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "list.h"

void printTime(char *msg) {

	struct timeval *time_in_secs = malloc(sizeof(struct timeval *));
	char *date;

	if (gettimeofday(time_in_secs, NULL) != 0)
		return;
	date = ctime(&time_in_secs->tv_sec);
	printf("%s%s", msg, date);

	free(time_in_secs);
}

void countTo(int count_to) {
	long int i = 1;
	while (count_to >= i) {
		printf("%10u\t%d\t%ld\n", (unsigned int) pthread_self(), getpid(), i);
		sleep(1);
		i++;
	}
}

int getRandInRange(int min, int max) {
	int random = rand();
	return min + (random / (RAND_MAX / (max - min + 1) + 1));
}

static void *thread_start(void *arg) {

	countTo(*(int *) arg);

	return 0;
}

int main(int argc, char **argv) {

	int count_to = 10, opt;
	int threadcount = 1;
	int random_mode = 0;
	int s;
	pthread_t thread;

	list_t *li;
	struct list_elem *li_el;
	if ((li = list_init()) == NULL) {
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
			threadcount = atoi(optarg);
			break;
		case 'r':
			random_mode = 1;
			break;
		}
	}

	//Generating Array of random numbers to count to
	int counter[threadcount], i = 0;
	while (i < threadcount) {

		if (random_mode)
			counter[i] = getRandInRange(count_to / 2, 1.5 * count_to);
		else
			counter[i] = count_to;
		i++;
	}

	printTime("Start: ");
	//Threads creating loop
	while (threadcount > 0) {

		if ((s = pthread_create(&thread, NULL, &thread_start,
				&counter[threadcount - 1])) == 0) {
			list_append(li, thread);
		}

		threadcount--;
	}

	//Wait for every child Process
	li_el = list_get(li);
	while (li_el != NULL) {
		s = pthread_join(li_el->data, NULL);
		free(li_el);
		li_el = list_get(li);
	}

	printTime("Ende: ");
	list_finit(li);
	return 1;

}
