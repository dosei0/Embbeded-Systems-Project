#include "lpc17xx_i2c.h"
#include "lpc17xx_uart.h"		// Central include files
#include "lpc17xx_pinsel.h"
#include "lpc_types.h"
#include <string.h>
#include "timers.h"
#include "menu.h"
#include "main.h"
#include "i2c.h"
#include "serial.h"
#include "files.h"
#include "ff.h"


I2C_M_SETUP_Type test;
int screen;

typedef int bool;
#define true 1
#define false 0

#define SCREEN_ADDR 0x3B
#define KEYPAD_ADDR 0x21

int fileSelected = 0;
int viewingInfo = 0;
char file[17];
char viewFile[17];
char menuMode = MENU_MAIN;

char list[100][17] = {
	"1.Record (UA0/2)",
	"2.Play Test (A1)",
	"3.Playback  (A3)",
	"4.Text Entry(U0)",
	"5.Exp graph(PCx)",
	"6.File List (U1)",
	"7.LED mode (IC3)",
	"8.              ",
	"9.              "
};

uint8_t listLen = 9;

char words[17];

void setUpScreen(void){
	uint8_t  buffer[11];
	buffer[0]  = 0x00;
	buffer[1]  = 0x34;
	buffer[2]  = 0x0c;
	buffer[3]  = 0x06;
	buffer[4]  = 0x35;
	buffer[5]  = 0x04;
	buffer[6]  = 0x10;
	buffer[7]  = 0x42;
	buffer[8]  = 0x9f;
	buffer[9]  = 0x34;
	buffer[10] = 0x02;

	test.sl_addr7bit = SCREEN_ADDR;
	test.tx_data = buffer;
	test.tx_length = 11;


	I2C_MasterTransferData(LPC_I2C1, &test,I2C_TRANSFER_POLLING);

}

void clearDisplay(void){
	uint8_t  buffer[2];

	test.sl_addr7bit = SCREEN_ADDR;
	test.tx_length = 2;
	int x;
	for(x = 0x80; x <= 0xff; x++){
		buffer[0] = 0x00;
		buffer[1] = x;
		test.tx_data = buffer;
		I2C_MasterTransferData(LPC_I2C1, &test,I2C_TRANSFER_POLLING);

		buffer[0] = 0x40;
		buffer[1] = 0xA0;
		test.tx_data = buffer;
		I2C_MasterTransferData(LPC_I2C1, &test,I2C_TRANSFER_POLLING);
	}
}

void write(uint8_t address, uint8_t data){
	uint8_t buffer[2];
	buffer[0] = 0x00;
	buffer[1] = address;

	test.tx_length = 2;
	test.sl_addr7bit = SCREEN_ADDR;
	test.tx_data = buffer;

	I2C_MasterTransferData(LPC_I2C1, &test,I2C_TRANSFER_POLLING);

	buffer[0] = 0x40;
	buffer[1] = data;
	test.tx_data = buffer;
	I2C_MasterTransferData(LPC_I2C1, &test,I2C_TRANSFER_POLLING);
}

void stringwrite(char* text, uint8_t position){

	uint8_t start_address;
	test.sl_addr7bit = SCREEN_ADDR;
	test.tx_length = 2;
	test.rx_data = NULL;
	test.rx_length = 0;

	if (position == 0){
		start_address = 0x80;
	}
	else{
		start_address = 0xA8;
	}
	int x;

	for(x = 0; x < strlen(text); x++){

		write(start_address,(uint8_t) text[x]+128);
		start_address += 1;
	}

}

void keypadwrite(uint8_t* input){
	test.sl_addr7bit = KEYPAD_ADDR;
	test.tx_data = &input;
	test.tx_length = 1;
	test.rx_data = NULL;
	test.rx_length = 0;

	I2C_MasterTransferData(LPC_I2C1, &test,I2C_TRANSFER_POLLING);

}

uint8_t keyread(){

	uint8_t output;

	test.sl_addr7bit = KEYPAD_ADDR;
	test.tx_data = NULL;
	test.tx_length = 0;
	test.rx_data = &output;
	test.rx_length = 1;

	I2C_MasterTransferData(LPC_I2C1, &test,I2C_TRANSFER_POLLING);

	return output;

}

