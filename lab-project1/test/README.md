# Project 1
## Team members
- Nestor Gorrotxategi Agirre
- Dragos Bratfalean
## Hardware description
![Image of all hardware](https://github.com/NestorGorrotxategi/digitalelectronics2/blob/main/lab-project1/Schematic.png)

As we see in the picture above, we have used the following hardware for our project:
- Arduino Uno with AtmelMega328P MCU.
- Joystick with 2ADC signals and a switch.
- A rotary encoder with a switch.
- Digilent PmodCLP LCD module.
- A breadboard.

We have carried out some research on every piece of hardware that we have been asked to implement for the project:

### Arduino Uno ATmega328P

Arduino Uno is a microcontroller board based on ATmega328P MCU. It offers 14 digital input/output pins and 6 analog inputs, and a 16MHZ clock signal among others.

Using AVR microcontroller, pins are associated into so-called ports, which are marked with the letters A,B,C,... Each of the pins is controlled independently and can either work as input or output point of the MCU. Control is only possible ussing control registers.

Our hardware has been connected to these pins, and we have used different registers and commands for the software implementation.

### Joystick

The basic principle of the analog joystick is a set of two interconnected potentiometers which provide information about vertical (Y-axis) and horizontal (X-axis) movement. In addition, the joystick also comes with a Select switch.

Apart from the VCC and GND pins for the Joystick which are connected to their corresponding pins in the breadboard, it has 3 other connections: 2 analog signals (Vrx for X and Vry for Y axis information) and a digital signal (switch). 

The first signals have to be processed by ADC channels. This is why Vrx and Vry have been connected respectively to A0 and A1 ADC channels on the Arduino board as we see on the scheme.

As far as the switch connection is concerned, as it is a digital signal, it has been connected to digital pin 13 (PB5) as an input.

### Rotary encoder

A rotary encoder is a position sensor which generates an electric digital signal and provides information about the angular position of the rotating shaft. In addition, it has a push button, which also works as a digital signal.

Briefly, functioning of rotary encoders will be explained. The encoder has a disk with evenly spaced contact zones connected to a common pin. In addition, it has two other separate contact pins A and B, which are decisive for the correct working of the encoder. 

When the disk rotates step by step, A and B pins make contact with the common pin, so two output signals will be generated. Comparing both signals we are able to know the rotated position of the encoder.

In our case, Output A has been connected to digital pin 10 (PB2, CLK signal), while Output B is connected to digital pin 11 (PB3, DT signal). Both pins have been defined as input signals.

The SW button pin has been connected to digital pin 12 (PB4) as an input.



## Software description


### int main()

* initializes the LCD screen
* configure the ADC
* sets overflows for Timer1 and Timer2
* enables interupts
* sets infinite loop for working device

![Image of all hardware](https://github.com/NestorGorrotxategi/digitalelectronics2/blob/main/lab-project1/flowcharts/main.png)




### ISR(Timer1)

* Interupt service which switches the ADC channel every 100 ms

![Image of all hardware](https://github.com/NestorGorrotxategi/digitalelectronics2/blob/main/lab-project1/flowcharts/TIMER1.jpg)




### ISR(Timer2)

* Interupt service which overflows every 4 ms
* Reads digital data sent by Encoder (Displays "Rear Camera" if you push the incoder. If not it displays acceleration or decelration depending on the sense of rotation)
* Reads the pushbutton from the Joystick( Displays "BUZZER" if the button is pushed)

![Image of all hardware](https://github.com/NestorGorrotxategi/digitalelectronics2/blob/main/lab-project1/flowcharts/TIMER2.png)




### ISR(ADC_vect)

* Interupt service which is responsible for displaying the direction accordingly to axis values of the joystick
* Displays on the lcd the direction whick the joystick indicates(up-right, up-left, down-right ,down-left)

![Image of all hardware](https://github.com/NestorGorrotxategi/digitalelectronics2/blob/main/lab-project1/flowcharts/ADC_vect.png)




## Video
## References
