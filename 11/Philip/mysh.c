#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <dirent.h>
#include "list.h"
#include "wildcard.h"

list_t *parse_inp(char *inp, char *envp[]);

void printer(void *p) {
    printf("inlist: %s\n", (char *) p);
}

char *GetPWD(char *envp[]) {
    list_t *r;
    r = parse_inp("$PWD", envp);
    char *data = (char *) r->first->data;
    list_finit(r);
    free(r);
    return data;
}

int filter_dir(char *op, char *dir) {
//    if (strcmp(op, "*") == 0) {
//        return 1;
//    }
//    if (op[0] == '*') {
//        return star_x_filter(dir, op);
//    }
//    return x_star_filter(dir, op);
    int ret = x_star_x_filter(dir, op);
//    printf("test %s and %s returned %d\n", op, dir, ret);
    return ret;
}

//char *getStringOfPoss(op, op_len, malloc(1024));

//char **getPoss(char **op, int op_len, char **acc, int i) {
//    if (op_len == 0) {
//        return acc;
//    } else {
//        char *star_pos = strchr(op[i], '*');
//        if (star_pos != NULL) {
//            struct dirent **dirs = NULL;
//            int dir_len;
//            dir_len = scandir(getcwd(NULL, 0), &dirs, NULL, alphasort);
//            int p = 0;
//            for (int dir_nr = 0; dir_nr < dir_len; dir_nr++) {
//                char *dir_name = dirs[dir_nr]->d_name;
//                if (dir_name[0] == '.' || filter_dir(op[i], dir_name) == 0) {
//                    continue;
//                }
//                p = fork();
//                if (dirs == NULL) {
//                    printf("dir is null\n");
//                }
//                if (p != 0) {
//                    op[i] = dir_name;
//                    break;
//                }
//            }
//            if (p == 0) {
//                exit(0);
//            }
//        }
//    }
//}

void exec2(char *path, list_t *out, char **envp) {

    for (int j = 1; j < list_len(out); j++) {
        char *star_pos = strchr(list_get(out, j), '*');
        if (star_pos == NULL) {
            continue;
        }

        char *w_star = list_remove(out, list_get_p(out, j));
        struct dirent **dirs = NULL;

        int dir_len = scandir(getcwd(NULL, 0), &dirs, NULL, alphasort);

        for (int dir_nr = 0; dir_nr < dir_len; dir_nr++) {
            char *dir_name = dirs[dir_nr]->d_name;
            if (dir_name[0] == '.' || filter_dir(w_star, dir_name) == 0) {//
                continue;
            }

            list_put(out, list_get_p(out, j - 1), dir_name);
            j++;
        }
    }

    char **outa = (char **) list_to_array(out);
    execve(path, outa, envp);
}

void exec(char *path, char **op, char **envp, int op_len) {
    for (int j = 1; j < op_len; j++) {
//        char *star_pos = strchr(op[i], '*');
//        if (star_pos != NULL) {
//            struct dirent **dirs = NULL;
//            int dir_len;
//            dir_len = scandir(getcwd(NULL, 0), &dirs, NULL, alphasort);
//            int p = 0;
//            for (int dir_nr = 0; dir_nr < dir_len; dir_nr++) {
//                char *dir_name = dirs[dir_nr]->d_name;
//                if (dir_name[0] == '.' || filter_dir(op[i], dir_name) == 0) {
//                    continue;
//                }
//                p = fork();
//                if (dirs == NULL) {
//                    printf("dir is null\n");
//                }
//                if (p != 0) {
//                    op[i] = dir_name;
//                    break;
//                }
//            }
//            if (p == 0) {
//                exit(0);
//            }
//        }
        char *star_pos = strchr(op[j], '*');
        if (star_pos == NULL) {
            continue;
        }

        struct dirent **namelist;
        char *element;
        char *result = (char *) malloc(sizeof(char));
        result[0] = '\0';
        int oldsize;
        int newsize;
        int n;
        int i = 0;

        n = scandir(getcwd(NULL, 0), &namelist, NULL, alphasort);
        if (n < 0)
            perror("scandir");
//        printf("nice3");
        while (i < n) {
            element = strdup(namelist[i]->d_name);
            free(namelist[i++]);
//            printf("nice3a");
            if (element[0] == '.' || filter_dir(op[j], element) == 0) //skip . .. and files starting with .
                continue;
//            printf("nice4a");
            oldsize = (int) strlen(result);
            newsize = oldsize + (int) strnlen(element, 256) +
                      2; // +2 because of terminating NULL char and space bewtween elements
            result = (char *) realloc(result, (size_t) newsize);
            if (oldsize != 0) //skips the first iteration
                strcat(result, " ");
            strcat(result, element);
        }
        free(namelist);
        op[j] = result;
//        printf("\nres: %s\n", result);
    }
//    getStringOfPoss(op, op_len, malloc(1024));
//    getPoss(op, op_len, malloc(sizeof(op)), 0);
//    printf("nice7");
    execve(path, op, envp);
}

