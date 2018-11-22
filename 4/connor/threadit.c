#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "list.h"

int compare_int(const char* a, const char* b) {
    if(a == b) {
        return 0;
    }
    return 1;
}

void run(int k) {
    for(int i = 1; i<=k; i++) {
        printf("%10u\t%d\t%ld\n", (unsigned int) pthread_self(), getpid(), i);
        sleep(1);
    }
}

int main(int argc, char const *argv[])
{
    int k = 10;
    int n = 1;
    int r = 0;

    int c;
    while( (c=getopt(argc, argv, "k:n:r")) != -1) {
        switch(c) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'n':
                n = atoi(optarg);
                break;
            case 'r':
                r = 1;
                break;
        }
    }
    
    list_t* list = list_init();
    time_t now;
    time(&now);
    srand((unsigned) &now);
    printf("Start: %s", ctime(&now));
    pthread_t thread;

    for(int i=0; i<n; i++) {
        if(r == 1) {
            time_t t;
            k = k/2 + ( rand() / (RAND_MAX / (k*1.5-k/2 + 1) + 1));
        }

        pthread_create(&thread, NULL, run, k);
        list_append(list, thread);
    }


    int stat;
    struct list_elem* elem;

    while(list->first != NULL) {
        thread = list->first->data;
        pthread_join(thread, NULL);
        elem = list_find(list, thread, compare_int);
        if(list_remove(list, elem) == -1) {
            break;
        }
    }

    printf("Ende: %s", ctime(&now));

    return 0; 
}
