#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int star_x_filter(char *dir, char *w_star) {
    char *star_pos = strchr(w_star, '*');
    if (strchr(dir, star_pos[1]) != NULL) {
        int cmp = strcmp(strchr(dir, star_pos[1]), star_pos + 1);
        if (cmp == 0) return 1;
    } else return 0;
    return 0;
}

int x_star_filter(char *dir, char *w_star) {
    char *star_pos = strchr(w_star, '*');
    if (strncmp(w_star, dir, star_pos - w_star) == 0) {
        return 1;
    }
    return 0;
}

//ssh -p 22222 student@localhost