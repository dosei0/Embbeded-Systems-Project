#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_spi.h"
#include "lpc17xx_ssp.h"

#include <stdio.h>

#include "main.h"
#include "serial.h"
#include "spi.h"
#include "ffconf.h"
#include "ff.h"
#include "i2c.h"
//#include "diskio.h"
#include "audio_interface.h"
#include "timers.h"
#include "menu.h"
#include "read_PC.h"
#include "i2c.h"
#include "spi.h"
#include "change_settings.h"
#include "files.h"
#include "commands.h"
#include "volume.h"
#include "LEDs.h"


#define buff_size 0x2000
// Define our pin numbers
#define LED1 18
#define LED2 20
#define LED3 21
#define LED4 23
int LEDs[4] = {1 << LED1, 1 << LED2, 1 << LED3, 1 << LED4};
int allLEDs = (1 << LED1) | (1 << LED2) | (1 << LED3) | (1 << LED4);

uint16_t recentMax = 0;
uint16_t recentMin = 0;
uint16_t range = 0;
uint16_t audioFrames[8];

// Allocate variables
char binString[4];
int val = 0;
int counter = 0;
char mode = '.';
FATFS FatFs;	// Work area (filesystem object) for logical drive
FIL fil;        /* File object */
FIL wrFil;
char buff[buff_size]; /* Line buffer */
FRESULT fr;     /* FatFs return code */
uint32_t bytes_read;
int refill = 0;
uint16_t L;
uint16_t R;
uint8_t f = 0;
int naming = 0;
char name[17];
int namecount;
char currentlyPressed = ' ';
int written = 0;
int stopRec = 0;
uint32_t startTime = 0;
uint32_t playtime = 0;
int volume = 1;
int listingFilesMenu = 0;
WAVHEADER meta;
char debounce = ' ';

