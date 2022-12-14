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

## Recommended README.md file structure

### Team members

* Martin Šmelka(App Design, Code, Device Planning)
* Šimon Špánik(Device Planning, Documentation)

## Hardware description

With the help of specified hardware components and Arduino, we created an implementation where we use the Encoder and Joystick to control the LEDs in the shape of a cross and display the data on the LCD.

### Schematic

![image]()

Components are connected to Arduino Uno like on the schematic. 
It's used HD44780 LCD display due to missing Digilent PmodCLP LCD module in SimullDE softwere.

LEDs are connected in active low mode. Joystick is in position of up-right corner to demonstrate the function of circuit.

### Encoder

#### Description

KY-040 is a rotary encoder that provides an output of how much it was turned and in which direction. The supply voltage is 5V and has 20 pulses per 360 degrees. It also includes a switch with a digital output.

Pinout:

![image](https://user-images.githubusercontent.com/99726477/205434930-50e16daf-f3ed-4ef1-aaff-bf3b9e7298a7.png)

Image of two data signals which determine a direction of rotating (taken from datasheet):

![image](https://user-images.githubusercontent.com/99726477/205435020-c9411c68-0b3f-4a49-8d00-867d629956a3.png)


#### Function

Enconder in our project controls the blinking speed of the LEDs. It is driven in both directions. When pressed, the output is reset and the flashing is turned off.


### Digilent PmodCLP LCD module

#### Description

16x2 character LCD display with 8-bit parallel data interface. Supply voltage is 5V. Its controlled by 10-bit instructions. From LSB to MSB are 8-bits of data and the next 2-bits are Read and Write operating bits.

![image](https://user-images.githubusercontent.com/99726477/205435246-52842a16-c7c0-4014-881b-cd2b4b707686.png)

Pinout:

![image](https://user-images.githubusercontent.com/99726477/205435281-7e80ee4f-f1b4-459c-ac3e-b6893d463a5d.png)


#### Function

The LCD displays the coordinates of XY axis, encoder blink rate value and joystick direction with its custom directional arrow characters

### LEDs

LEDs used in the circuit has RED color, Package: DIL

![image](https://user-images.githubusercontent.com/99726477/205602378-859bb8f7-a3a6-496c-943f-c0729d48a317.png)

### Servo

#### Description



Pinout:

![image]()

Image of control PWM (taken from datasheet):

![image]()


#### Function



## Software description
 1. **Timer:** [timer.h](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/include/timer.h)
 2. **Gpio:** [gpio.c](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/lib/gpio/gpio.c), [gpio.h](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/lib/gpio/gpio.h)
 3. **LCD:** [lcd.c](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/lib/lcd/lcd.c), [lcd.h](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/lib/lcd/lcd.h), [lcd_definitions.h](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/lib/lcd/lcd_definitions.h)
 4. **Source:** [main.c](https://github.com/MartinSmelka/Digital-Electronics-2-Smelka/blob/main/Labs/Project2_Lock/src/main.c)

Put flowchats of your algorithm(s):

![main() flowchart]()

![TIMER (1) drawio]()

![last_flow drawio (1)]()

## Video

[Youtube Link to View the Implementation of the Project]()

## References

1. https://www.tme.eu/Document/07d3a201631d75ac52980710936cebec/410-142P.pdf
2. [SG90](http://www.ee.ic.ac.uk/pcheung/teaching/DE1_EE/stores/sg90_datasheet.pdf)
3. [KY040](https://www.rcscomponents.kiev.ua/datasheets/ky-040-datasheet.pdf)
