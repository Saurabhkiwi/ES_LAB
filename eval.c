#include <LPC17xx.h>

void delay_ms(unsigned int time)
{
    unsigned int i, j;
    for(i = 0; i < time; i++)
        for(j = 0; j < 10000; j++);   // ~1ms delay (rough)
}

int main(void)
{
    uint8_t johnson = 0;  // 8-bit Johnson counter register
    int i;

    // Configure pins as GPIO
    LPC_PINCON->PINSEL3 &= ~(0xFF << 14);  // P1.23–P1.26 as GPIO
    LPC_PINCON->PINSEL4 &= ~(0xFF << 20);  // P2.10–P2.13 as GPIO

    // Set directions: output
    LPC_GPIO1->FIODIR |= (0xF << 23);  // P1.23–P1.26
    LPC_GPIO2->FIODIR |= (0xF << 10);  // P2.10–P2.13

    while (1)
    {
        // Johnson counter: Shift left and insert inverted MSB at LSB
        for (i = 0; i < 8; i++)
        {
            johnson = (johnson << 1) | (~(johnson >> 7) & 0x01);

            // Output lower 4 bits on P1.23–P1.26
            LPC_GPIO1->FIOCLR = (0xF << 23);
            LPC_GPIO1->FIOSET = ((johnson & 0x0F) << 23);

            // Output upper 4 bits on P2.10–P2.13
            LPC_GPIO2->FIOCLR = (0xF << 10);
            LPC_GPIO2->FIOSET = (((johnson >> 4) & 0x0F) << 10);

            delay_ms(300);  // visible LED delay
        }
    }
}
