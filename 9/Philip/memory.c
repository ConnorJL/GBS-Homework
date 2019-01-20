#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"

enum mem_algo { none, first , worst };

struct memblock {
    enum {
        FREE, USED
    } status; // ist der Speicherblock frei
    unsigned int size; // Laenge des Speicherblocks
    unsigned int in_use; // Anteil zugewiesener Speicher; 0 wenn status == FREE ∗/
    void *addr; // Adresse des Speicherblocks
};

enum mem_algo alg;
unsigned int bs;
//unsigned int s;
list_t *blocks;

void printer(void *arg) {
    struct memblock *mb = (struct memblock *)arg;
    char pf = 'P';
    if (mb->status == FREE) {
        pf = 'F';
    }
    printf("[%p %c %d/%d] ", mb->addr, pf, mb->in_use, mb->size);
}

void mem_dump () {
    list_print(blocks, printer);
    printf("\n");
}

int mem_init (unsigned int size, unsigned int blocksize, enum mem_algo strategy) {
    alg = strategy;
    bs = blocksize;
    blocks = list_init();
    struct memblock *mb = malloc(sizeof(struct memblock));
    mb->status = FREE;
    mb->size = size;
    mb->in_use = 0;
    mb->addr = mb;
    list_append(blocks, mb);
    mem_dump();
    return 0;
}

void *mem_alloc (unsigned int size) {
    void *ret = NULL;
    if (alg == first) {
        struct list_elem *c = blocks->first;
        while(1) {
            if (c == NULL) {
                printf("Not enough memory to allocate size %d.\n", size);
                exit(-1);
            }
            struct memblock *m = (struct memblock *)c->data;
            if (m->status == USED || m->size < size) {
                c = c->next;
            }
            else {
                void *n = m->addr;
                m->status = USED;
                m->in_use = size;
                int old_size = m->size;
                m->size = ((size - 1) / bs + 1) * bs;
                if (old_size - size >= bs) {
                    struct memblock *nmb = malloc(sizeof(struct memblock));
                    nmb->status = FREE;
                    nmb->size = old_size - m->size;
                    nmb->in_use = 0;
                    nmb->addr = nmb; //TODO: what is addr?
                    list_put(blocks, c, nmb);
                }
                ret = n;
                break;
            }
        }
    } else {
        struct memblock *bblock;
        struct list_elem *c = blocks->first;
        struct memblock *m;
        while (1) {
            if (c == NULL) {
                break;
            }
            m = (struct memblock *)c->data;
            if (m->status == FREE && (bblock == NULL || m->size > bblock->size)) {
                bblock = m;
            }
            if (c->next == NULL) {
                break;
            }
            c = c->next;
        }
        if (bblock == NULL || bblock->size < size) {
            printf("Not enough memory to allocate size %d.\n", size);
            exit(-1);
        }
        m = bblock;
        void *n = m->addr;
        m->status = USED;
        m->in_use = size;
        int old_size = m->size;
        m->size = ((size - 1) / bs + 1) * bs;
        if (old_size - size >= bs) {
            struct memblock *nmb = malloc(sizeof(struct memblock));
            nmb->status = FREE;
            nmb->size = old_size - m->size;
            nmb->in_use = 0;
            nmb->addr = nmb; //TODO: what is addr?
            list_put(blocks, c, nmb);
        }
        ret = n;
    }
    mem_dump();
    return ret;
}
void mem_free (void *addr) {
    int llen = list_len(blocks);
    for (int i = 0; i < llen; i++) {
        struct memblock *mb = list_get(blocks, i);
        if (mb->addr == addr) {
            if (mb->status == USED) {
                unsigned int nsize = mb->size;
                if (i + 1 < llen) {
                    struct memblock *nme = (struct memblock *)(list_get(blocks, i+1));
                    int llen = list_len(blocks);
                    if (nme->status == FREE) {
                        nsize += nme->size;
                        list_rem_i(blocks, i+1);
                    }
                }
                if (i != 0) {
                    struct memblock *nme = (struct memblock *)(list_get(blocks, i-1));
                    if (nme->status == FREE) {
                        list_rem_i(blocks, i);
                        nme->size = nsize + nme->size;
                        mem_dump();
                        return;
                    }
                }
                mb->size = nsize;
                mb->status = FREE;
                mb->in_use = 0;
                mem_dump();
                return;
            } else {
                printf("Memblock that should be freed is not in use: %p.\n", addr);
                mem_dump();
                return;
            }
        }
    }
    mem_dump();
}
