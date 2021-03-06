#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h> 

#include "list.h"

list_t* parse(char* s);

//#define DEBUG

void print_list(list_t *t) {
    struct list_elem *current = t->first;
    int i = 1;
    while (current != NULL) {
        printf("%i:%s\n", i, current->data);
        current = current->next;
        i++;
    }
}

int list_len(list_t* list) {
    int i = 0;
    struct list_elem* current = list->first;

    while(current != NULL) {
        i++;
        current = current->next;
    }
    return i;
}

int list_args_len(list_t* list) {
    int i = 0;
    struct list_elem* current = list->first;

    while(current != NULL) {
        if(strcmp(current->data, "<") == 0 || strcmp(current->data, ">") == 0) {
            return i;
        }
        i++;
        current = current->next;
    }
    return i;
}

char** list_to_array(list_t* list) {
    int len = list_len(list) + 1;
    char** out = (char**) malloc(sizeof(char*) * len);
    struct list_elem* current = list->first;

    for(int i=0; i < len-1; i++) {
        out[i] = current->data;
        current = current->next;
    }
    out[len-1] = NULL;

    return out;
}

char** list_to_args_array(list_t* list) {
    int len = list_args_len(list) + 1;
    char** out = (char**) malloc(sizeof(char*) * len);
    struct list_elem* current = list->first;

    for(int i=0; i < len-1; i++) {
        out[i] = current->data;
        current = current->next;
    }
    out[len-1] = NULL;

    return out;
}

char* get_output_file(list_t* list) {
    struct list_elem* current = list->first;

    while(current != NULL) {
        if(strcmp(current->data, ">") == 0) {
            if(current->next != NULL) {
                return current->next->data;
            }
            return NULL;
        }
        current = current->next;
    }
    return NULL;
}

char* get_input_file(list_t* list) {
    struct list_elem* current = list->first;

    while(current != NULL) {
        if(strcmp(current->data, "<") == 0) {
            if(current->next != NULL) {
                return current->next->data;
            }
            return NULL;
        }
        current = current->next;
    }
    return NULL;
}

int find_pipe(list_t* list) {
    int i = 0;
    struct list_elem* current = list->first;

    while(current != NULL) {
        if(strcmp(current->data, "|") == 0) {
            return i;
        }
        i++;
        current = current->next;
    }
    return -1;
}

char*** split_args(list_t* list) {
    int pipe_position = find_pipe(list);
    int len = pipe_position + 1;
    char** out1 = (char**) malloc(sizeof(char*) * len);
    struct list_elem* current = list->first;

    for(int i=0; i < len-1; i++) {
        out1[i] = current->data;
        current = current->next;
    }
    out1[len-1] = NULL;

    // Second half
    len = list_len(list) - pipe_position;
    char** out2 = (char**) malloc(sizeof(char*) * len);
    current = list->first;
    for(int i=0; i < pipe_position; i++) {
        current = current->next;
    }

    current = current->next;
    for(int i=0; i < len-1; i++) {
        out2[i] = current->data;
        current = current->next;
    }
    out2[len-1] = NULL;


    char*** out = (char***) malloc(sizeof(char**) * 2);
    out[0] = out1;
    out[1] = out2;
    return out;
}

void wait_on_list(list_t* list) {
    int pid;
    while(list->first != NULL) {
        pid = list->first->data;
        waitpid(pid, NULL, 0);
        list_remove(list, list->first);
    }
}

void kill_processes(list_t* list) {
    int pid;
    while(list->first != NULL) {
        pid = list->first->data;
        kill(pid, SIGKILL);
        list_remove(list, list->first);
    }
}
    

