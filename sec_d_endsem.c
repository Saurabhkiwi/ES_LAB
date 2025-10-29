#include <LPC17xx.h>
#include <stdio.h>  // For sprintf()
#include <math.h>   // For fabsf() (float absolute value)

/*
 * --- Global Variables ---
 */

// Volatile variables to share between main() and the ISR
volatile unsigned int adc_val4 = 0;
volatile unsigned int adc_val5 = 0;

// LCD function helper variables
unsigned long int temp1_lcd = 0, temp2_lcd = 0;
unsigned int i;
unsigned char flag1_lcd = 0, flag2_lcd = 0;

// LCD Pin Definitions
#define RS_CTRL 0x08000000  // P0.27
#define EN_CTRL 0x10000000  // P0.28
#define DT_CTRL 0x07800000  // P0.23 to P0.26

/*
 * --- LCD Functions (Adapted from your previous code) ---
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

/*
 * --- ADC Interrupt Handler ---
 */
void ADC_IRQHandler(void)
{
    long status = LPC_ADC->ADSTAT;
    char line1_buf[17]; // 16 chars + null terminator
    char line2_buf[17];
    
    // Check if channel 4 interrupt is active
    if (status & (1<<4)) 
    {
        // Read the value, extract the 12-bit result [15:4]
        adc_val4 = (LPC_ADC->ADDR4 >> 4) & 0xFFF; 
    }
    
    // Check if channel 5 interrupt is active
    if (status & (1<<5))
    {
        // Read the value, extract the 12-bit result [15:4]
        adc_val5 = (LPC_ADC->ADDR5 >> 4) & 0xFFF;
    }

    // --- THIS IS THE COMPLETED CODE ---
    
    // 1. Calculate Voltages
    // Assuming Vref = 3.3V and 12-bit ADC (0-4095)
    float volt4 = (adc_val4 * 3.3) / 4095.0;
    float volt5 = (adc_val5 * 3.3) / 4095.0;
    float volt_diff = fabsf(volt4 - volt5); // Absolute difference

    // 2. Format the strings for display
    sprintf(line1_buf, "V4:%.2fV V5:%.2fV", volt4, volt5);
    sprintf(line2_buf, "Diff: %.2f V", volt_diff);

    // 3. Display on Line 1
    flag1_lcd = 0; // 0 = command
    temp1_lcd = 0x80; // Set cursor to line 1
    lcd_write();
    lcd_display((unsigned char*)line1_buf);
    
    // 4. Display on Line 2
    flag1_lcd = 0;
    temp1_lcd = 0xC0; // Set cursor to line 2
    lcd_write();
    lcd_display((unsigned char*)line2_buf);
    lcd_display((unsigned char*)"   "); // Add padding to clear old text
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
    lcd_display((unsigned char*)"ADC Volt Meter");
    delay_lcd(5000000); // Show message for a moment

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
