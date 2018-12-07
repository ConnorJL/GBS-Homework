#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>

#include "list.h"

list_t* parse(char* s);

// #define DEBUG

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
            // print_list(list);
            if(strcmp(s, "exit\n") == 0) {
                return 0;
            }
            else {
                #ifdef DEBUG
                    pid = 0;
                #else
                    pid = fork();
                #endif
                //pid = 0;
                if(pid == 0) {
                    char** args = list_to_array(list);
                    char* name = args[0];

                    if(strchr(name, '/') != NULL) {
                        char* last_slash_ptr = strrchr(name, '/');
                        int last_slash = last_slash_ptr - name;
                        char* new_name = (char*) malloc((strlen(name) - last_slash) * sizeof(char) +1);
                        new_name = strncpy(new_name, name+last_slash+1, strlen(name) - last_slash);
                        args[0] = new_name;
                        if(execve(name, args, envp) == -1) {
                            printf("ERROR");
                            return -1;
                        }
                    }

                    // Parse PATH
                    list_t* path_list = list_init();
                    char* ptr;

                    char* PATH = malloc(strlen(getenv("PATH")));
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
                    printf("ERROR");
                    return -1;
                }
                else {
                    list_append(pids, (char*) &pid);
                    wait(NULL);
                    fflush(stdout);
                }
            }
            
        }
        
    }


    return 0;
}
