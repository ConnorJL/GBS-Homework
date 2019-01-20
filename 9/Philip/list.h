struct list_elem {
    struct list_elem *next;	// Zeiger auf das naechste Element
    int	     *data;	// Zeiger auf ein Datenobject
};

typedef struct list {
	struct list_elem *first;// erstes Element in der Liste
	struct list_elem *last;	// letztes Element in der Liste
} list_t;

typedef struct thread_data {
    int num;
    int prio;
    int start;
    int time_runned;
    int end;
    int inQueue;
} thread_data;

/* function prototypes */
struct thread_data      *t_data_init(int num, int prio, int start, int time_runned);
list_t           *list_init ();
int              list_len(list_t *list);
void            *list_get(list_t *list, int ind);
struct list_elem *list_insert (list_t *list, void *data);
struct list_elem *list_append (list_t *list, void *data);
void *              list_remove (list_t *list, struct list_elem *elem);
void *           list_rem_i (list_t *list, int i);
void             list_finit (list_t *list);
void *              list_first (list_t *list);
void             list_print (list_t *list, void (*print_elem) (void *));
void 			 list_dump (list_t *list, void (*print_elem) (struct list_elem *));
struct list_elem *list_find (list_t *list, void *data, int (*cmp_elem) (const void *, const void *));
void **			list_to_array(list_t *list);
struct list_elem list_put (list_t *list, struct list_elem *current, void *data);




