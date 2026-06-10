#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

void setupVolume(){

  PINSEL_CFG_Type cfg;
  cfg.OpenDrain = 0;
  cfg.Pinmode = 0;
  cfg.Portnum = 2;
  cfg.Funcnum = 0;

  cfg.Pinnum = 2;
  PINSEL_ConfigPin(&cfg);
  cfg.Pinnum = 3;
  PINSEL_ConfigPin(&cfg);
  cfg.Pinnum = 4;
  PINSEL_ConfigPin(&cfg);
  cfg.Pinnum = 5;
  PINSEL_ConfigPin(&cfg);

  GPIO_SetDir(2, (1<<2)|(1<<3)|(1<<4)|(1<<5), 1);
}


void displayVolume(int level){

  level = level - 1;

  if(level - 8 > -1){
    LPC_GPIO2->FIOSET = (1<<2);
    level = level - 8;
  } else {
    LPC_GPIO2->FIOCLR = (1<<2);
  }

  if(level - 4 > -1){
    LPC_GPIO2->FIOSET = (1<<3);
    level = level - 4;
  } else {
    LPC_GPIO2->FIOCLR = (1<<3);
  }

  if(level - 2 > -1){
    LPC_GPIO2->FIOSET = (1<<4);
    level = level - 2;
  } else {
    LPC_GPIO2->FIOCLR = (1<<4);
  }

  if(level - 1 > -1){
    LPC_GPIO2->FIOSET = (1<<5);
    level = level - 1;
  } else {
    LPC_GPIO2->FIOCLR = (1<<5);
  }
}
