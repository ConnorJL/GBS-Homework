#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "list.h"

void append_char(char *s, char c) {
    int len = (int)strlen(s);
    s[len] = c;
    s[len + 1] = '\0';
}

//TODO: change tok_s
char *append_tok(list_t *tokens, char tok[1024]) {
    list_append(tokens, tok);
    tok = malloc(sizeof(char) * 1024);
    tok[0] = '\0';
    return tok;
}

//void getVarName(char *v, int *from, int *to) {
//    char *valid = "ABZDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
//    for (int j = 0; j < strlen(v); j++) {
//        for (int i = 0; i < strlen(valid); i++) {
//            if (v[j] == valid[i]) {
//                if (*from == -1) {
//                    *from = j;
//                }
//            }
//            else {
//                if (*from != -1 && v[j] != valid[i]) {
//                    *to = j;
//                    return;
//                }
//            }
//        }
//    }
//}

void getVarVal(char *ret, char *tokc, char *env[]) {
    FILE *fp;
    char echo[1030];
    echo[0] = '\0';
    strcpy(echo, "echo $");
    strcat(echo, tokc);
    fp = popen(echo, "r");

    char ne[1024];
    ne[0] = '\0';
    while (fgets(ne, 1023, fp) != NULL) {
        strcat(ret, ne);
    }
    ret[strlen(ret) - 1] = '\0';
}

//void getVarVal(char *ret, char *token, char *env[]) {
//    int i = 0;
////    strcat(ret, env[0]);
//    while (env[i] != '\0') {
//        printf("STILL OK %s\n", env[i]);
//        if (strcmp(token, &(strtok(env[i], "=")[0])) == 0) {
//            printf("BEFORE CAT\n");
//            strcat(ret, &(strtok(NULL, "=")[0]));
//            printf("strtok %s\n", ret);
//            return;
//        }
//        i++;
//    }
//}

list_t *parse_inp(char *inp, char *envp[]) {
    char *valid = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    char *tok = malloc(sizeof(char) * 1024);
    tok[0] = '\0';
    int tok_s = 1024;
    char varName[1024];
    varName[0] = '\0';
    int bs = 0;
    int dou = 0;
    int sin = 0;
    int isVar = 0;
    list_t *tokens = list_init();
    for (int i = 0; i < strlen(inp); i++) {
        if (isVar == 1) {
            for (int j = 0; j < strlen(valid); j++) {
                if (inp[i] == valid[j]) {
                    append_char(varName, inp[i]);
                    isVar = 2;
                    break;
                }
            }
            if (isVar == 2) {
                isVar = 1;
                continue;
            }
            if (inp[i] == '\\' && inp[i + 1] == ' ') {
                i = i + 1;
                continue;
            }
            char ret[1024];
            ret[0] = '\0';
            getVarVal(ret, varName, envp);
            if (strlen(tok) + strlen(ret) >= tok_s) {
                tok_s *= 2;
                tok = realloc(tok, tok_s);
            }
            strcat(tok, ret);
            ret[0] = '\0';
            varName[0] = '\0';
        }

        if (inp[i] == ' ' && dou == 0 && sin == 0 && bs == 0) {
            if (strcmp((tok), "") != 0) {
                tok = append_tok(tokens, tok);
                tok_s = 1024;
            }
            isVar = 0;
            tok[0] = '\0';
        } else {
            if (inp[i] == '\\' && bs == 0) {
                bs = 1;
                isVar = 0;
            } else if (inp[i] == '"' && sin == 0 && bs == 0) {
                if (dou == 1)
                    dou = 0; // echo \\$PWD  "What\'s up?" \' Hello World\'\nexit\n
                else
                    dou = 1;
                bs = 0;
                isVar = 0;
            } else if (inp[i] == "'"[0] && dou == 0 && bs == 0) {
                if (sin == 1)
                    sin = 0;
                else
                    sin = 1;
                bs = 0;
                isVar = 0;
            } else if (inp[i] == '$' && bs == 0) { //  && dou == 0 && sin == 0
//                append_char(varName, inp[i]);
                isVar = 1;
                bs = 0;
            } else {
                append_char(tok, inp[i]);
                bs = 0;
                isVar = 0;
            }
        }
    }
    if (isVar == 1) {
        char ret[1024];
        ret[0] = '\0';
        getVarVal(ret, varName, envp);
        strcat(tok, ret);
        append_tok(tokens, tok);
        ret[0] = '\0';
        varName[0] = '\0';
    } else {
        append_tok(tokens, tok);
    }
//    append_tok(tokens, tok);
    return tokens;
}
