/*
 * list.c
 *
 *  Created on: 03.11.2018
 *      Author: jobrm
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "list2.h"

list_t *list_init() {

//	list_t *list = malloc(2 * sizeof(struct list_elem *));
	list_t *list = malloc(sizeof(list_t));
	if (list == NULL)
		return NULL;
	list->first = NULL;
	list->last = NULL;

	return list;
}

struct list_elem *list_insert(list_t *list, struct tData *data) {
	//NULL-List
	if (list == NULL)
		return NULL;

	struct list_elem *li_el = malloc(sizeof(struct list_elem));
	li_el->data = data;

	if (list->first != NULL)
		li_el->next = list->first;
	else {
		li_el->next = NULL;
		list->last = li_el;
	}
	list->first = li_el;

	return li_el;
}

struct list_elem *list_append(list_t *list, struct tData *data) {
	//NULL-List
	if (list == NULL)
		return NULL;

	struct list_elem *li_el = malloc(sizeof(struct list_elem));
	li_el->data = data;
	li_el->next = NULL;
	if (list->last != NULL)
		list->last->next = li_el;
	else
		list->first = li_el;

	list->last = li_el;

	return li_el;
}

// Removes and Returns the first Element of the list
struct tData *list_pop(list_t *list) {
	//NULL-List or empty List
	if (list == NULL || list->first == NULL)
		return NULL;

	struct list_elem *li_el = list->first;

	if (li_el->next == NULL) {
		list->first = NULL;
		list->last = NULL;
	} else
		list->first = li_el->next;
	struct tData *yarn = li_el->data;
	free(li_el);
	return yarn;
}

// Removes and Returns the first Element of the list
struct list_elem *list_getFirst(list_t *list) {
	//NULL-List or empty List
	if (list == NULL || list->first == NULL)
		return NULL;

	return list->first;
}

int list_remove(list_t *list, struct list_elem *elem) {

	//NULL-List
	if (list == NULL)
		return -1;

	struct list_elem *li_el = list->first;
	//Empty List
	if (li_el == NULL)
		return -1;
	//check if the first entry matches
	if (li_el == elem) {
		list->first = li_el->next;
		free(li_el);
//		free(elem);
		return 1;
	}

	//check if the next entries matches
	struct list_elem *li_el_next = li_el->next;
	while (li_el_next != NULL) {

		if (li_el_next == elem) {
			li_el->next = li_el_next->next;
			free(li_el_next);
//			free(elem);
			return 1;
		}
		li_el = li_el_next;
		li_el_next = li_el->next;
	}

	return -1;
}

void list_finit(list_t *list) {

	if (list == NULL)
		return;

	struct list_elem *li_el = list->first;
	struct list_elem *li_el_next;
	if (li_el == NULL) {
		free(li_el);
		return;
	} else
		li_el_next = li_el->next;

	while (1) {

		free(li_el);
		li_el = li_el_next;
		if (li_el == NULL) {
			free(li_el);
			return;
		} else
			li_el_next = li_el->next;
	}
	free(list);
}

struct list_elem *list_find(list_t *list, struct tData *data,
		int (*cmp_elem)(const struct tData *, const struct tData *)) {

	if (list == NULL)
		return NULL;
	struct list_elem *li_el = list->first;

	while (li_el != NULL) {

		if (li_el->data == data) {
			return li_el;
		}
		li_el = li_el->next;
	}

	return NULL;
}

void print_data(struct tData *data) {
	printf("%ld", (long int) data->data);
}

void list_print(list_t *list, void (*print_elem)(struct tData *)) {
	if (list == NULL)
		return;
	struct list_elem *li_el = list->first;
	int i = 1;

	while (li_el != NULL) {
		printf("%i:", i);
		print_elem(li_el->data);
		printf("\n");
		li_el = li_el->next;
		i++;
	}
}

int isEmpty(list_t *list) {
	if(list == NULL)
		return -1;
	return list->first == NULL && list->last == NULL;
}

int list_length(list_t *list){
	if (list == NULL || list->first == NULL)
			return 0;
	int length = 0;
	struct list_elem *li_el = list->first;
	while(li_el != NULL){
		li_el = li_el->next;
		length++;
	}
	return length;
}