int main(int argc, char *argv[], char *envp[]) {
    char inp[1024];
    inp[1023] = '\0';
    list_t *out;
//    printf("DD%s\n", envp[0]);
    setvbuf(stdout, NULL, _IONBF, 0);
    while (1) {
//        printf("%s $ ", (char *) pwd->first->data);
        printf("%s $ ", getcwd(NULL, 0));
        fgets(inp, 1023, stdin);
        inp[strcspn(inp, "\n")] = 0;
        if (strncmp(inp, "exit", 1024) == 0) {
//            write(STDOUT_FILENO, "\n", 2);
            break;
        }
        // printf(inp);
        out = parse_inp(inp, envp);
        int llen = list_len(out);
        if (llen < 1) {
            perror("Too little arguments!");
            return -1;
        }
        char **outa = (char **) list_to_array(out);
//        for (int ti = 0; ti < list_len(out); ti++) {
//            printf("%d:%s\n", ti + 1, (char *)(list_get(out, ti)));
//        }
        int p;
//        list_t *procs = list_init();
        if (strcmp(outa[0], "cd") == 0) {
            int cdlen = list_len(out);
            if (cdlen > 2) {
                printf("Usage: cd <dir>\n");
            } else if (cdlen == 1) {
                printf("%s\n", getcwd(NULL, 0));
            } else { // cdlen == 2
                if (chdir(outa[1]) >= 0) {
                    setenv("PWD", getcwd(NULL, 0), 1);
                } else {
                    printf("Error when trying to change path %s\n", outa[1]);
                    strerror(1);
                }
            }
        } else {
            if ((p = fork()) == 0) {
                // child process
                if (strchr(outa[0], '/')) {
                    exec2(outa[0], out, envp);
//                    exec(outa[0], outa, envp, list_len(out));
                    printf("exec returned, meaning the filename was wrong");
                    return -1;
                } else {
//                char *path = (char *)(parse_inp("$PATH", envp)->first->data);
//                char *cp = path;
//                cp[strchr]
//                int c = 0;
//                char *path = (char *) (parse_inp("$PATH", envp)->first->data);
                    char *paths = (char *) (parse_inp("$PATH", envp)->first->data);
                    char *path = strtok(paths, ":");
//                char *nenv = {path, NULL};
//                exec(outa[0], outa, nenv);
                    while (path != NULL) {
                        char *oldPath = malloc((strlen(path) + 1) * sizeof(char));
                        oldPath[0] = '\0';
                        strcat(oldPath, path);
                        path = malloc((strlen(path) + strlen(outa[0]) + 4) * sizeof(char));
                        path[0] = '\0';
                        strcat(path, oldPath);
                        strcat(path, "/");
                        strcat(path, outa[0]);
                        exec2(path, out, envp);
//                        exec(path, outa, envp, list_len(out));
                        path = strtok(NULL, ":");
                    }
                    printf("No valid path found!\n");
                    list_finit(out);
                    free(out);
                    free(outa);
//                list_finit(procs);
//                free(procs);
                    return -1;
                }
            } else {
//            list_append(procs, &p);
                int returnStatus;
                waitpid(p, &returnStatus, 0);
                list_finit(out);
                free(out);
                free(outa);
//            list_finit(procs);
//            free(procs);
                if (returnStatus != 0) {
                    return -1;
                }
            }
        }
//        list_print(out, printer);

        //system(inp);
    }
    return 0;
}