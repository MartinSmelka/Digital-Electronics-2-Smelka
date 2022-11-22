/***********************************************************************
 * 
 * Stopwatch by Timer/Counter2 on the Liquid Crystal Display (LCD)

 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2017 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 * Components:
 *   16x2 character LCD with parallel interface
 *     VSS  - GND (Power supply ground)
 *     VDD  - +5V (Positive power supply)
 *     Vo   - (Contrast)
 *     RS   - PB0 (Register Select: High for Data transfer, Low for Instruction transfer)
 *     RW   - GND (Read/Write signal: High for Read mode, Low for Write mode)
 *     E    - PB1 (Read/Write Enable: High for Read, falling edge writes data to LCD)
 *     D3:0 - NC (Data bits 3..0, Not Connected)
 *     D4   - PD4 (Data bit 4)
 *     D5   - PD5 (Data bit 5)
 *     D6   - PD6 (Data bit 3)
 *     D7   - PD7 (Data bit 2)
 *     A+K  - Back-light enabled by PB2
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD screen when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/
uint8_t customChar[8] = {
    0b00111,
    0b01110,
    0b11100,
    0b11000,
    0b11100,
    0b01110,
    0b00111,
    0b00011
};



int main(void)
{
    lcd_init(LCD_DISP_ON);
    lcd_gotoxy(1, 0); lcd_puts("value:");
    lcd_gotoxy(3, 1); lcd_puts("key:");
    lcd_gotoxy(8, 0); lcd_puts("a");  // Put ADC value in decimal
    lcd_gotoxy(13,0); lcd_puts("b");  // Put ADC value in hexadecimal
    lcd_gotoxy(8, 1); lcd_puts("c");  // Put button name here


    ADMUX = ADMUX  | (1<<REFS0) | (1<<REFS1);
    // Select input channel ADC0 (voltage divider pin)
    ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<MUX0 );
    // Enable ADC module
    ADCSRA |= (1<<ADEN);
    // Enable conversion complete interrupt
    ADCSRA |= (1<<ADIE);    
    // Set clock prescaler to 128
    ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

    ADCSRB |= (1<<ADEN);
    // Enable conversion complete interrupt
    ADCSRB |= (1<<ADIE);    
    // Set clock prescaler to 128
    ADCSRB |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

    // Configuration of 8-bit Timer/Counter2 for Stopwatch update
    // Set the overflow prescaler to 16 ms and enable interrupt

    TIM2_overflow_1ms();
    TIM2_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter2 overflow interrupt
 * Purpose:  Update the stopwatch on LCD screen every sixth overflow,
 *           ie approximately every 100 ms (6 x 16 ms = 100 ms).
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
  static uint8_t no_of_overflows = 0;
  no_of_overflows++;
    if (no_of_overflows >= 3)
    {
    // Start ADC conversion
    ADCSRA |= (1<<ADSC);
    }
    // Else do nothing and exit the ISR
}

ISR(ADC_vect)
{
    uint16_t value;
    char string[4];  // String for converted numbers by itoa()

    // Read converted value
    // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
    value = ADC;
    // Convert "value" to "string" and display it
    itoa(value, string, 10);
    lcd_gotoxy(8, 0);
    lcd_puts("    ");
    lcd_gotoxy(8, 0);
    lcd_puts(string);

    itoa(value, string, 16);
    lcd_gotoxy(13, 0);
    lcd_puts("    ");
    lcd_gotoxy(13, 0);
    lcd_puts(string);

    if (value == 1023)
    {
    lcd_gotoxy(8, 1);
    lcd_puts("     ");
    lcd_gotoxy(8, 1);
    lcd_puts("None");
    }
    
    if (value == 0)
    {
    lcd_gotoxy(8, 1);
    lcd_puts("     ");
    lcd_gotoxy(8, 1);
    lcd_puts("Right");
    }

    if (value == 410)
    {
    lcd_gotoxy(8, 1);
    lcd_puts("      ");
    lcd_gotoxy(8, 1);
    lcd_puts("Left");
    }
   
    if (value == 257)
    {
    lcd_gotoxy(8, 1);
    lcd_puts("      ");
    lcd_gotoxy(8, 1);
    lcd_puts("Down");
    }

    if (value == 641)
    {
    lcd_gotoxy(8, 1);
    lcd_puts("      ");
    lcd_gotoxy(8, 1);
    lcd_puts("Select");
    }

    if (value == 100)
    {
    lcd_gotoxy(8, 1);
    lcd_puts("      ");
    lcd_gotoxy(8, 1);
    lcd_puts("Up");
    }
    
}
