/*
 * threadit.c
 *
 *  Created on: 17.11.2018
 *      Author: jobrm
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "list2.h"

void print_time_step(int time, int thread_num) {
	static int first_time = 1;
	int i;

	if (first_time) {
		printf("  Time |  1  2  3  4  5  6  7  8  9  10\n");
		printf("-------+--------------------------------\n");
		first_time = 0;
	}
	printf("%06d |", time);
	if (thread_num) {
		for (i = 1; i < thread_num; i++)
			printf("   ");
		printf("  %d\n", thread_num);
	} else
		printf("\n");
}

struct tData *parseUserInput(int i) {

	struct tData *yarn = malloc(sizeof(struct tData));
	int prio = 0, startT = 0, totalRunT = 0;

	fscanf(stdin, "%d %d %d", &prio, &startT, &totalRunT);

	// Checking for correct range
	if (startT < 0)
		startT = 0;
	if (100000 < startT)
		startT = 100000;

	if (totalRunT < 0)
		totalRunT = 1;
	if (30000 < totalRunT)
		totalRunT = 30000;

	if (prio < 1)
		prio = 1;
	if (10 < prio)
		prio = 10;

	yarn->tNumber = i;
	yarn->priority = prio;
	yarn->startTime = startT;
	yarn->totalRuntime = totalRunT;
	yarn->execRuntime = 0;

	return yarn;
}

int main(int argc, char **argv) {

	int timeStep = 10, opt;
	int threadcount = 1;
	int time_quantum = 1;
	int i = 0, k = 0, algo = 1;
	int time = 0;
	int wasIdle = 1;

	list_t *li;
	list_t *queue;
	struct tData *yarn;
	struct tData *yarnQueued;
	if ((li = list_init()) == NULL) {
		perror("Cannot allocate memory");
		exit(-1);
	}
	if ((queue = list_init()) == NULL) {
		perror("Cannot allocate memory");
		exit(-1);
	}

	//Parsing Opt Codes
	while ((opt = getopt(argc, argv, "t:n:q:a:")) != -1) {
		switch (opt) {
		case 't':
			timeStep = atoi(optarg);
			break;
		case 'n':
			threadcount = atoi(optarg);
			if (threadcount < 1)
				threadcount = 1;
			if (10 < threadcount)
				threadcount = 10;
			break;
		case 'q':
			time_quantum = atoi(optarg);
			break;
		case 'a':
			if (strcmp(optarg, "RR") == 0)
				algo = 1;
			else if (strcmp(optarg, "PRR") == 0)
				algo = 2;
			else if (strcmp(optarg, "SRTN") == 0)
				algo = 3;
			else
				algo = -1;
		}
	}

	//Parse user input
	while (i < threadcount) {
		yarn = parseUserInput(i + 1);
		list_append(li, yarn);
		i++;

//		printf(
//				"tNum: %d, Prio: %d, Starttime: %d, Runtime: %d, Total Time: %d\n",
//				yarn->tNumber, yarn->priority, yarn->startTime,
//				yarn->execRuntime, yarn->totalRuntime); //DEBUG
	}
	i = 0;
	//Applying the scheduling strategies
	switch (algo) {
	case 1:
		//a) Round-robin
		while (!isEmpty(li) || !isEmpty(queue)) {

			// Let the threads work
			yarnQueued = list_pop(queue);
			while (yarnQueued != NULL && k < time_quantum) {
				//Thread is done with its runtime, can be deleted
				if (yarnQueued->execRuntime >= yarnQueued->totalRuntime) {
					free(yarnQueued);
					yarnQueued = NULL;
					threadcount--;
					wasIdle = 0;
					break;
				}
				wasIdle = 0;
				print_time_step(time, yarnQueued->tNumber);
				time += timeStep;
				k += timeStep;
				yarnQueued->execRuntime += timeStep;
			}
			k = 0;

			// check for changed idle status
			int j = 0;
			int idleTcount = list_length(li);
			while (j < idleTcount) {
				j++;
				if ((yarn = list_pop(li)) == NULL)
					break;
				//If thread ready, insert in queue, otherwise, append back to idle
				if (yarn->startTime <= time)
					list_append(queue, yarn);
				else
					list_append(li, yarn);
			}

			if (yarnQueued != NULL)
				list_append(queue, yarnQueued);
			if (wasIdle && isEmpty(queue)) {
				print_time_step(time, 0);
				time += timeStep;
			}
			wasIdle = 1;
		}
		break;
	case 2:
		//b) Priority Round-robin
		while (!isEmpty(li) || !isEmpty(queue)) {

			// Let the threads work
			yarnQueued = list_pop(queue);
			while (yarnQueued != NULL && k < time_quantum) {
				//Thread is done with its runtime, can be deleted
				if (yarnQueued->execRuntime >= yarnQueued->totalRuntime) {
					free(yarnQueued);
					yarnQueued = NULL;
					threadcount--;
					wasIdle = 0;
					break;
				}
				wasIdle = 0;
				print_time_step(time, yarnQueued->tNumber);
				time += timeStep;
				k += timeStep;
				yarnQueued->execRuntime += timeStep;
			}
			k = 0;

			// check for changed idle status
			int j = 0;
			int idleTcount = list_length(li);
			while (j < idleTcount) {
				j++;
				if ((yarn = list_pop(li)) == NULL)
					break;
				//If thread ready, insert in queue, otherwise, append back to idle
				if (yarn->startTime <= time
						&& (isEmpty(queue)
								|| (yarnQueued != NULL
										&& yarn->priority
												<= yarnQueued->priority)))
					list_append(queue, yarn);
				else
					list_append(li, yarn);
			}

			if (yarnQueued != NULL)
				list_append(queue, yarnQueued);
			if (wasIdle && isEmpty(queue)) {
				print_time_step(time, 0);
				time += timeStep;
			}
			wasIdle = 1;
		}
		break;
	case 3:
		//c) Shortest Remaining Time Next

		break;
	default:
		printf("Error: Selected Algorithm not yet supported.");
	}

	list_finit(li);
	return 1;

}
