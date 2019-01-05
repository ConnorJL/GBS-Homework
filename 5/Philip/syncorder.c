#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include<sys/wait.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include "list.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int l;
int f;
int o;
long curr = 0;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int cmp(const void *i1, const void *i2) {
    return (int) i1 - (int) i2;
}

int write_buffer(long thread, char *buffer, int len) {
    if (o) {
        if (buffer == NULL && len == 0) {
            curr++;
            pthread_cond_broadcast(&cond);
        } else {
            while (thread != curr) {
                pthread_mutex_lock(&mutex);
                pthread_cond_wait(&cond, &mutex);
                pthread_mutex_unlock(&mutex);
            }
            return (int) write(STDOUT_FILENO, buffer, (size_t) len);
        }
    } else {
        return (int) write(STDOUT_FILENO, buffer, (size_t) len);
    }
    return -1;
}

void *startRoutine(void *k) {
//    float sto = *((float *) k);
    char filename[6];
    filename[0] = (char) (k + (int) '0');
    strcat(filename, ".txt");
    int fd = open(filename, O_RDONLY);
    int i = 0;

    char inp[65];
    char toWrite[10];
    for (int ii = 0; ii <= 64; ii++) {
        inp[ii] = '\0';
    }
    if (f)
        pthread_mutex_lock(&mutex);
    while (read(fd, inp, 64) != 0) {
        sprintf(toWrite, "[%02d] %03d\t", (int) k, i);
        if (l)
            pthread_mutex_lock(&mutex);
        write_buffer((long) k, toWrite, (int) strlen(toWrite));
        write_buffer((long) k, inp, (int) strlen(inp));
        write_buffer((long) k, "\n", (int) strlen("\n"));
        if (l)
            pthread_mutex_unlock(&mutex);
        i++;
        for (int ii = 0; ii < 64; ii++) {
            inp[ii] = '\0';
        }
    }
    if (o)
        write_buffer((long) k, NULL, 0);
    if (f)
        pthread_mutex_unlock(&mutex);
    pthread_exit((void *) 3);
}

int main(int argc, char *argv[]) {
    int c;
    l = 0;
    f = 0;
    o = 0;
    int n = 1;
    while ((c = getopt(argc, argv, "n:lfo")) != -1)
        switch (c) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'l':
                if (f)
                    exit(-1);
                l = 1;
                break;
            case 'f':
                if (l)
                    exit(-1);
                f = 1;
                break;
            case 'o':
                o = 1;
                break;
            default:
                printf("%c", (char) c);
                perror("Unknown argument type");
                exit(1);
        }
//    time_t t = time(NULL);
//    struct tm *tm = localtime(&t);
//    char s[64];
//    strftime(s, sizeof(s), "%c", tm);
//    printf("Start: %s\n", s);
    list_t *lt = list_init();
    pthread_t newPr = 0;

    srand(getpid());
    for (int i = 0; i < n; i++) {
        if (pthread_create(&newPr, NULL, startRoutine, (void *) i)) {
            fprintf(stderr, "Error creating thread\n");
        }
        list_insert(lt, (void *) newPr);
    }

    if (newPr != 0) {
        for (int i = 0; i < n; i++) {
            int er = pthread_join((pthread_t) list_first(lt), NULL);
            while (er != 0) {
                sleep(1);
                er = pthread_join((pthread_t) list_first(lt), NULL);
            }
            list_remove(lt, list_find(lt, list_first(lt), cmp));
        }
//        t = time(NULL);
//        tm = localtime(&t);
//        strftime(s, sizeof(s), "%c", tm);
//        printf("Ende: %s\n", s);
//        list_finit(lt);
    }
    exit(0);
}