char menuList[9][17] = {
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

char infoList[2][17] = {
	"1.Play File     ",
	"2.View Info     "
};
char files[100][17];

// Actual Functions
int main(void)
{
	SystemInit();
	serial_init();
	setupI2C();
	SPI_init();
	GPIO_SetDir(2, 0xFFFFFFFF, 1);
	setup_menu();
	SysTick_Config(SystemCoreClock / 1000); // Systick once per second
	setup_digits();
	setupVolume();
	int d;
	//for(d=100000000; d; d--);
	// Clear interrupts
	LPC_GPIOINT->IO0IntClr = (1 << 5);
	LPC_SC->EXTINT = (1 << 3);

	// Enable falling edge interrupt on P0.5
	LPC_GPIOINT->IO0IntEnF |= (1 << 5);

	NVIC_EnableIRQ(EINT3_IRQn);
	// Enable interrupts globally
	__enable_irq();

	//nmap();

	/* Register work area to the default drive */
    f_mount(&FatFs, "", 0);

	setup_write();
	audio_control();
	setupTimingInterrupt();
	resetInterrupt();
	startWav("MTV8k.wav");

	int n = 0;
	int frameGap = 0;

	// Handle tasks which are less time sensitive or interrupt based
	// in this main loop
    while(1){
		COMMAND cmd;
		readCmdBuff(&cmd);
		switch(cmd.cmd){

			case CMD_PLAY:
				stopPlayback();
				startWav(cmd.infile);
			break;

			case CMD_COPY:
				write_usb_serial_blocking("Copying...\n\r",12);
				stopPlayback();
				copyWAV(cmd, buff, FORWARDS);
			break;

			case CMD_DELETE:
				write_usb_serial_blocking("Deleting...\n\r",13);
				fr = f_unlink(cmd.infile);
				if (fr != FR_OK){
					// File access failed
					char str[32];
					sprintf(str, "Failed to delete file:%7d\n\r", fr);
					write_usb_serial_blocking(str,31);
				} else {
					write_usb_serial_blocking("Deleted file!\n\r",15);
				}
			break;

			case CMD_VOLUME:
				// set a scalar value? reconfigure board?
				volume = atoi(cmd.infile);
				if(volume < 25) volume = 25;
				if(volume > 100) volume = 100;
				volume += 27;
				writeSetting(0b0000010010000000|volume); //  2 - Max Left Out Vol
				writeSetting(0b0000011010000000|volume); //  3 - Max Right Out Vol
			break;

			case CMD_REVERSE:
				// Is this meant to save a new file or jst playback in reverse?
				stopPlayback();
				copyWAV(cmd, buff, REVERSE);
				startWav("reverse.wav");
			break;

			case CMD_LSTFILS:
				listFiles("",files);
			break;
			default:
			break;
		}
		// Do LED frame
		if(frameGap > 30){
        	ledCtl(n, audioFrames, recentMax);
			n = (n+1) % 128;
			frameGap = 0;
			// Send PC the audio val too
			if(mode == 'p'){
				char audioData[13];
				sprintf(audioData, "A: %8d\n\r", recentMax);
				write_usb_serial_none_blocking(audioData, 13);
			}
			displayVolume(recentMax >> 12);
			playback_light(mode == 'p');
			record_light(mode == 'r');
		}
		// Write playtime
		if(mode == 'p' || mode == 'r'){
			playtime = (timems - startTime)/1000;
			//send_time(playtime/60, playtime%60);
		} else {
			//clear_digits();
		}
		// Above lines currently causing main loop to hang if I2C send is interrupted

        // delay
        int tmp;
        for (tmp = 0; tmp < 5000; tmp++);
		frameGap++;
    }
	return 0;
}

void startWav(char *filename){
	// Open a wav file 
    fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK){
		// File access failed
		char str[33];
		sprintf(str, "Couldn't open file err:%7d\n\r", fr);
		write_usb_serial_blocking(str,32);
	}

	// Read header bytes
	uint8_t metadata[44];
	fr = f_read(&fil, metadata, 44, &bytes_read);
	if (fr != FR_OK){
		// File access failed
		char str[33];
		sprintf(str, "Couldn't read head err:%7d\n\r", fr);
		write_usb_serial_blocking(str,32);
	}

	// Extract metadata
	readWavHeader(metadata, &meta);

	// Read in initial buffer
	fr = f_read(&fil, buff, buff_size, &bytes_read);
	if (fr != FR_OK){
		// File access failed
		char str[33];
		sprintf(str, "Couldn't read buff err:%7d\n\r", fr);
		write_usb_serial_blocking(str,32);
	}

	// Set timer interval (us):
	// int period = 1000000 / meta.sampleRate;
	// LPC_TIM0->MR0 = period - 1;
	/* NB: this also requires reconfiguring the audio board...*/

	// Enable audio playback mode and interrupts
	NVIC_EnableIRQ(TIMER0_IRQn);
	mode = 'p';
	startTime = timems;
}

void startRecording(){
	counter = 0;
	f_close(&fil);
	fr = f_open(&wrFil, "sample.wav", FA_CREATE_ALWAYS | FA_WRITE);
	if (fr != FR_OK){
		// File access failed
		char str[31];
		sprintf(str, "Open record file err:%7d\n\r", fr);
		write_usb_serial_blocking(str,30);
	}
	// Skip past header to write data
	fr = f_lseek(&wrFil,44);
	if (fr != FR_OK){
		char str[32];
		sprintf(str, "Couldn't seek data   :%7d\n\r", fr);
		write_usb_serial_blocking(str,31);
	}
	// Set current mode to recording
	mode = 'r';
	startTime = timems;
	// stringwrite("RECORDING...    ",0);
	// stringwrite("                ",0);
}

void stopPlayback(){
	NVIC_DisableIRQ(TIMER0_IRQn);
	mode = 's';
	if(mode == 'p'){
		f_close(&fil);
	}
}


