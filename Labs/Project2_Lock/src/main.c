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
#include <uart.h>           // Interrupt UART library with receive/transmit circular buffers

// Define LEDS for "Lock" Status 
#define LED_A_Green PC5
#define LED_A_Red PC4
#define LED_B_Green PC3
#define LED_B_Red PC2

// Define ENCODER for choosing the "Lock" State.
#define OutputCLK PD1
#define OutputDT PD2
#define OutputSW PD3

// Define Servo for PWM output of drive signal of Arduino


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
    GPIO_mode_input_nopull(&DDRD,OutputCLK);
    GPIO_mode_input_nopull(&DDRD,OutputDT);
    GPIO_mode_input_pullup(&DDRD,OutputSW);

    Last_State = GPIO_read(&PIND,OutputCLK);

    // Initialize display
    lcd_init(LCD_DISP_ON);

    GPIO_mode_output(&DDRC,LED_A_Green);
    GPIO_mode_output(&DDRC,LED_A_Red);
    GPIO_mode_output(&DDRC,LED_B_Green);
    GPIO_mode_output(&DDRC,LED_B_Red);






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
    
    // Engage Timer at 4ms(functional for our purposes)
    TIM1_overflow_4ms();
    TIM1_overflow_interrupt_enable();
    

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // sei Enables interrupts by setting the global interrupt mask
    sei();

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
 **********************************************************************/

ISR(TIMER1_OVF_vect)
{
  char string[4];
  State = GPIO_read(&PIND,OutputCLK);
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
      lcd_puts(string);
  }

Last_State = State;
SW_ENC = GPIO_read(&PIND,OutputSW);
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
  GPIO_write_high(&PORTC,LED_A_Green);
  GPIO_write_high(&PORTC,LED_A_Red);
  GPIO_write_high(&PORTC,LED_B_Green);
  GPIO_write_high(&PORTC,LED_B_Red);

  
  /* Encoder status for LCD -----------------------------------------*/
  if( counter == 0 || counter == 0 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                   "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Null");  
    } 
  
  
  if( counter == 1 || counter == -1 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                   "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Unlock A");  
    }
    if( counter == 2 || counter == -2 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                 "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Lock A");  
    }
    if( counter == 3 || counter == -3 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Unlock B");  
    }
    if( counter == 4 || counter == -4 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("               "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Lock B");  
    }
      if( counter == 5 || counter == -5 )
  {  
      lcd_gotoxy(6, 1);
      lcd_puts("                "); 
      lcd_gotoxy(6, 1);
      lcd_puts("Unlock AB");  
    }
        if( counter == 6 || counter == -6 )
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

/* Set Position Servo("Lock") when Encoder SW is pressed -----------------------------------------*/
if( SW_ENC == 0 && (counter == 1 || counter == -1) )
  {  
      GPIO_write_low(&PORTC,LED_A_Green);
    } 

if( SW_ENC == 0 && (counter == 2 || counter == -2) )
  {  
      GPIO_write_low(&PORTC,LED_A_Red);
    } 

if( SW_ENC == 0 && (counter == 3 || counter == -3) )
  {  
      GPIO_write_low(&PORTC,LED_B_Green);
    } 

if( SW_ENC == 0 && (counter == 4 || counter == -4) )
  {  
      GPIO_write_low(&PORTC,LED_B_Red);
    } 

if( SW_ENC == 0 && (counter == 5 || counter == -5) )
  {  
      GPIO_write_low(&PORTC,LED_A_Green);
      GPIO_write_low(&PORTC,LED_B_Green);
    } 

if( SW_ENC == 0 && (counter == 6 || counter == -6) )
  {  
      GPIO_write_low(&PORTC,LED_A_Red);
      GPIO_write_low(&PORTC,LED_B_Red);
    } 




}