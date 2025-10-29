#include <LPC17xx.h>
#include <stdlib.h> // For abs()

/*
 * --- Global Variables ---
 */

// Volatile variables to share between main() and the ISR
volatile unsigned int adc_val4 = 0;
volatile unsigned int adc_val5 = 0;
volatile unsigned int adc_diff = 0;

// LCD function helper variables (adapted from your token system code)
unsigned long int temp1_lcd = 0, temp2_lcd = 0;
unsigned int i;
unsigned char flag1_lcd = 0, flag2_lcd = 0;
unsigned char str_buffer[16];

// LCD Pin Definitions
#define RS_CTRL 0x08000000  // P0.27
#define EN_CTRL 0x10000000  // P0.28
#define DT_CTRL 0x07800000  // P0.23 to P0.26

/*
 * --- LCD Functions (Adapted from your Queue Token Management code) ---
 */

void delay_lcd(unsigned int r1) {
    unsigned int r;
    for (r = 0; r < r1; r++);
}

void port_write(void) {
    LPC_GPIO0->FIOPIN = temp2_lcd;
    if (flag1_lcd == 0)
        LPC_GPIO0->FIOCLR = RS_CTRL;
    else
        LPC_GPIO0->FIOSET = RS_CTRL;
    
    LPC_GPIO0->FIOSET = EN_CTRL;
    delay_lcd(25000);
    LPC_GPIO0->FIOCLR = EN_CTRL;
    delay_lcd(500000);
}

void lcd_write(void) {
    flag2_lcd = (flag1_lcd == 1) ? 0 : ((temp1_lcd == 0x30) || (temp1_lcd == 0x20)) ? 1 : 0;
    temp2_lcd = temp1_lcd & 0xF0;
    temp2_lcd = temp2_lcd << 19;
    port_write();
    if (!flag2_lcd) {
        temp2_lcd = temp1_lcd & 0x0F;
        temp2_lcd = temp2_lcd << 23;
        port_write();
    }
}

void lcd_display(unsigned char *msg) {
    flag1_lcd = 1;
    i = 0;
    while (msg[i] != '\0') {
        temp1_lcd = msg[i];
        lcd_write();
        i++;
    }
}

void int_to_string(unsigned int n, unsigned char *str) {
    int i = 0, j;
    unsigned char temp[6];
    if (n == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    while (n > 0) {
        temp[i++] = (n % 10) + '0';
        n /= 10;
    }
    for (j = 0; j < i; j++) {
        str[j] = temp[i - j - 1];
    }
    str[i] = '\0';
}

/*
 * --- ADC Interrupt Handler ---
 */
void ADC_IRQHandler(void)
{
    long x; // Use long to hold the full 32-bit register value
    
    // Read the ADC status register
    x = LPC_ADC->ADSTAT;
    
    // Check if channel 4 interrupt is active
    if (x & (1<<4)) 
    {
        // Read the value, extract the 12-bit result [15:4], and clear the flag
        adc_val4 = (LPC_ADC->ADDR4 >> 4) & 0xFFF; 
    }
    
    // Check if channel 5 interrupt is active
    if (x & (1<<5))
    {
        // Read the value, extract the 12-bit result [15:4], and clear the flag
        adc_val5 = (LPC_ADC->ADDR5 >> 4) & 0xFFF;
    }

    // --- THIS IS THE COMPLETED CODE ---
    
    // 1. Calculate the absolute difference
    adc_diff = abs(adc_val4 - adc_val5);

    // 2. Set cursor to the beginning of the first line (0x80)
    flag1_lcd = 0; // 0 = command
    temp1_lcd = 0x80;
    lcd_write();

    // 3. Display the "Diff: " label
    lcd_display((unsigned char*)"Diff: ");
    
    // 4. Convert the integer difference to a string
    int_to_string(adc_diff, str_buffer);
    
    // 5. Display the number
    lcd_display(str_buffer);
    
    // 6. Display blank spaces to clear any old digits
    lcd_display((unsigned char*)"    "); 
}


/*
 * --- Main Function ---
 */
int main()
{
    unsigned long int init_command[] = { 0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x06, 0x01, 0x80 };

    SystemInit();
    SystemCoreClockUpdate();

    // --- LCD Setup ---
    // 1. Set LCD pins (P0.23-P0.28) as output
    LPC_GPIO0->FIODIR |= DT_CTRL | RS_CTRL | EN_CTRL;
    
    // 2. Initialize the LCD module
    flag1_lcd = 0;
    for (i = 0; i < sizeof(init_command)/sizeof(init_command[0]); i++) {
        temp1_lcd = init_command[i];
        lcd_write();
    }
    lcd_display((unsigned char*)"ADC Initialized");

    // --- ADC Setup ---
    // 1. Configure P1.30 as AD0.4 and P1.31 as AD0.5
    LPC_PINCON->PINSEL3 |= 0x30000000; //p1.30 as AD0.4
    LPC_PINCON->PINSEL3 |= 0xC0000000; //p1.31 as AD0.5
    
    // 2. Power on the ADC peripheral
    LPC_SC->PCONP |= (1<<12);
    
    // 3. Configure ADC:
    //    Select channels 4 & 5
    //    Set BURST mode (1<<16) for continuous conversion
    //    Set PDN bit (1<<21) to make ADC operational
    LPC_ADC->ADCR = (1<<4) | (1<<5) | (1<<16) | (1<<21);
    
    // 4. Enable interrupts for channels 4 and 5
    LPC_ADC->ADINTEN = (1<<4) | (1<<5);
    
    // 5. Enable the ADC interrupt in the main CPU (NVIC)
    NVIC_EnableIRQ(ADC_IRQn);
    
    // 6. Loop forever. The ADC_IRQHandler will do all the work.
    while(1);
}