// Handles user interface through keypad menu
void poll(){
	currentlyPressed = keypad();
	// debounce
	if(currentlyPressed == debounce){
		return;
	} else {
		debounce = currentlyPressed;
	}

	// The only keypad use we allow during recording is to stop it
	if(mode == 'r'){
		if(currentlyPressed == '1'){
			stopRec = 1;
		}
		return;
	}

	if (menuMode != MENU_MAIN){
		switch(currentlyPressed){
			case '#':
				move_down();
			break;

			case '*':
				move_up();
			break;

			case 'A':
				if(menuMode == MENU_FILES){
					viewInfo(files[screen]);
					screen = 0;
					filesToMenu();
					menuMode = MENU_INFO;
				}
			break;

			case 'B': // Back button
				switch(menuMode){
					// If on the files screen, go back to main screen
					case MENU_FILES:
						for(f = 0; f < 9; f++){
							strcpy(list[f], menuList[f]);
						}
						listLen = 9;
						screen = 0;
						filesToMenu();
						menuMode = MENU_MAIN;
					break;

					// If on track info screen, go to files screen
					case MENU_INFO:
						listFiles("", files);
						for(f=0; f < 100; f++){
							if(strlen(files[f]) == 0) break;
							char line[17];
							sprintf(line, "%2d %13s", f+1, files[f]);
							strcpy(list[f], line);
						};
						strcpy(list[f], "                ");
						listLen = f + 1;
						filesToMenu();
						menuMode = MENU_FILES;
					break;
				}
			break;

			case 'C':
				if(menuMode == MENU_FILES){
					startWav(files[screen]);
					for(f = 0; f < 9; f++){
						strcpy(list[f], menuList[f]);
					}
					listLen = 9;
					screen = 0;
					filesToMenu();
					menuMode = MENU_MAIN;
				}
			break;
		}
	} else {
		switch(currentlyPressed){
			case '#':
				// Scroll down the menu
				move_down();
			break;

			case '*':
				// Scroll up the menu
				move_up();
			break;

			case '1':
				// Start/stop a recording
				
				if(mode != 'r' && stopRec == 0){
					// Starting a recording
					startRecording();
				} else {
					// Set flag to stop recording after this audio block
					stopRec = 1;
				}
			break;

			case '2':
				// Set flag to play a test tone
				stopPlayback();
				mode = 't';
				NVIC_EnableIRQ(TIMER0_IRQn);
			break;

			case '3':
				// Start a wav file playing
				stopPlayback();
				write_usb_serial_blocking("Stopped Playback\n\r",18);
				startWav("NGGYU8k.wav");
			break;

			case '4':
				// Test keypad text entry
				naming = 1;
				strcpy(name,"");
				namecount = 0;
			break;

			case '5':
				stopPlayback();
				getVolumeGraph("MTV8k.wav", 200, 300);
				mode = 's';
			break;

			case '6':
				listFiles("", files);
				for(f=0; f < 100; f++){
					if(strlen(files[f]) == 0) break;
					char line[17];
					sprintf(line, "%2d %13s", f+1, files[f]);
					strcpy(list[f], line);
				};
				strcpy(list[f], "                ");
				listLen = f + 1;
				screen = 0;
				filesToMenu();
				menuMode = MENU_FILES;
			break;

			case  '7':
				LEDMode = 1 - LEDMode;
			break;
		}
	}		
}

// General debug / error code function
void displayNibble(char N){
	int digit;
	// Count down the digits, so MSB is first
	for (digit = 3; digit >= 0; digit--)
	{
		if (N & (1 << (3 - digit)))
		{
			GPIO_SetValue(1, LEDs[digit]);
			binString[digit] = '1';
		} else
		{
			GPIO_ClearValue(1, LEDs[digit]);
			binString[digit] = '0';
		}
	}
}

// Required for fatfs
int get_fatttime(){
	return 1;
}

int16_t audioVal;


