# Project 2

Application of 2 Servos, rotary encoder, and Digilent PmodCLP LCD module.

## Instructions

The goal of the project is cooperation in pairs, further study of the topic, design of own solutions, implementation, creation of project documentation and presentation of results. The division of roles and tasks within the team is up to their members.

* Students work on a project in the labs during the 10th and 11th weeks of the semester.

* Through the BUT e-learning, students submit a link to the GitHub repository, which contains the complete project in PlatfomIO and a descriptive README file. The submission deadline is the day before the next laboratory, i.e. in 2 weeks.

* The source code for the AVR must be written in C and/or Assembly and must be implementable on Arduino Uno board using toolchains from the semester. No other development tools are allowed.

## Recommended GitHub repository structure

   ```c
   YOUR_PROJECT        // PlatfomIO project
   ├── include         // Included files
   ├── lib             // Libraries
   ├── src             // Source file(s)
   │   └── main.c
   ├── test            // No need this
   ├── platformio.ini  // Project Configuration File
   └── README.md       // Report of this project
   ```
## Team members

* Martin Šmelka(App Design, Code, Device Planning, Documentation)
* Šimon Špánik(Code, Device Planning, Documentation, Video Editing)

## Hardware description

With the help of specified hardware components and Arduino, we created an implementation where we use the Encoder to control two servos with assigned LEDs in that denote their state in an application as "Locks". In a real application the Servos in a "Locked" state would move into a position where they would deny the opening of the structure they were installed upon by bracing on a strikeplate.

### Schematic

![image](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Pic/Circuit_Projekt2.png)

Components are connected to Arduino Uno like on the schematic. 
It's using a HD44780 LCD display due to missing Digilent PmodCLP LCD module in SimullDE softwere.

LEDs are connected in active low mode. Encoder is in the 5th position to illustrate the function of the circuit in this state. Waveform analyser to illustrate the PWM used to drive the Servos.

### Arduino UNO

#### Description

Arduino UNO is a microcontroller board based on the ATmega328P [(Datasheet)](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf). It has 14 digital input/output pins (of which 6 can be used as PWM outputs), 6 analog inputs, a 16 MHz ceramic resonator (CSTCE16M0V53-R0), a USB connection, a power jack, an ICSP header and a reset button.

Pinout:

![image](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Pic/UNO_Pinout.png)

#### Function

Chosen as a development board due to being based on the ATmega328P. Used for realisation of our application.

### Encoder

#### Description

KY-040 is a rotary encoder that provides an output of how much it was turned and in which direction. The supply voltage is 5V and has 20 pulses per 360 degrees. It also includes a switch with a digital output.

Pinout:

![image](https://user-images.githubusercontent.com/99726477/205434930-50e16daf-f3ed-4ef1-aaff-bf3b9e7298a7.png)

Image of two data signals which determine a direction of rotating (taken from datasheet):

![image](https://user-images.githubusercontent.com/99726477/205435020-c9411c68-0b3f-4a49-8d00-867d629956a3.png)


#### Function

Enconder in our project controls the the position of both servos (individually as well as both of them at the same time), pressing on the encoder will execute the chosen state untill a new state is chosen.


### Digilent PmodCLP LCD module

#### Description

16x2 character LCD display with 8-bit parallel data interface. Supply voltage is 5V. Its controlled by 10-bit instructions. From LSB to MSB are 8-bits of data and the next 2-bits are Read and Write operating bits.

![image](https://user-images.githubusercontent.com/99726477/205435246-52842a16-c7c0-4014-881b-cd2b4b707686.png)

Pinout:

![image](https://user-images.githubusercontent.com/99726477/205435281-7e80ee4f-f1b4-459c-ac3e-b6893d463a5d.png)


#### Function

The LCD displays the current state of the "Locks", used for displaying encoder values to select a chosen state.

### LEDs

LEDs used in the circuit have both RED and GREEN color, Package: DIL

![image](https://user-images.githubusercontent.com/99726477/205602378-859bb8f7-a3a6-496c-943f-c0729d48a317.png)

#### Function

The LEDs are used to signify a status of their assigned servo "Lock" (GREEN = "Unlocked", RED = "Locked")

### Servo

#### Description
Lightweight servo capable of turning 180 degrees (90 in either way). Lightweight and controlable via generated PWM. 

Pinout:

![image](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Pic/SG90_Pin_out.png)

Image of control PWM (taken from datasheet):

![image](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Pic/SG90_PWM.png)


#### Function

Used as "Locks" switching between two positions of "Unlocked" and "Locked".

## Software description
 1. **Timer:** [timer.h](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/include/timer.h)
 2. **Gpio:** [gpio.c](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/lib/gpio/gpio.c), [gpio.h](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/lib/gpio/gpio.h)
 3. **LCD:** [lcd.c](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/lib/lcd/lcd.c), [lcd.h](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/lib/lcd/lcd.h), [lcd_definitions.h](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/lib/lcd/lcd_definitions.h)
 4. **Source:** [main.c](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/src/main.c)

## Flowcharts

![main() flowchart]()

![TIMER (1) drawio]()

![last_flow drawio (1)]()

## Video

[Youtube Link to View the Implementation of the Project](https://youtu.be/GND5rMOg6Ww)

## References

1. [Digilent PmodCLP LCD module](https://www.tme.eu/Document/07d3a201631d75ac52980710936cebec/410-142P.pdf)
2. [SG90](http://www.ee.ic.ac.uk/pcheung/teaching/DE1_EE/stores/sg90_datasheet.pdf)
3. [KY-040](https://www.rcscomponents.kiev.ua/datasheets/ky-040-datasheet.pdf)
4. [Arduino UNO](https://docs.arduino.cc/resources/datasheets/A000066-datasheet.pdf)
