/***********************************************************************
 * DE2-Project 1:
 * Martin Šmelka, Šimon Špánik
 * Use of Analog-to-digital conversion to transalte Joysitc movements to an array of LEDs and represend the position of said 
 * joystic with the possiblity of modulating the signal frequency and cause the LEDs to flicker. All of these functions are then
 * Represented on a LCD screen via UART and LCD libraries. 
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO, KY-040 Rotary encoder, KY-023 Joystick, 5 ressistors for LEDs, 5 LEDs arranged
 * in a cross pattern to represent the position of the Joystic.
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

// Define LEDS for directional output
#define LED_Up PB2
#define LED_Right PB3
#define LED_Center PC3
#define LED_Left PC4
#define LED_Down PC5

// Define ENCODER for modulating the frequency of the Flicker of the signal send to the LEDs and use as button to reset the value.
#define OutputCLK PD1
#define OutputDT PD2
#define OutputSW PD3

// Define JOYSTICK for digital input of the Button built into the KY-023 Joystic.
#define OutputJOY PC2

// Static VARIABLES for functions 
static int8_t counter = 0; // Counter to Cap the Encoder
static uint8_t State; // Encoder State 
static uint8_t Last_State; // Encoder Last State(Used as referral)
static uint8_t SW_ENC = 1; // Base Digital signal of the encoder Switch
static uint8_t SW_JOY = 1; // Base Digital signal of the joystick Switch
static uint8_t internalCounter = 0; // Internal Counter Variable for use in LED Flicker

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Ininitialize display, set up pullup and nopull for JOYSTICK and ENCODER
 *           Prepare Custom Characters for use on the LCD
 *           Configure Analog-to-Digital Convertion unit
 *           Engage Timer at 4ms(functional for our purposes)
 *           Enable Global interrupts by setting the global interrupt mask
 **********************************************************************/
int main(void)
{
    GPIO_mode_input_nopull(&DDRD,OutputCLK);
    GPIO_mode_input_nopull(&DDRD,OutputDT);
    GPIO_mode_input_pullup(&DDRD,OutputSW);
    GPIO_mode_input_pullup(&DDRC,OutputJOY);

    Last_State = GPIO_read(&PIND,OutputCLK);

    // Initialize display
    lcd_init(LCD_DISP_ON);

    GPIO_mode_output(&DDRB,LED_Up);
    GPIO_mode_output(&DDRB,LED_Right);
    GPIO_mode_output(&DDRC,LED_Center);
    GPIO_mode_output(&DDRC,LED_Left);
    GPIO_mode_output(&DDRC,LED_Down);





 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Custom character definition using https://omerk.github.io/lcdchargen/ , for Arrow pointing up and down, diagonal arrows 
// and a center character(dot).
// These characters were not found in the dedicated ASCII symbol library.
// We use 56 bits to store the 7 characters(8 bits per character) in adresses 0x00-0x06
    uint8_t customChar[56] = {


	  0b00000,
	  0b00000,
	  0b01110,
	  0b11111,
	  0b11111,
	  0b01110,
	  0b00000,
	  0b00000,// Center(Dot)
    0b00000,
    0b00000,
    0b00000,
    0b00100,
    0b00100,
    0b10101,
    0b01110,
    0b00100,// Arrow Down
    0b00000,
    0b00000,
    0b00000,
    0b00100,
    0b01110,
    0b10101,
    0b00100,
    0b00100,// Up
    0b00000,
    0b00000,
    0b01111,
    0b00011,
    0b00101,
    0b01001,
    0b10000,
    0b00000,  // Diagonal R-U(Right-Up)
    0b00000,
    0b00000,
    0b00001,
    0b10010,
    0b10100,
    0b11000,
    0b11110,
    0b00000, // Diagonal L-D(Left-Down)
    0b00000,
    0b00000,
    0b11110,
    0b11000,
    0b10100,
    0b10010,
    0b00001,
    0b00000, // Diagonal L-U(Left-Up)
    0b00000,
    0b00000,
    0b10000,
    0b01001,
    0b00101,
    0b00011,
    0b01111,
    0b00000 // Diagonal R-D(Right-Down)

    };


    lcd_command(1<<LCD_CGRAM);       // Set addressing to CGRAM (Character Generator RAM)
                                     // ie to individual lines of character patterns
    for (uint8_t i = 0; i < 56; i++)  // Copy new character patterns line by line to CGRAM
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
  lcd_puts("ENCDR:");
  //lcd_putc(0x01); //for testing Characters

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
     // Encoder capped at 10 speeds of flicker (with negative mirror = 20 values)
      if(counter>10)
        counter = 10;
      else if(counter<-10)
        counter = -10;

      itoa(counter, string, 10);
      lcd_gotoxy(6, 0);
      lcd_puts("    "); // Input empty space to refresh LCD cursor for new imput, this is used most cases 
      lcd_gotoxy(6, 0);
      lcd_puts(string);
  }

Last_State = State;
     // Value reset via utilizing the Encoder button function