int keypad(){
	char keypad[4][4] = {
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','0','#','D'}};
	int x;
	int y;
	char currentLetter = 'z';
	uint8_t bytes[] = {0b01111111, 0b10111111, 0b11011111, 0b11101111};
	char outputterm[5];
	uint8_t output;
	for(x = 0; x < 4; x++){
			keypadwrite(bytes[x]);
			output = keyread();
			output = output & 0x0f;
			y = map(output);
			if (y != -1){
				currentLetter = keypad[y][x];
			}
	}
	return currentLetter;
}

int map(int letter){
	switch(letter){
		case 0x7:
			return 0;
		break;

		case 0xb:
			return 1;
		break;

		case 0xd:
			return 2;
		break;

		case 0xe:
			return 3;
		break;

		default:
			return -1;
	}
}

void setup_menu(){

	test.retransmissions_max = 0;
	test.rx_length = 0;
	test.rx_data = NULL;

	setUpScreen();
	clearDisplay();

	screen = 0;

	filesToMenu();
}

void filesToMenu(){
	//listFiles();
	strcpy(words, &list[screen]);
	stringwrite(words,0);

	strcpy(words, &list[screen+1]);
	stringwrite(words,1);
}


// void fileSelect(char file[17]){
// 	fileSelected = 1;
//  	//viewFile = file;

// 	char fileSelectedList[2][17] = {
// 	"1.Play File     ",
// 	"2.View Info     "
// 	}

// 	int f;
// 	for(f=0; f < 2; f++){
// 		strcpy(&list[f*17], &fileSelectedList[f*17]);
// 	}
// 	listLen = 2;

// 	filesToMenu();
// }

void viewInfo(char* fname){
	write_usb_serial_blocking(fname, 16);
	FIL qFile;
	FRESULT fr;
	fr = f_open(&qFile, fname, FA_READ);
    if (fr != FR_OK){
        char str[32];
        sprintf(str, "Failed to open file:  %7d\n\r", fr);
        write_usb_serial_blocking(str,31);
        return;
    }

    // Read the metadata
    char header[44];
	UINT br;
    fr = f_read(&qFile, header, 44, &br);
    if (fr != FR_OK){
        char str[32];
        sprintf(str, "Failed to read infile:%7d\n\r", fr);
        write_usb_serial_blocking(str,31);
        return;
    }

    WAVHEADER meta;
    readWavHeader(header, &meta);
	f_close(&qFile);

	char line[17];

	sprintf(line, "Duration:  %02d:%02d", meta.duration/60, meta.duration % 60);
	strcpy(list[0], line);
	
	sprintf(line, "Size: %9dB", meta.datasize);
	strcpy(list[1], line);

	sprintf(line, "Channels: %6d", meta.numChannels);
	strcpy(list[2], line);

	sprintf(line, "SmplRt: %6dHz", meta.sampleRate);
	strcpy(list[3], line);
	
	sprintf(line, "SmplSize: %5db", meta.bitsPerSample);
	strcpy(list[4], line);

	listLen = 5;
}

void move_down(){
	screen ++;
	if (screen >= listLen-1){
		screen = 0;
	}
	filesToMenu();
}

void move_up(){
	screen --;
	if (screen == -1){
		screen = listLen - 2;
	}
	filesToMenu();
}

void resetInterrupt()
{
	uint8_t byte[1] = {0xf0};
	sendBytes(KEYPAD_ADDR,byte,1);
}

void inputName(char *name, int *namecount, int *naming){
	char pressed = keypad();
	if(pressed == '*'){
		write_usb_serial_blocking(name,16);
		*naming = 0;
		f_rename("sample.wav",name);
	} else if(pressed == '#'){
		if(*namecount > 0) *namecount = *namecount - 1;
		name[*namecount] = 0x00;
		stringwrite("                  ",1);
		while (keypad() == pressed);
	} else if(*namecount == 16){
		write_usb_serial_blocking(name,16);
		*naming = 0;
	} else {
		int i;
		int j;
		int k;
		k = 0;
		if(pressed != 'z'){
			if (pressed == '4' && *namecount == 0){
				*namecount = *namecount + 1;
				while (keypad() == pressed);
			} else {
				name[*namecount] = pressed;
				while (keypad() == pressed);
				*namecount = *namecount + 1;
			}
			name[*namecount] = 0x00;
		}
	}
	stringwrite("Enter File Name:  ",0);
	stringwrite(name,1);
}

void setup_status_lights(){
	PINSEL_CFG_Type cfg;
	cfg.OpenDrain = 0;
	cfg.Pinmode = 0;
	cfg.Portnum = 2;
	cfg.Funcnum = 0;
	cfg.Pinnum = 1;
	PINSEL_ConfigPin(&cfg);
	cfg.Pinnum = 0;
	PINSEL_ConfigPin(&cfg);
	GPIO_SetDir(2, (1<<0)|(1<<1), 1);
}

void record_light(int value){
	if (value == 1){
		GPIO_SetValue(2,(1<<1));
	} else {
		GPIO_ClearValue(2,(1<<1));
	}
}

void playback_light(int value){
	if (value == 1){
		GPIO_SetValue(2,(1<<0));
	} else {
		GPIO_ClearValue(2,(1<<0));
	}
}
