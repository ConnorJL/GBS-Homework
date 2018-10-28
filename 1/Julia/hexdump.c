#include<stdio.h>

void hexdump (FILE *output, unsigned char *buffer, int length){
  char printable[17];
  int i;

  for(i = 0; i < length; i++){
    //Print Offset
    if((i % 16) == 0) {
      if(i != 0){
        fprintf(output, "  %s\n", printable);
      }
      fprintf(output, "%06x : ", i);
    }

    //Print HexNumbers
    fprintf(output, "%02x ", buffer[i]);

    //Store Printable
    if((buffer[i] < 0x20) || (buffer[i] > 0x7e)){
      printable[i % 16] = '.';
    } else {
      printable[i % 16] = buffer[i];
    }
    printable[(i % 16) + 1] = '\0';
  }

  while((i % 16) != 0){
    fprintf(output, "   ");
    i++;
  }
  fprintf(output, "  %s\n\n", printable);

}
