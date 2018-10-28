#include<stdio.h>
#include <unistd.h>
#include <ctype.h>

int main() {

  char text[256];
  char result[256];

  read(0, text, sizeof(text));

  //To Upper letter
  int i = 0;
  while((isalpha(text[i]))) {
      result[i] = (char) (toupper(text[i]));
      i++;
   }

   //Rot13
   i = 0;
   while(result[i]){
     if (result[i] > 64 && result[i] < 78) {
       result[i] += 13;
     } else {
       result[i] -= 13;
     }
     i++;
   }

   //delete \n
   char newtext[256];
   i = 0;
   while((text[i] != '\n')){
     newtext[i] = text[i];
     i++;
   }

   write(1, "Hallo: ", 7);
   write(1, newtext, sizeof(newtext));
   write(1, " -- ", 4);
   write(1, result, sizeof(result));
}
