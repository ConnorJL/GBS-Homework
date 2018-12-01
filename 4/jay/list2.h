/*
 * list.h
 *
 *  Created on: 03.11.2018
 *      Author: jobrm
 */

struct tData {
	pthread_t data;	// Zeiger auf ein Datenobject
	int tNumber;
	int priority;
	int startTime;
	int execRuntime;
	int totalRuntime;
};

struct list_elem {
	struct list_elem *next;	// Zeiger auf das naechste Element
	struct tData *data;
};

typedef struct list {
	struct list_elem *first;	// erstes Element in der Liste
	struct list_elem *last;	// letztes Element in der Liste
} list_t;

/* function prototypes */
list_t *list_init();
struct list_elem *list_insert(list_t *list, struct tData *data);
struct list_elem *list_append(list_t *list, struct tData *data);
int list_remove(list_t *list, struct list_elem *elem);
void list_finit(list_t *list);
void list_print(list_t *list, void (*print_elem)(struct tData *));
struct list_elem *list_find(list_t *list, struct tData *data,
		int (*cmp_elem)(const struct tData *, const struct tData *));
void print_data(struct tData *data);
struct tData *list_pop(list_t *list);
int isEmpty(list_t *list);
int list_length(list_t *list);


