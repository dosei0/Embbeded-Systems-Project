#include "lpc_types.h"

#define segAddr 0x38
#

uint8_t sevSegDigits[16]={63, 6, 91, 79, 102, 109, 125,7, 127, 111, 119, 124, 57, 94, 121, 113};

void setup_digits(){

  uint8_t toSend[5];

  toSend[0] = 0b00000000;

  //instruction byte 0 means the next byte is control byte
// control byte turns displays on & dynamic mode

  toSend[1] = 0b01000111;

  sendBytes(segAddr,toSend,2);

   toSend[0] = 1; // instruction byte - first digit to control is 1 (right hand side)
   toSend[1] = sevSegDigits[0];
   toSend[2] = sevSegDigits[0];
   toSend[3] = sevSegDigits[0];
   toSend[4] = sevSegDigits[0];
   sendBytes(segAddr,toSend,5);

}

void send_time(int minutes, int seconds){
  uint8_t toSend[5];
  int secondsTens = seconds/10;
  int secondsUnits = seconds - secondsTens*10;

  int minutesTens = minutes/10;
  int minutesUnits = minutes - minutesTens*10;


  toSend[0] = 1; // instruction byte - first digit to control is 1 (left hand side)
  toSend[1] = sevSegDigits[minutesTens];
  toSend[2] = sevSegDigits[minutesUnits];
  toSend[3] = sevSegDigits[secondsTens];
  toSend[4] = sevSegDigits[secondsUnits];
  sendBytes(segAddr,toSend,5);
}

void clear_digits(){
  uint8_t toSend[] = {1,0,0,0,0};
  sendBytes(segAddr,toSend,5);
}
