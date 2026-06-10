#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "spi.h"
#include "timers.h"
#include "main.h"
#include "i2c.h"

#define BCLK   1<<17
#define DIN    1<<6 //Data into audio board
#define LRCIN  1<<4
#define DOUT   1<<16 //Data out of audio board
#define LRCOUT 1<<15
#define AUDIOI2CADDR 0x1B

#define MESSAGE(addr,data) ((addr) << 9) + (data)

void setup_write(){

	//BCLK
	PINSEL_CFG_Type cfg;
	cfg.OpenDrain = 0;
	cfg.Pinmode = 0;
	cfg.Portnum = 0;
	cfg.Funcnum = 0;
	cfg.Pinnum = 17;
	PINSEL_ConfigPin(&cfg);

	//DIN
	cfg.Pinnum = 6;
	PINSEL_ConfigPin(&cfg);

	//DIN
	cfg.Pinnum = 16;
	PINSEL_ConfigPin(&cfg);

	//LRCIN
	cfg.Pinnum = 4;
	PINSEL_ConfigPin(&cfg);

	//LRCOUT
	cfg.Pinnum = 15;
	PINSEL_ConfigPin(&cfg);

	//set to outputs
	GPIO_SetDir(0,BCLK  , 1);
	GPIO_SetDir(0,DIN   , 1);
	GPIO_SetDir(0,LRCIN , 1);
	GPIO_SetDir(0,DOUT  , 0);
	GPIO_SetDir(0,LRCOUT, 1);
}

void writeSetting(uint16_t message){
	//GPIO_ClearValue(0, 1 << 11);
	uint8_t txbuff[2];
	txbuff[0] = message >> 8;
	txbuff[1] = message & 0x00FF;
	sendBytes(AUDIOI2CADDR, txbuff, 2);
	//GPIO_SetValue(0, 1 << 11);
}

void audio_control(){
	// GPIO_SetValue(0, 1 << 11);
	//setupI2C();
	//nmap();

	// PINSEL_CFG_Type cfg;
	// cfg.OpenDrain = 0;
	// cfg.Pinmode = 0;
	// cfg.Portnum = 2;
	// cfg.Funcnum = 0;
	// cfg.Pinnum = 4;
	// PINSEL_ConfigPin(&cfg);

	// //set mode to SPI for control signals
	// GPIO_SetDir(2,1 <<4, 1);
	// GPIO_SetValue(2, 1 <<4);

	// cfg.Portnum = 0;
	// cfg.Funcnum = 0;
	// cfg.Pinnum = 11;
	// PINSEL_ConfigPin(&cfg);
	// GPIO_SetDir(0,1 <<11, 1);

	writeSetting(0b0001111000000000); // 15 - Reset
	writeSetting(0b0000000000110000); //  0 - Max Left Input Vol
	writeSetting(0b0000001000110000); //  1 - Max Right Input Vol
	// writeSetting(0b0000010000111111); //  2 - Max Left Out Vol
	// writeSetting(0b0000011000111111); //  3 - Max Right Out Vol
	writeSetting(0b0000010011011111); //  2 - Max Left Out Vol
	writeSetting(0b0000011011011111); //  3 - Max Right Out Vol
	writeSetting(0b0000100000010000); //  4 - DAC selected
	writeSetting(0b0000101000000000); //  5 - Unmute DAC
	writeSetting(0b0000110001100010); //  6 - Everything On
	writeSetting(0b0000111000000001); //  7 - MSB first, left aligned
	writeSetting(0b0001000000001100); //  8 - SR0&1 -> 8kHz sample rate
	writeSetting(0b0001001000000001); //  9 - Activate Digital interface

}

void write_audio(uint16_t packet){
	write_audio_stereo(packet, packet);
}

void write_audio_stereo(uint16_t L_chan, uint16_t R_chan){
	// Send each bit (MSB first) while toggling clocks
	int count = 15;
	int bitVal;
	LPC_GPIO0->FIOCLR = LRCIN;
	while(count > -1){
		LPC_GPIO0->FIOCLR = BCLK;
		bitVal = ((L_chan >> count) & 1);
		LPC_GPIO0->FIOSET = bitVal << 6; // Output Bit
		LPC_GPIO0->FIOCLR = ~bitVal << 6; // Output LOW
		LPC_GPIO0->FIOSET = BCLK;
		count--;
	}
	//Switch channel
	count = 15;
	LPC_GPIO0->FIOSET = LRCIN;
	while(count > -1){
		LPC_GPIO0->FIOCLR = BCLK;
		bitVal = ((R_chan >> count) & 1);
		LPC_GPIO0->FIOSET = bitVal << 6; // Output Bit
		LPC_GPIO0->FIOCLR = ~bitVal << 6; // Output LOW
		LPC_GPIO0->FIOSET = BCLK;
		count--;
	}
}

void read_audio(int16_t *L, int16_t *R){
	int count = 15;
	int bitVal;
	LPC_GPIO0->FIOCLR = BCLK;
	LPC_GPIO0->FIOCLR = LRCOUT;
	while(count > -1){
		// Clock rising edge
		LPC_GPIO0->FIOSET = BCLK;
		// Read audio bit
		bitVal = LPC_GPIO0->FIOPIN>>16 & 0x1;
		// Clock falling edge
		LPC_GPIO0->FIOCLR = BCLK;
		*L |= bitVal << count;
		count--;
	}
	//Switch channel
	count = 15;
	LPC_GPIO0->FIOSET = LRCOUT;
	while(count > -1){
		// Clock rising edge
		LPC_GPIO0->FIOSET = BCLK;
		// Read audio bit
		bitVal = ((LPC_GPIO0->FIOPIN) >> 16) & 0x1;
		// Clock falling edge
		LPC_GPIO0->FIOCLR = BCLK;
		*R |= bitVal << count;
		count--;
	}
	if(*L > 0x8000){
		*L -= 0x10000;
	}
	if(*R > 0x8000){
		*R -= 0x10000;
	}
}
