#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "memory.h"
#include "list.h"

struct op {
    enum { AL, FR} oper;
    int v;
};

void err(char *msg) {
    perror(msg);
    exit(-1);
}

int main(int argc, char *argv[]) {
    int c;
    int tsize = 256 * 4096;
    int bs = 4096;
    list_t *ops = list_init();
    enum mem_algo alg = none;
//    while ((c = getopt(argc, argv, "m:b:1|w(a:|f:)*")) != -1) {
    while ((c = getopt(argc, argv, "m:b:1|wa:f:")) != -1) {
        switch (c) {
            case 'm':
                tsize = atoi(optarg);
                break;
            case 'b':
                bs = atoi(optarg);
                break;
            case '1':
                if (alg != none) {
                    err("Multiple algorithms in input");
                }
                alg = first;
                break;
            case 'w':
                if (alg != none) {
                    err("Multiple algorithms in input1");
                }
                alg = worst;
                break;
            case 'a':
                0;
                struct op *nop = malloc(sizeof(struct op));
                nop->oper = AL;
                nop->v = atoi(optarg);
                list_append(ops, nop);
                break;
            case 'f':
                0;
                nop = malloc(sizeof(struct op));
                nop->oper = FR;
                nop->v = atoi(optarg);
                list_append(ops, nop);
                break;
        }
    }
    if (alg == none) {
        err("No Algorithm has been specified");
    }
    mem_init((unsigned int) tsize, (unsigned int) bs, alg);
    list_t *allocated = list_init();
    for (int i = 0; i < list_len(ops); i++) {
        struct op *ope = list_get(ops, i);
        if (ope->oper == AL) {
            list_append(allocated, mem_alloc((unsigned int) ope->v));
        }
        else
            mem_free(list_get(allocated, (ope->v - 1)));
    }

}