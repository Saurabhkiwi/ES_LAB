//Adc channel 6 display output on led 15 to 22

#include <LPC17xx.h>
int main(void){
  SystemInit(); LPC_SC->PCONP|=(1<<12);
  LPC_PINCON->PINSEL0|=(3<<22);           // P0.11 as AD0.6
  LPC_GPIO0->FIODIR|=(0xFF<<15);          // P0.15–P0.22 as output
  LPC_ADC->ADCR=(1<<6)|(1<<16)|(1<<21);   // Ch6, enable, burst
  while(1){
    uint16_t r=(LPC_ADC->ADGDR>>4)&0xFFF; // 12-bit ADC
    LPC_GPIO0->FIOPIN=(r>>4)<<15;         // Show upper 8 bits
  }
}
