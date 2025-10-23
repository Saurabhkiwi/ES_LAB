#include "LPC17xx.h"
#include <stdio.h>

unsigned long int duty_cycle = 0;
unsigned long int new_duty = 0;
int press_count = 0;

void delayMs(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 20000; j++);
}

void PWM1_IRQHandler(void)
{
    if (LPC_PWM1->IR & (1 << 0))
    {
        LPC_PWM1->MR4 = new_duty;
        LPC_PWM1->LER = (1 << 4);
        LPC_PWM1->IR = (1 << 0);
    }
}

int main(void)
{
    char key;

    LPC_PINCON->PINSEL3 |= (2 << 14);
    LPC_PWM1->TCR = (1 << 1);
    LPC_PWM1->CTCR = 0x00;
    LPC_PWM1->PR = 0;
    LPC_PWM1->MR0 = 3000;
    LPC_PWM1->MR4 = duty_cycle;
    LPC_PWM1->MCR = (1 << 1) | (1 << 0);
    LPC_PWM1->PCR = (1 << 12);
    LPC_PWM1->LER = (1 << 0) | (1 << 4);

    NVIC_EnableIRQ(PWM1_IRQn);

    LPC_PWM1->TCR = (1 << 0) | (1 << 3);

    printf("Press any key to change brightness...\n");

    while (1)
    {
        key = getchar();

        if (key)
        {
            press_count++;

            if (press_count == 1)
                new_duty = 300;
            else if (press_count == 2)
                new_duty = 1500;
            else if (press_count == 3)
                new_duty = 2700;
            else
            {
                new_duty = 0;
                press_count = 0;
            }

            printf("Duty cycle set to %lu\r\n", new_duty);

            delayMs(500);
        }
    }
}
