#include "read_PC.h"
#include "main.h"
#include <stdio.h>

int pc_read(){
  char serial_recieved[2];
  int valid = 0;


  valid = read_usb_serial_blocking(serial_recieved,1);


  sprintf(serial_recieved,valid);

  //FILE *stuff;
  //stuff = fopen("lemons.txt","w");
  //fprintf(stuff,"%s",valid);
  //fclose(stuff);

  write_usb_serial_blocking(serial_recieved,1);

}
