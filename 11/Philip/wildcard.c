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

int x_star_x_filter(char *dir, char *w_star) {
    //compare strings until a star occurs
    int i = 0;
    char ofdir = dir[i];
    char ofpattern = w_star[i];
    while (ofpattern != '*') {
        if (ofdir != ofpattern) //pattern does not match
            return 0;
        i++; //check next char for equality
        ofdir = dir[i];
        ofpattern = w_star[i];
    }

    //compare strings backwards if pattern matches
    i = strlen(w_star) - 1;
    int k = strlen(dir) - 1;
    ofdir = dir[k];
    ofpattern = w_star[i];
    while (ofpattern != '*') {
        if (ofdir != ofpattern) //pattern does not match
            return 0;
        i--; //check next char for equality
        k--;
        ofdir = dir[k];
        ofpattern = w_star[i];
    }
    return 1;
}

//ssh -p 22222 student@localhost