int main(int argc, char const *argv[], char *envp[])
{
    char s[1024];
    list_t* pids = list_init();
    int pid;

    while(1) {
        printf("$ ");
        fflush(stdout);
        if(fgets(s, 1024, stdin) != NULL) {
            list_t* list = parse(s);
            if(list == NULL) {
                printf("Invalid input!\n");
                continue;
            }
            // print_list(list);
            if(strcmp(s, "exit\n") == 0) {
                return 0;
            }
            else {
                char* input_file = get_input_file(list);
                char* output_file = get_output_file(list);
                int has_pipe = find_pipe(list);
                char** args;
                char* name;
                int id = -1;
                int pipes[2];
                if(has_pipe == -1) {
                    args = list_to_args_array(list);
                    #ifdef DEBUG
                        pid = 0;
                    #else
                        pid = fork();
                    #endif
                    if(pid != 0) {
                        int pid_copy = malloc(sizeof(int));
                        pid_copy = pid;
                        list_append(pids, pid_copy);
                    }
                }
                else {
                    // We have a pipe, need to start two processes
                    int pid2;
                    char*** args_both = split_args(list);
                    pipe(pipes);
                    pid = fork();
                    if(pid == 0) {
                        args = args_both[0];
                        id = 0;
                    } else {
                        int pid_copy = malloc(sizeof(int));
                        pid_copy = pid;
                        list_append(pids, pid_copy);
                    }
                    pid2 = fork();
                    if(pid2 == 0) {
                        args = args_both[1];
                        id = 1;
                        pid = 0;
                    } else {
                        int pid_copy = malloc(sizeof(int));
                        pid_copy = pid2;
                        list_append(pids, pid_copy);
                    }
                }
                if(pid == 0) {
                    if(input_file != NULL) {
                        close(0);
                        open(input_file, O_RDONLY);
                    }
                    if(output_file != NULL) {
                        close(1);
                        open(output_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
                    }

                    if(has_pipe != -1) {
                        if(id == 0) {
                            printf("Reached 1\n");
                            fflush(1);
                            dup2(1, pipes[1]);
                            close(pipes[0]);
                        }
                        else if(id == 1) {
                            printf("Reached 2\n");
                            fflush(1);
                            dup2(0, pipes[0]);
                            close(pipes[1]);
                        }
                    }

                    name = args[0];

                    if(strchr(name, '/') != NULL) {
                        char* last_slash_ptr = strrchr(name, '/');
                        int last_slash = last_slash_ptr - name;
                        char* new_name = (char*) malloc((strlen(name) - last_slash) * sizeof(char) +1);
                        new_name = strncpy(new_name, name+last_slash+1, strlen(name) - last_slash);
                        args[0] = new_name;
                        if(execve(name, args, envp) == -1) {
                            printf("ERROR\n");
                            kill_processes(pids);
                            return -1;
                        }
                    }

                    // Parse PATH
                    list_t* path_list = list_init();
                    char* ptr;

                    char* PATH = malloc(strlen(getenv("PATH")) + 1);
                    strcpy(PATH, getenv("PATH"));
                    ptr = strtok(PATH, ":");
                    int len;
                    while(ptr != NULL) {
                        len = strlen(ptr);
                        char* buffer = (char*) malloc(len * sizeof(char)+1);
                        strncpy(buffer, ptr, len+1);
                        list_append(path_list, buffer);
                        ptr = strtok(NULL, ":");
                    }

                    // print_list(path_list);

                    char** paths = list_to_array(path_list);
                    int num_paths = list_len(path_list);
                    // Iterate through paths
                    char path[1024];
                    for(int i=0; i<num_paths; i++) {
                        char* cur_path = paths[i];
                        snprintf(path, 1024, "%s/%s", cur_path, name);
                        if(execve(path, args, envp) == -1) {
                            continue;
                        }
                        else {
                            return 0;
                        }
                        
                    }
                    kill_processes(pids);
                    printf("ERROR\n");
                    return -1;
                }
                else {
                    if(id == -1 && has_pipe != -1) {
                        close(pipes[0]);
                        close(pipes[1]);
                    }
                    wait_on_list(pids);
                    fflush(stdout);
                }
            }
            
        }
        
    }
    return 0;

}