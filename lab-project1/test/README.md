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