SW_ENC = GPIO_read(&PIND,OutputSW);

    if (SW_ENC == 0)
    {

        counter = 0;

        itoa(counter, string, 10);
        lcd_gotoxy(6, 0);
        lcd_puts("     ");
        lcd_gotoxy(6, 0);
        lcd_puts(string);
    }

}


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: ADC_vect function where the ADC converts Analog Joystick input into useable Digital 
 * Purpose:  Adress shift due to single ADC limitation
 *           Display X and Y variable as they are converted and read
 *           Set up overflow
 **********************************************************************/

ISR(ADC_vect)
{
  char charX[4];
  char charY[4];
  uint16_t X;
  uint16_t Y;
  uint8_t channel = 0;


  static int8_t nooverflow = 0;
  nooverflow++;
  if(nooverflow > 3)
  {
    nooverflow = 0;
    internalCounter++;
  }
  
  /* address shifting ADMUX -----------------------------------------*/

  if (channel == 0)
  {
    // Input channel ADC0 for adress
    ADMUX = ADMUX & ~( 1<<MUX3 | 1<<MUX2 | 1<<MUX0 | 1<<MUX1);
    ADCSRA |= (1<<ADSC);
    X = ADC;
    // Display X variable
    itoa(X, charX, 10);
    lcd_gotoxy(0,1);
    lcd_puts("       ");
    lcd_gotoxy(0,1);
    lcd_puts("X:");
    lcd_gotoxy(2,1);
    lcd_puts(charX);

    channel = 1;  // Switch to next channel = trigger if condition
  }
  if (channel == 1)
  {
    // Input channel ADC1 for adress
    ADMUX &= ~((1<<MUX3 | 1<<MUX2 | 1<<MUX1)); ADMUX |= (1<<MUX0);
    ADCSRA |= (1<<ADSC);
    Y = ADC;
    // Display Y variable    
    itoa(Y, charY, 10);
    lcd_gotoxy(7,1);
    lcd_puts("       "); 
    lcd_gotoxy(7,1);
    lcd_puts("Y:");
    lcd_gotoxy(9,1);
    lcd_puts(charY);

    channel = 0; // Switch back to the previous channel = trigger if condition(endless loop for refresh)
  }
  
  /* Turning all LEDs OFF with permanent High on driving pins ------------------------------------------------*/
  GPIO_write_high(&PORTB,LED_Up);
  GPIO_write_high(&PORTB,LED_Right);
  GPIO_write_high(&PORTC,LED_Center);
  GPIO_write_high(&PORTC,LED_Left);
  GPIO_write_high(&PORTC,LED_Down);
  
  /* Blinking LEDS and directional fucntions -----------------------------------------*/
  /* Using arrays of possible location of the joystic in two dimensions represented by two variables X and Y we can activate coresponding LEDs -----------------------------------------*/
  if( 1025 > X && X > 850 &&  550 > Y && Y > 450 )
  {
    for(uint8_t i = 0; i<11; i++)
    {
      if(abs(counter)==i)
      {
        if(internalCounter%(11-i) == 0) // We use Mod to check for the opposite number in the 0-10 array
        {
          GPIO_write_low(&PORTB,LED_Right);
        }
        else if(i == 0)
        {
          GPIO_write_low(&PORTB,LED_Right);
        } 
    }
    }    
      lcd_gotoxy(11, 0);
      lcd_puts("      "); 
      lcd_gotoxy(11, 0);
      lcd_putc(0x7e);  //We use standard and custom ASCII symbols to show direction on the LCD
    }

  /* X-axis Directional output via LED -----------------------------------------*/
  else if ( X < 50 && X > 0 && 450 < Y && Y < 550 )
  {
    for(uint8_t i = 0; i<11; i++)
    { 
      if(abs(counter)==i)
      {
        if(internalCounter%(11-i) == 0)
        {
            GPIO_write_low(&PORTC,LED_Left);
        }
        else if(i == 0)
        {
          GPIO_write_low(&PORTC,LED_Left);
        } 
    }
    }
    lcd_gotoxy(11, 0);
    lcd_puts("      ");
    lcd_gotoxy(11, 0);
    lcd_putc(0x7f);
  }
/* Central Directional output via LED -----------------------------------------*/
  else if (600 > X && X > 400 && 550 > Y && Y > 400 )
  {
    for(uint8_t i = 0; i<11; i++)
    {  
      if(abs(counter)==i)
      {
        if(internalCounter%(11-i) == 0)
        {
          GPIO_write_low(&PORTC,LED_Center);
        } 
        else if(i == 0)
        {
          GPIO_write_low(&PORTC,LED_Center);
        }
    }
    }
    lcd_gotoxy(11, 0);
    lcd_puts("      ");
    lcd_gotoxy(11, 0);
    lcd_putc(0x00);
  }

 /* Y-axis Directional output via LED -----------------------------------------*/ 
  if(550 > X && X > 450 && 1025 > Y && Y > 850)
  {
    for(uint8_t i = 0; i<11; i++)
    {  
      if(abs(counter)==i){
        if(internalCounter%(11-i) == 0)
        {
          GPIO_write_low(&PORTB,LED_Up);
        } 
        else if(i == 0)
        {
          GPIO_write_low(&PORTB,LED_Up);
        }
    }
    }
    lcd_gotoxy(11, 0);
    lcd_puts("      ");
    lcd_gotoxy(11, 0);
    lcd_putc(0x02);
  }

  if(600 > X && X > 400 && 200 > Y && Y >= 0)
  {
    for(uint8_t i = 0; i<11; i++)
    {  
      if(abs(counter)==i)
      {
        if(internalCounter%(11-i) == 0)
        {
          GPIO_write_low(&PORTC,LED_Down);
        } 
        else if(i == 0)
        {
          GPIO_write_low(&PORTC,LED_Down);
        }
    }
    }
    lcd_gotoxy(11, 0);
    lcd_puts("      ");
    lcd_gotoxy(11, 0);
    lcd_putc(0x01);
  } 
 /* Middle State Activation (suggested in-class by doc. Ing. Tomáš Frýza, Ph.D.) -----------------------------------------*/ 
 // During class work it came to our attention that middle states of the position pertaining to the joystick being in-between two
 // states was not fully realized, this section was made to rectify this oversight
    if(440 > X && X > 100 && 840 > Y && Y > 550)
  {
    for(uint8_t i = 0; i<11; i++)
    {  
      if(abs(counter)==i){
        if(internalCounter%(11-i) == 0)
        {
          GPIO_write_low(&PORTB,LED_Up);
          GPIO_write_low(&PORTC,LED_Left);
        } 
        else if(i == 0)
        {
          GPIO_write_low(&PORTB,LED_Up);
          GPIO_write_low(&PORTC,LED_Left);
        }
    }
    }
    lcd_gotoxy(11, 0);
    lcd_puts("      ");
    lcd_gotoxy(11, 0);
    lcd_putc(0x05);
  }
  


  if(950 > X && X > 750 && 950 > Y && Y > 700)
  {
    for(uint8_t i = 0; i<11; i++)
    {  
      if(abs(counter)==i){
        if(internalCounter%(11-i) == 0)
        {
          GPIO_write_low(&PORTB,LED_Up);
          GPIO_write_low(&PORTB,LED_Right);
        } 
        else if(i == 0)
        {
          GPIO_write_low(&PORTB,LED_Up);
          GPIO_write_low(&PORTB,LED_Right);
        }
    }
    }
    lcd_gotoxy(11, 0);
    lcd_puts("      ");
    lcd_gotoxy(11, 0);
    lcd_putc(0x03);
  }

    if(950 > X && X > 750 && 350> Y && Y > 50)
  {
    for(uint8_t i = 0; i<11; i++)
    {  
      if(abs(counter)==i){
        if(internalCounter%(11-i) == 0)
        {
          GPIO_write_low(&PORTC,LED_Down);
          GPIO_write_low(&PORTB,LED_Right);
        } 
        else if(i == 0)
        {
          GPIO_write_low(&PORTC,LED_Down);
          GPIO_write_low(&PORTB,LED_Right);
        }
    }
    }
    lcd_gotoxy(11, 0);
    lcd_puts("      ");
    lcd_gotoxy(11, 0);
    lcd_putc(0x06);
  }

      if(500 > X && X > 10 && 350> Y && Y > 50)
  {
    for(uint8_t i = 0; i<11; i++)
    {  
      if(abs(counter)==i){
        if(internalCounter%(11-i) == 0)
        {
          GPIO_write_low(&PORTC,LED_Down);
          GPIO_write_low(&PORTC,LED_Left);
        } 
        else if(i == 0)
        {
          GPIO_write_low(&PORTC,LED_Down);
          GPIO_write_low(&PORTC,LED_Left);
        }
    }
    }
    lcd_gotoxy(11, 0);
    lcd_puts("      ");
    lcd_gotoxy(11, 0);
    lcd_putc(0x04);
  }




 /* All LED Activation -----------------------------------------*/      
  SW_JOY = GPIO_read(&PINC,OutputJOY);

    if (SW_JOY == 0){
      for(uint8_t i = 0; i<11; i++)
      {
        if(abs(counter)==i)
        {
          if(internalCounter%(11-i) == 0)
          {
            GPIO_write_low(&PORTB,LED_Up);
            GPIO_write_low(&PORTB,LED_Right);
            GPIO_write_low(&PORTC,LED_Center);
            GPIO_write_low(&PORTC,LED_Left);
            GPIO_write_low(&PORTC,LED_Down);
          }
          else if(i == 0)
          {
            GPIO_write_low(&PORTB,LED_Up);
            GPIO_write_low(&PORTB,LED_Right);
            GPIO_write_low(&PORTC,LED_Center);
            GPIO_write_low(&PORTC,LED_Left);
            GPIO_write_low(&PORTC,LED_Down);
          }
        } 
      }

    }    
    




}