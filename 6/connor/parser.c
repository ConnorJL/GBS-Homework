#include <string.h>
#include <stdlib.h>

#include "list.h"

#define NORMAL 0
#define SPACE 1
#define DOUBLEQUOTES 2
#define QUOTES 3

void append(char* s, char c) {
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}
/*
int expand_variable(char* buffer, int start, int buffer_len) {
    int len = strlen(buffer) - start;
    char* substring = (char*) malloc(len*sizeof(char));
    strncpy(substring, buffer+start, len);
    char* var = getenv(substring);

    int var_len = strlen(var);
    buffer = (char*) realloc(buffer, buffer_len + var_len);

    strcpy(buffer+start, var);

    return buffer_len + var_len;
}
*/
char* add_buffer_to_list(list_t* l, char* buffer) {
    if(buffer[0] == '\0') {
        return buffer;
    }
    char* new;
    
    new = (char*) malloc(strlen(buffer) * sizeof(char));
    strcpy(new, buffer);

    list_append(l, new);

    free(buffer);
    buffer = (char*) malloc(1024);
    buffer[0] = '\0';
    return buffer;
}

list_t* parse(char *s) {
    list_t *l = list_init();
    int state = NORMAL;
    int len = strlen(s);
    int i = 0;
    char c;
    char* buffer = (char*) malloc(1024);
    buffer[0] = '\0';
    int var_start = 0;
    int dollar = 0;
    int buffer_len = 1024;

    while(i < len) {
        c = s[i];

        if(c == ' ') {
            if(state == SPACE) {
                // Multiple spaces
                i++;
                continue;
            }
            else if (state == DOUBLEQUOTES || state == QUOTES) {
                if(dollar == 1) {
                    // Expand variable
                    int len = strlen(buffer) - var_start;
                    char* substring = (char*) malloc(len*sizeof(char));
                    strncpy(substring, buffer+var_start, len);
                    char* var = getenv(substring);
                    free(substring);
                    int var_len = strlen(var);
                    buffer_len = buffer_len + var_len;
                    buffer = (char*) realloc(buffer, buffer_len);
                    strcpy(buffer+var_start, var);

                    dollar = 0;
                }
                // Space within quotes
                append(buffer, ' ');
                i++;
                continue;
            }
            else {
                // State is normal or DOLLAR, add buffer to list
                if(dollar == 1) {
                    // Expand variable
                    int len = strlen(buffer) - var_start;
                    char* substring = (char*) malloc(len*sizeof(char));
                    strncpy(substring, buffer+var_start, len);
                    char* var = getenv(substring);
                    free(substring);
                    int var_len = strlen(var);
                    buffer_len = buffer_len + var_len;
                    buffer = (char*) realloc(buffer, buffer_len);
                    strcpy(buffer+var_start, var);
                    dollar = 0;
                }
                buffer = add_buffer_to_list(l, buffer);
                state = SPACE;
                i++;
                continue;
            }
        }

        if(c == '\'') {
            if(state == QUOTES) {
                state = NORMAL;
                i++;
                continue;
            }
            else if(state == DOUBLEQUOTES) {
                append(buffer, '\'');
                i++;
                continue;
            }
            else {
                state = QUOTES;
                i++;
                continue;
            }
        }

        if(c == '"') {
            if(state == DOUBLEQUOTES) {
                state = NORMAL;
                i++;
                continue;
            }
            else if(state == QUOTES) {
                append(buffer, '"');
                i++;
                continue;
            }
            else {
                state = DOUBLEQUOTES;
                i++;
                continue;
            }
        }

        if(c == '$') {
            if(dollar == 1) {
                // Expand variable
                int len = strlen(buffer) - var_start;
                char* substring = (char*) malloc(len*sizeof(char));
                strncpy(substring, buffer+var_start, len);
                char* var = getenv(substring);
                free(substring);
                int var_len = strlen(var);
                buffer_len = buffer_len + var_len;
                buffer = (char*) realloc(buffer, buffer_len);
                strcpy(buffer+var_start, var);
                dollar = 0;
                continue;
            }
            var_start = strlen(buffer);
            dollar = 1;
            i++;
            continue;
        }

        if(c == '\\') {
            if(state == SPACE) {
                state = NORMAL;
            }
            if(i+1 < len) {
                append(buffer, s[i+1]);
                i = i+2;
                continue;
            }
            else {
                i++;
                continue;
            }
        }

        if(c == '\n') {
            if(dollar == 1) {
                // Expand variable
                int len = strlen(buffer) - var_start;
                char* substring = (char*) malloc(len*sizeof(char));
                strncpy(substring, buffer+var_start, len);
                char* var = getenv(substring);
                free(substring);
                int var_len = strlen(var);
                buffer_len = buffer_len + var_len;
                buffer = (char*) realloc(buffer, buffer_len);
                strcpy(buffer+var_start, var);
                dollar = 0;
            }
            buffer = add_buffer_to_list(l, buffer);
            break;
        }

        // Normal character
        if(dollar == 1) {
            if((c > 64 && c < 91) || c == 95) {
                // Allowed var character
                append(buffer, c);
                i++;
                continue;
            }
            else {
                // Expand variable
                int len = strlen(buffer) - var_start;
                char* substring = (char*) malloc(len*sizeof(char));
                strncpy(substring, buffer+var_start, len);
                char* var = getenv(substring);
                free(substring);
                int var_len = strlen(var);
                buffer_len = buffer_len + var_len;
                buffer = (char*) realloc(buffer, buffer_len);
                strcpy(buffer+var_start, var);
                dollar = 0;
                continue;
            }
        }
        else {
            if(state == SPACE) {
                state = NORMAL;
            }
            append(buffer, c);
            i++;
        }
    }

    free(buffer);

    return l;
}