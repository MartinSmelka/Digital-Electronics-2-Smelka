/***********************************************************************
 * DE2-Project 1:
 * Martin Šmelka, Šimon Špánik
 * Use of 
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO, KY-040 Rotary encoder, 2x Servo, 4 ressistors for LEDs, 4 LEDs arranged
 * in Red-Green pairs to denote the status of selected "Lock" servo.
 *
 * Copyright (c) 2018 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions
#include <twi.h>            // I2C/TWI library for AVR-GCC.
#include <uart.h>  


// Define LEDS for "Lock" Status 
#define LED_A_Green PC5
#define LED_A_Red PC4
#define LED_B_Green PC3
#define LED_B_Red PC2

// Define ENCODER for choosing the "Lock" State.
#define OutputCLK PB4
#define OutputDT PD2
#define OutputSW PC1

// Define Servo for PWM output of drive signal of Arduino

#define SERVOA PD3
#define SERVOB PB2

// Static VARIABLES for functions 
static int8_t counter = 0; // Counter to Cap the Encoder
static uint8_t State; // Encoder State 
static uint8_t Last_State; // Encoder Last State(Used as referral)
static uint8_t SW_ENC = 1; // Base Digital signal of the encoder Switch



/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Ininitialize display, set up pullup and nopull for ENCODER
 *           Prepare Custom Characters for use on the LCD
 *           Engage Timer at 4ms(functional for our purposes)
 *           Enable Global interrupts by setting the global interrupt mask
 **********************************************************************/
int main(void)
{
    GPIO_mode_input_nopull(&DDRB,OutputCLK);
    GPIO_mode_input_nopull(&DDRD,OutputDT);
    GPIO_mode_input_pullup(&DDRC,OutputSW);

    GPIO_mode_output(&DDRD, SERVOA);
    GPIO_mode_output(&DDRB, SERVOB);


    Last_State = GPIO_read(&PINB,OutputCLK);

    // Initialize display
    lcd_init(LCD_DISP_ON);

    GPIO_mode_output(&DDRC,LED_A_Green);
    GPIO_mode_output(&DDRC,LED_A_Red);
    GPIO_mode_output(&DDRC,LED_B_Green);
    GPIO_mode_output(&DDRC,LED_B_Red);


    GPIO_write_high(&PORTC,LED_A_Green);
    GPIO_write_high(&PORTC,LED_A_Red);
    GPIO_write_high(&PORTC,LED_B_Green);
    GPIO_write_high(&PORTC,LED_B_Red);

    
    



 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Custom character definition using https://omerk.github.io/lcdchargen/ , for Lock Logo.
// These characters were not found in the dedicated ASCII symbol library.
// We use 8 bits to store the 1 characters(8 bits per character) in adresse 0x00
    uint8_t customChar[8] = {

    0b11111,
    0b10001,
    0b10001,
    0b11111,
    0b10001,
    0b11011,
    0b11011,
    0b11111
    };


    lcd_command(1<<LCD_CGRAM);       // Set addressing to CGRAM (Character Generator RAM)
                                     // ie to individual lines of character patterns
    for (uint8_t i = 0; i < 8; i++)  // Copy new character patterns line by line to CGRAM
        lcd_data(customChar[i]);
    lcd_command(1<<LCD_DDRAM);       // Set addressing back to DDRAM (Display Data RAM)
                                     // ie to character codes



  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVCC with external capacitor at AREF pin"
    ADMUX = ADMUX | (1<<REFS0);
    // Enable ADC module
    ADCSRA |= (1<<ADEN);
    // Enable conversion complete interrupt
    ADCSRA |= (1<<ADIE);
    // Set clock prescaler to 128
    ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Configurations of registers for SERVO
    TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10) | _BV(WGM11);
    TCCR1B = _BV(WGM13);
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);
    TCCR2B = _BV(WGM22);

    // Engage Timer at 4ms(functional for our purposes)

    TIM2_overflow_16ms();
    TIM2_overflow_interrupt_enable();

    // TIMER2 OVF enable
    TIM1_overflow_33ms();
    TIM1_overflow_interrupt_enable();

    
    

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // sei Enables interrupts by setting the global interrupt mask
    sei();

    //Set DEVIDER to 1.5ms pulse - default
    
    OCR1A = 19999;
    OCR1B = 1499;

    OCR2A = 159;
    OCR2B = 12;

   
    // Infinite loop to keep operations running
    while (1)
    {
      

            /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    return 0;
}

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: TIMER1_OVF_vect function where the timer operates and Encoder is set up
 * Purpose:  Start ADC conversion
 *           LCD Display set up
 *           Encoder set up
 * 
 **********************************************************************/

