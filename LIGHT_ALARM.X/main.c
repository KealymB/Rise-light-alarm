/*
 * Author: Kealym Bromley
 * Rise - Light alarm
 * Using a PIC16F1827, a DS3231 RTC connected to I2C pins 6 and 9 and a POT connected to pin 1 
 * an alarm is set using the potentiometer value and when the alarm is active the LED bank on pin 6 is faded on 
 */
#include "mcc_generated_files/mcc.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define _XTAL_FREQ 32000000 //set clock as 32Mhz

#define RTC_ADD 0b1101000 //need a one at the end?

void ADC2TIME();
int BIT2SECMIN(uint8_t time);
int BIT2HOUR(uint8_t time);

bool alarmFlag;
int alarmMin;
int alarmHour;
int alarmLength;

void main(void)
{
    SYSTEM_Initialize();
    
    TRISA3 = 0; //sets pin RA3 as an output pin for LED

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    //below is code used to set the time. This should only need to happen once...
    
    //i2c_write1ByteRegister(RTC_ADD, 0x02, 0b00010011); //enable 24-hour format and set hours to 13
    //i2c_write1ByteRegister(RTC_ADD, 0x01, 0b00100001); //set minutes to 25 min
    
    PWM3_LoadDutyValue(1); //need a better way of turning on and off LED
    
            
    alarmMin = 42; //will be set by pot
    alarmHour = 14; //will be set by pot
    alarmLength = 1; //default time that the LED will stay lit
    
    uint16_t increment = 1; //change in brightness each pass
    uint16_t brightness = increment; //current brightness
    uint16_t maxBrightness = 300; //max brightness
    
    while (1)
    {
        //ADC2TIME();     
        
        int seconds = BIT2SECMIN(i2c_read1ByteRegister(RTC_ADD, 0x00));  
        int minutes = BIT2SECMIN(i2c_read1ByteRegister(RTC_ADD, 0x01));  
        int hours = BIT2HOUR(i2c_read1ByteRegister(RTC_ADD, 0x02));  
        
        printf("HH:MM:ss %d:%d:%d\r\n", hours, minutes, seconds);
        
        if(minutes == alarmMin && hours == alarmHour && !alarmFlag){
            alarmFlag = true;
        }
        
        if(alarmFlag && minutes < alarmMin+alarmLength){
            PWM3_LoadDutyValue(brightness);
            
            if(brightness <= maxBrightness){
                brightness += increment; //will increase brightness quickly
                __delay_ms(250);
            }
        }
        
        if(minutes >= alarmMin+alarmLength && alarmFlag){
            alarmFlag = false;
            PWM3_LoadDutyValue(1); //need a better way of turning on and off LED
        }
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
    
    H &= 0b00110000; //get the tens of the time as its 24-hour
    
    timeReturn = (H>>4) * 10; //shift and multiply to get it in decimal tens
    
    return timeReturn + (time & 0b00001111); // add the units back
}
