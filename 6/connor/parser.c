#include <string.h>
#include <stdlib.h>

#include "list.h"

#define NORMAL 0
#define SPACE 1
#define DOUBLEQUOTES 2
#define QUOTES 3
#define DOLLAR 4

void append(char* s, char c) {
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

void add_buffer_to_list(list_t* l, char* buffer) {
    char* new = (char*) malloc(strlen(buffer) * sizeof(char));
    strcpy(new, buffer);
    list_append(l, new);
    buffer[0] = "\0";
}

// TODO: Correct termination of DOLLAR mode
list_t* parse(char *s) {
    list_t *l = list_init();
    int state = NORMAL;
    int len = strlen(s);
    int i = 0;
    char c;
    char* buffer[1024];

    while(i < len) {
        c = s[i];

        if(c == " ") {
            if(state == SPACE) {
                // Multiple spaces
                i++;
                continue;
            }
            else if (state == DOUBLEQUOTES || state == QUOTES) {
                // Space within quotes
                append(buffer, ' ');
                i++;
                continue;
            }
            else if (state == DOLLAR) {
                // Expand variable and add to list
                char* var = getenv(buffer);
                add_buffer_to_list(l, var);
                buffer[0] = "\0";
                state = SPACE;
                i++;
                continue;
            }
            else {
                // State is normal, add buffer to list
                add_buffer_to_list(l, buffer);
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
            if(state == QUOTES) {
                append(buffer, '$');
                i++;
                continue;
            }
            else {
                state = DOLLAR;
                i++;
                continue;
            }
        }

        if(c == '\\') {
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

        // Normal character
        append(buffer, c);
        i++;
    }

    return l;
}