//Declaration of fuction which turns all LEDs OFF
void f()
{
      GPIO_write_high(&PORTC,LED_A_Green);
      GPIO_write_high(&PORTC,LED_A_Red);
      GPIO_write_high(&PORTC,LED_B_Green);
      GPIO_write_high(&PORTC,LED_B_Red);
};


ISR(TIMER1_OVF_vect)
{
  char string[4];
  State = GPIO_read(&PINB,OutputCLK);
    // Start ADC conversion
    ADCSRA |= (1<<ADSC);

    // LCD Display
  lcd_gotoxy(0, 0);
  lcd_puts("Lock App");
  lcd_gotoxy(8, 0);
  lcd_putc(0x00);
  lcd_gotoxy(0, 1);
  lcd_puts("State:");


    // Encoder checks state to dermine operation to be preformed upon turning the encoder(right from last state +1, left from last state -1)
  if (State != Last_State && State == 1)
  {

      if (GPIO_read(&PIND,OutputDT) != State) 
      {
      counter ++;
      }
      
      else {
      counter --;
      }
     // Encoder capped at 6 for six states(6 lock positions)
      if(counter>6)
        counter = 6;
      else if(counter<-6)
        counter = -6;

      itoa(counter, string, 10);
      lcd_gotoxy(11, 0);
      lcd_puts("    "); // Input empty space to refresh LCD cursor for new imput, this is used most cases 
      
      lcd_gotoxy(11, 0);
      //lcd_puts(string);
  }

Last_State = State;
SW_ENC = GPIO_read(&PINC,OutputSW);
}

ISR(TIMER2_OVF_vect)
{


}

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Locl_vect function where the ADC converts Analog Joystick input into useable Digital 
 * Purpose:  Lock Driving
 *           
 **********************************************************************/

ISR(ADC_vect)
{
  
  /* Turning all LEDs OFF with permanent High on driving pins ------------------------------------------------*/
  //GPIO_write_high(&PORTC,LED_A_Green);
  //GPIO_write_high(&PORTC,LED_A_Red);
  //GPIO_write_high(&PORTC,LED_B_Green);
  //GPIO_write_high(&PORTC,LED_B_Red);

  
  /* Encoder status for LCD -----------------------------------------*/
  if( counter == 0 || counter == 0 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                   "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Null");  
    } 
  
  
  else if( counter == 1 || counter == -1 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                   "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Unlock A");  
    }
  else if( counter == 2 || counter == -2 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                 "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Lock A");  
    }
  else if( counter == 3 || counter == -3 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Unlock B");  
    }
  else if( counter == 4 || counter == -4 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("               "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Lock B");  
    }
  else if( counter == 5 || counter == -5 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Unlock AB");  
    }
  else if( counter == 6 || counter == -6 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Lock AB");  
    }
 /* Encoder SW status for driving the Servos and LEDs -----------------------------------------*/
 /* Reset Servo when Encoder SW is not pressed -----------------------------------------*/
if( SW_ENC == 1)
  {  

    } 

else if( SW_ENC == 0 && counter == 0 )
  {   
      f();
      OCR1B = 1499;
      OCR2B = 12;
            
    } 
/* Set Position Servo("Lock") when Encoder SW is pressed -----------------------------------------*/
else if( SW_ENC == 0 && (counter == 1 || counter == -1) )
  {   
      f();
      GPIO_write_low(&PORTC,LED_A_Green);
      
      OCR1B = 999;

    } 

else if( SW_ENC == 0 && (counter == 2 || counter == -2) )
  {   
      f();
      GPIO_write_low(&PORTC,LED_A_Red);
      
      OCR1B = 1999;      
    } 

else if( SW_ENC == 0 && (counter == 3 || counter == -3) )
  {   
      f();
      GPIO_write_low(&PORTC,LED_B_Green);
      
      OCR2B = 8;
    } 

else if( SW_ENC == 0 && (counter == 4 || counter == -4) )
  {   
      f();
      GPIO_write_low(&PORTC,LED_B_Red);
      
      OCR2B = 16;
    } 

else if( SW_ENC == 0 && (counter == 5 || counter == -5) )
  {   
      f();
      GPIO_write_low(&PORTC,LED_A_Green);
      GPIO_write_low(&PORTC,LED_B_Green);
      
      OCR1B = 999;
      OCR2B = 8;
    } 

else if( SW_ENC == 0 && (counter == 6 || counter == -6) )
  {   
      f();
      GPIO_write_low(&PORTC,LED_A_Red);
      GPIO_write_low(&PORTC,LED_B_Red);
      
      OCR1B = 1999;
      OCR2B = 16;
    } 




}