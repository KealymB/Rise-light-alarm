#include "mcc_generated_files/mcc.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define _XTAL_FREQ 32000000 //set clock as 32Mhz

#define RTC_ADD 0b1101000 //need a one at the end?
#define RTC_WRITE_ADD 0b1101000

void ADC2TIME();
int BIT2SECMIN(uint8_t time);
int BIT2HOUR(uint8_t time);

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
        //ADC2TIME();
        RB0 = 1; //turn on Pin at RB0
        __delay_ms(500);
        RB0 = 0;
        //ADC2TIME();
        __delay_ms(500);
        
        
        int seconds = BIT2SECMIN(i2c_read1ByteRegister(RTC_ADD, 0x00));  
        int minutes = BIT2SECMIN(i2c_read1ByteRegister(RTC_ADD, 0x01));  
        int hours = BIT2HOUR(i2c_read1ByteRegister(RTC_ADD, 0x02));  
        
        printf("Seconds: %d\r\n", seconds);
        printf("Minutes: %d\r\n", minutes);
        printf("Hours: %d\r\n", hours);
    }
}

void ADC2TIME(){
    //from experiments pot goes from 0 - 950
    //we can make every 15min choose-able
    //24 hour alarm means that every 15 min is about 10 from the ADC
    
    uint16_t potValue = ADC_GetConversion(pot);
    
    int hours = potValue/(9.9*4);
    int min = potValue/(9.9*24)*15; //gives incorrect minutes
    
    printf("Pot value: %dH%dM\r\n", hours, min);
}

int BIT2SECMIN(uint8_t time){ //converts the bits from the RTC into readable integers for both min and seconds
    uint8_t timeReturn = 0;
    uint8_t H = time;
    
    H &= 0b01110000;//get bit 6-4
    
    timeReturn += (H>>4) * 10; //shift and multiply to get it in decimal tens
    
    return timeReturn + (time & 0b00001111); // add the units back
}

int BIT2HOUR(uint8_t time){
    uint8_t timeReturn = 0;
    uint8_t H = time;
    
    timeReturn = H&0b00010000 * 10; //shift and multiply to get it in decimal tens
    
    return timeReturn + (time & 0b00000111); // add the units back
}