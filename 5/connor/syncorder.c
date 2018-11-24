#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include "list.h"

int f = 0;
int l = 0;
int o = 0;
int current_thread = 0;
pthread_cond_t cond;
pthread_mutex_t mutex;

int compare_int(const char* a, const char* b) {
    if(a == b) {
        return 0;
    }
    return 1;
}

struct thread_args {
    int id;
};

int write_buffer(long thread, char *buffer, int len) {
    pthread_mutex_lock(&mutex);
    while(current_thread != thread) {
        pthread_cond_wait(&cond, &mutex);
    }
    write(1, buffer, len);
    pthread_mutex_unlock(&mutex);
}

void* run(void* args) {
    struct thread_args* arg = (struct thread_args*) args;
    int id = arg->id;

    char* fi = (char*) malloc(sizeof(char)*64);
    char* output = (char*) malloc(sizeof(char)*128);
    sprintf(fi, "%i.txt", id);
    int filedesc = open(fi, O_RDONLY);

    int i = 0;
    int done = 0;
    int result;
    char* s = (char*) malloc(sizeof(char)*64);

    if(l == 1) {
        while(done != 1) {
            result = read(filedesc, s, 64);
            if(result == 0) {
                break;
            }
            if(result < 64) {
                done = 1;
            }
            
            pthread_mutex_lock(&mutex);
            sprintf(output, "[%02d] %03d\t", id, i);
            write(1, output, strlen(output));
            write(1, s, result);
            write(1, "\n", 1);
            pthread_mutex_unlock(&mutex);
            sleep(1);
            i++;
        }
    }
    else if(f == 1) {
        pthread_mutex_lock(&mutex);
        while(done != 1) {
            result = read(filedesc, s, 64);
            if(result == 0) {
                break;
            }
            if(result < 64) {
                done = 1;
            }
            
            sprintf(output, "[%02d] %03d\t", id, i);
            write(1, output, strlen(output));
            write(1, s, result);
            write(1, "\n", 1);
            sleep(1);
            i++;
        }
        pthread_mutex_unlock(&mutex);
    }
    else if(o == 1) {
        while(done != 1) {
            result = read(filedesc, s, 64);
            if(result == 0) {
                break;
            }
            if(result < 64) {
                done = 1;
            }
            
            sprintf(output, "[%02d] %03d\t", id, i);
            write_buffer(id, output, strlen(output));
            write_buffer(id, s, result);
            write_buffer(id, "\n", 1);
            sleep(1);
            i++;
        }
        current_thread++;
        pthread_cond_broadcast(&cond);
    }
    

    pthread_exit(0);
}

int main(int argc, char const *argv[])
{
    int n = 1;

    int c;
    while( (c=getopt(argc, argv, "n:rlfo")) != -1) {
        switch(c) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'l':
                l = 1;
                break;
            case 'f':
                f = 1;
                break;
            case 'o':
                o = 1;
                break;
        }
    }

    if(n > 10) {
        n = 10;
    }

    if(l + f + o != 1) {
        printf("Must set either l, f or o!\n");
        return -1;
    }
    
    list_t* list = list_init();
    //printf("Start: %s", ctime(&now));
    pthread_t thread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    struct thread_args args[n];

    for(int i=0; i<n; i++) {
        args[i].id = i;

        pthread_create(&thread, NULL, run, &args[i]);
        list_append(list, thread);
    }

    struct list_elem* elem;
    while(list->first != NULL) {
        thread = list->first->data;
        pthread_join(thread, NULL);
        elem = list_find(list, thread, compare_int);
        if(list_remove(list, elem) == -1) {
            break;
        }
    }

    //printf("Ende: %s", ctime(&now));

    return 0; 
}
