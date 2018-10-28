#include <stdio.h>
#include <unistd.h>

void rot(char* s, int count, char* out) {
    for(int i = 0; i<count; i++) {
        out[i] = (char) (((((int) s[i] - 65) + 13) % 26) + 65);
    }
    out[count] = '\0';
}

int main(int argc, char const *argv[])
{
    char name[256];
    char clean_name[256];
    char out[257];
    int count = 0;
    int temp = 0;
    read(STDIN_FILENO, name, 256);

    while(1) {
        if(name[temp] == '\n') {
            clean_name[temp] = '\0';
            break;
        }
        else {
            clean_name[temp] = name[temp];
        }
        temp++;
    }

    while(1) {
        if(name[count] == '\n') {
            // End of String
            break;
        }
        if((int) name[count] > 64 && (int) name[count] < 91) {
            // Uppercase character
            count++;
            continue;
        }
        if((int) name[count] > 96 && (int) name[count] < 123) {
            // Lowercase character
            name[count] = (char) ((int) name[count] - 32);
            count++;
            continue;
        }
        break;
    }
    
    rot(name, count, out);
    write(STDOUT_FILENO, "Hallo: ", 7);
    write(STDOUT_FILENO, clean_name, temp);
    write(STDOUT_FILENO, " -- ", 4);
    write(STDOUT_FILENO, out, count);
    write(STDOUT_FILENO, "\n", 1);
    // printf("Hallo: %s -- %s\n", clean_name, out);
    return 0;
}