// Called at freq of sample rate - handles all audio operations
void TIMER0_IRQHandler()
{
	LPC_TIM0->IR |= 1;
	switch(mode){
		case 'p':
			// Playback mode
			if(meta.numChannels == 1){
				audioVal = (buff[counter*meta.blockAlign + 1] << 8) + buff[counter*meta.blockAlign + 0];
				write_audio(audioVal);
			} else {
				int16_t L_chan = (buff[counter*meta.blockAlign + 1] << 8) + buff[counter*meta.blockAlign + 0];
				int16_t R_chan = (buff[counter*meta.blockAlign + 3] << 8) + buff[counter*meta.blockAlign + 2];
				audioVal = (L_chan + R_chan)/2;
				write_audio_stereo(L_chan, R_chan);
			}

			// Update audio amplitude value
			if(recentMax > 0x10){
				recentMax -= 0x10;
			}
			if (recentMax < audioVal){
				recentMax = audioVal;
			}

			// Update timer readout
			// playtime = (timems - startTime)/1000;
			// send_time(playtime/60, playtime%60);
			/* NB: Slows down the ISR too much to be viable */

			// Advance through file and refill if needed
			counter++;
			if(counter >= buff_size/meta.blockAlign){
				counter = 0;
				bytes_read = 0;
				fr = f_read(&fil, buff, buff_size, &bytes_read);
				if (fr != FR_OK){
					char str[10];
					sprintf(str, "%7d\n\r", fr);
					write_usb_serial_blocking(str,10);
				}
				if (bytes_read == 0){
					mode = 's';
				}
			}
		break;

		case 't':
			// Test tone mode - outputs a ~2kHz sawtooth wave
			write_audio(counter);
			counter += 0x4000;
			if(counter >= 0x10000){
				counter = 0;
			}
		break;
		
		case 'r':
			// Recording mode
			L = 0;
			R = 0;
			read_audio(&L, &R);
			buff[counter + 0] = L & 0x00FF;
			buff[counter + 1] = L >> 8;
			buff[counter + 2] = R & 0x00FF;
			buff[counter + 3] = R >> 8;
			counter += 4;

			// Update time readout
			// playtime = (timems - startTime)/1000;
			// send_time(playtime/60, playtime%60);
			
			// If we've hit the end of a block
			if(counter >= buff_size){
				counter = 0;
				// Save buffer to file
				int w;
				fr = f_write(&wrFil, buff, buff_size, &w);
				if (fr != FR_OK){
					char str[10];
					sprintf(str, "%7d\n\r", fr);
					write_usb_serial_blocking(str,10);
				} else {

					written += buff_size;
				}

				// Stop recording if requested
				if(stopRec){
					// Write file header
					uint8_t header[44];
					writeWavHeader(header, written, 2, 8000, 16);
					fr = f_lseek(&wrFil,0);
					if (fr != FR_OK){
						char str[32];
						sprintf(str, "Couldn't seek header :%7d\n\r", fr);
						write_usb_serial_blocking(str,31);
					}
					fr = f_write(&wrFil, header, 44, &w);
					if (fr != FR_OK){
						char str[32];
						sprintf(str, "Couldn't write header:%7d\n\r", fr);
						write_usb_serial_blocking(str,31);
					}
					f_close(&wrFil);
					write_usb_serial_blocking("Saved\n\r",7);

					// Start rename subroutine
					strcpy(name,"");
					namecount = 0;
					stringwrite("Enter File Name:  ",0);
					stringwrite("                  ",1);
					naming = 1;

					stopRec = 0;
					mode = 's';
				}
			}
		break;
		case 's':
		default:
		break;
	}
	//resetInterrupt();
}

void EINT3_IRQHandler(void){
	LPC_GPIOINT->IO0IntClr = (1 << 5);
	LPC_SC->EXTINT = (1 << 3);
	LPC_GPIOINT->IO0IntEnF |= (1<<10);
	//write_usb_serial_blocking("cheese",7);

	if (naming == 0){
		poll();
	} else {
		inputName(name, &namecount, &naming);
	}

	resetInterrupt();
}
