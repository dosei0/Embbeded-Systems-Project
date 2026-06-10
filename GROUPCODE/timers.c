#include "LPC17xx.h"
#include "lpc_types.h"
void setupTimingInterrupt()
{
	/*Assuming that PLL0 has been setup with CCLK = 100Mhz and PCLK = 25Mhz.*/
	LPC_SC->PCONP |= (1<<1); //Power up TIM0. By default TIM0 and TIM1 are enabled.
	LPC_SC->PCLKSEL0 &= ~(0x3<<3); //Set PCLK for timer = CCLK/4 = 100/4 (default)

	LPC_TIM0->CTCR = 0x0;
	LPC_TIM0->PR = (25 - 1); //Increment TC at every 24999+1 clock cycles
	//25000 clock cycles @25Mhz = 1 uS

    LPC_TIM0->MR0 = 124; //Toggle Time in uS
	LPC_TIM0->MCR |= (1<<0) | (1<<1); // Interrupt & Reset on MR0 match
	
    LPC_TIM0->TCR |= (1<<1); //Reset Timer0

	NVIC_EnableIRQ(TIMER0_IRQn); //Enable timer interrupt
	
	LPC_TIM0->TCR = 0x01; //Enable timer
}

void setupDelayNs()
{
	/*Assuming that PLL0 has been setup with CCLK = 100Mhz and PCLK = 25Mhz.*/
	LPC_SC->PCONP |= (1<<2); //Power up TIM1.
	LPC_SC->PCLKSEL1 &= ~(0x3<<3);

	LPC_TIM1->CTCR = 0x0;
	LPC_TIM1->PR = (1 - 1); //Increment TC at every 25 clock cycles (40ns)

    LPC_TIM1->TCR = 0x02; //Reset timer
}

void delay40Ns(int nanoseconds)
{
    LPC_TIM1-> TCR = 0x02; // Reset timer
    LPC_TIM1-> TCR = 0x01; // Enable timer
    while(LPC_TIM1->TC < nanoseconds); //Wait nanoseconds
    LPC_TIM1->TCR = 0x00; //Disable timer
}


uint32_t timems = 0;

void delayms(int ms)
{
	int start = timems;
	while (timems - start < ms) {}
}

void SysTick_Handler(void)
{
	timems++;
}