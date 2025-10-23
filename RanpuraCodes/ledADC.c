#include <stdio.h>
#include <lpc17xx.h>
unsigned long result,i;
void ADC_IRQHandler(void);
int main()
{
	LPC_SC->PCONP = 1<<12;
	LPC_PINCON->PINSEL0=0;
	LPC_PINCON->PINSEL1=0;
	LPC_GPIO0->FIODIR = 0xFF<<15;
	LPC_ADC->ADCR = 1<<6|1<<21|1<<16;
	LPC_ADC->ADINTEN = 1<<6;
	NVIC_EnableIRQ(ADC_IRQn);
	while(1);
}

void ADC_IRQHandler()
{
	result = (LPC_ADC->ADGDR & 0xFFF<<4) >> 4; //<<4 prev
	LPC_GPIO0->FIOPIN = result<<15;
	for(i=0;i<10;i++);
}