#include "lpc_types.h"
#include "spi.h"

uint16_t tx[320];
uint16_t rx[320];
uint8_t LEDMode = 0;
int lit;
int val;

void ledCtl(int n, uint16_t* LEDFrames, uint16_t audioVal){
    int i = 0;
	for(i = 7; i > 0; i--){
		LEDFrames[i] = LEDFrames[i-1];
	}
	LEDFrames[0] = audioVal >> 12;
    for(i = 0; i < 64; i ++){
		// Get LED Co-ordinates
        int y = (i / 8);
        int x = i - y*8;
		if(y % 2) x = 7-x;
		// Make pretty things
        switch(LEDMode){
            case 0:
                x = 7-x;
                lit = (LEDFrames[y] >= x);
                setLED(&tx[i*5],lit << 6, lit << (7-x), 0);
            break;

            case 1:
                val = (LEDFrames[0] + LEDFrames[1] + LEDFrames[2] + LEDFrames[3] + LEDFrames[4] + LEDFrames[5])*4 - ((x*x + y*y)*2);
                if(val < 0){
                    val = 0;
                } else if(val > 255){
                    val = 255;
                }
                setLED(&tx[i*5],val, val >> 2, 0);
            break;
        }
    }
    SPI_rw(tx,rx,640);
}

void setLED(uint16_t *pos,uint8_t R, uint8_t G, uint8_t B){
    uint32_t data = (G << 16) + (R << 8) + B;
    int packet;
    for(packet = 0; packet < 5; packet++){
        int pp = (23 - packet*5);
        pos[packet] = 0x00 | ((data >> (pp - 0 - 13)) & (1 << 13))
                           | ((data >> (pp - 1 - 10)) & (1 << 10))
                           | ((data >> (pp - 2 -  7)) & (1 <<  7))
                           | ((data >> (pp - 3 -  4)) & (1 <<  4));
        if(packet != 4){
            pos[packet] |= ((data >> (pp - 4 -  1)) & (1 <<  1));
            pos[packet] |= 0b0100100100100100;
        } else {
            pos[packet] |= 0b0100100100100000;
        }
    }
}