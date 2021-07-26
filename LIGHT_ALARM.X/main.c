#include "mcc_generated_files/mcc.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define _XTAL_FREQ 32000000 //set clock as 32Mhz


void ADC2TIME();

void main(void)
{
    SYSTEM_Initialize();
    
    TRISB0 = 0; //sets pin RB0 as an output pin for LED

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    while (1)
    {
        ADC2TIME();
        RB0 = 1; //turn on Pin at RB0
        __delay_ms(1000);
        RB0 = 0;
        ADC2TIME();
        __delay_ms(1000);
    }
}

void ADC2TIME(){
    //from experiments pot goes from 0 - 950
    //we can make every 15min choose-able
    //24 hour alarm means that every 15 min is about 10 from the ADC
    
    uint16_t potValue = ADC_GetConversion(pot);
    
    int hours = potValue/(9.9*4);
    int min = (potValue/(9.9*24))*15;
    
    printf("Pot value: %dH%dM\r\n", hours, min);
}