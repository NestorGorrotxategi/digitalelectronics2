/*****************************************************************************************
 * Teammates: 
 *    NESTOR GORROTXATEGI AGIRRE
 *    DRAGOS BRATFALEAN
 *
 * Project 1
 * 
 * Read ADC signals from Joystick, read signal from rotary encoder and both
 * push buttons and display on LCD screen results.
 * 
 * Application: Control of direction and acceleration/deceleration of a system. Joystick
 *              is used to control direction. When Joystick's push button is
 *              pressed, a buzzer activates.
 *              The rotary encoder is used to control acceleration and braking.
 *              When encoder's push button is pressed, the rear camera activates.
 ********************************************************************************************/

/* Defines ----------------------------------------------------------*/

#define JOYSTICK_PUSH PB5  //Joystick push button definition
#define ENCODER_PUSH PB4  //Encoder push button definition
#define ENCODER_OB PB3    //DT of encoder defined, OUTPUT B
#define ENCODER_OA PB2   //CLK of encoder defined, OUTPUT A


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions
#include "Arduino.h"


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Use Timer/Counter1, Timer/Counter2 and start ADC conversion every 100 ms.
 *           Initialize LCD display.
 *           
 * Returns:  none
 **********************************************************************/

int main(void)
{
    // Initialize display
    lcd_init(LCD_DISP_ON);
    //lcd_gotoxy(1, 0); lcd_puts("Joystick:");
    //lcd_gotoxy(1, 1); lcd_puts("Encoder:");


    //Set digital pins of Joystick and Encoder as input
    GPIO_mode_input_pullup(&DDRB, JOYSTICK_PUSH);
    GPIO_mode_input_pullup(&DDRB, ENCODER_PUSH);
    GPIO_mode_input_pullup(&DDRB, ENCODER_OB);
    GPIO_mode_input_pullup(&DDRB, ENCODER_OA);

  
    // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
    ADMUX = ADMUX | (1<<REFS0);
    // Enable ADC module
    ADCSRA = ADCSRA | (1<<ADEN);
    // Enable conversion complete interrupt
    ADCSRA = ADCSRA | (1<<ADIE);
    // Set clock prescaler to 128
    ADCSRA = ADCSRA | (1<< ADPS2 | 1<<ADPS1 | 1<<ADPS0);

    // Configure 16-bit Timer/Counter1 to start ADC conversion
    // Set prescaler to 33 ms and enable overflow interrupt
    TIM1_overflow_33ms();
    TIM1_overflow_interrupt_enable();

    //Configure 16-bit Timer/Counter2 for the Encoder
    //We set prescaler to 4ms and enable overflow interrupt
    TIM2_overflow_4ms();
    TIM2_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();
    
    // Infinite loop
    while (1){
    // Will never reach this
    }
    return 0;    
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Use single conversion mode and start conversion every 100 ms.
 *           Switch ADC channels from A0 to A1 to read both ADC values.
 **********************************************************************/

ISR(TIMER1_OVF_vect)
{
    //Definition of OVFs to start conversion every 100ms
    static uint8_t no_of_overflows = 0;
    
    //Definition of variable caso to create a condition to switch ADC channels
    static uint8_t caso = 0;

    no_of_overflows++;
    if (no_of_overflows >= 3)
    {
      //Select channel
      if(caso==0){
      caso=1;
      //Channel A0 selected
      ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
      }else{
      caso=0;
      //Channel A1 selected
      ADMUX = ADMUX | (1<<MUX0);
      }
    
      // Start ADC conversion
      ADCSRA = ADCSRA | (1<<ADSC);
      no_of_overflows= 0;
    }  
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************************
 * Function: Timer/Counter2 overflow interrupt
 * Purpose: Every 4ms, read encoder's and push buttons' values and display them
 *          on the LCD screen according to application.
 *          Timer has been set to 4ms for one reason: Time of pulse of the encoder
 *          is estimated 10ms. If a Timer is used which overflows in more than 10ms,
 *          we can lose changes of state of the encoder.
 *********************************************************************************/

ISR(TIMER2_OVF_vect)
{
    //Encoder definitions
    static uint8_t aLastState = 0;
    static uint8_t aState=0;
    static uint8_t bState=0;

    //Push button definitions
    uint8_t Push_Joystick;
    uint8_t Push_Encoder;


    //Read push button values
    Push_Joystick = GPIO_read(&PINB, JOYSTICK_PUSH);
    Push_Encoder = GPIO_read(&PINB, ENCODER_PUSH);

    //Actions to be done if buttons are pushed
    if (Push_Joystick == 0){
      lcd_clrscr();
      lcd_gotoxy(1,6);
      lcd_puts("BUZZER");
    }else if(Push_Encoder == 0){       
      lcd_clrscr();
      lcd_gotoxy(1,6);
      lcd_puts("REAR CAMERA");   
    }else{

      //Loop to know in which direction encoder is turning
      aState = GPIO_read(&PINB, ENCODER_OA);

      // If the previous and the current state of the outputA are different, that means a pulse has occured.
      if(aLastState != aState){
        lcd_clrscr();
        bState = GPIO_read(&PINB, ENCODER_OB);
      
        // Let's compare a and b states
        if(aState != bState){
          //If a and b states are different, this means encoder is rotating clockwise.
          lcd_gotoxy(1, 1);
          lcd_puts("Accelerate");
        }else{
          //Encoder is rotating counter clockwise.
          lcd_gotoxy(1, 1);
          lcd_puts("Deccelerate");   
        }
        aLastState = aState;
      }  
      
    }
}

/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display converted values on LCD screen.
 **********************************************************************/

ISR(ADC_vect)
{
    //Definitions of variable no_of_conversions. We want to process X and Y values
    //every 2 conversions
    static uint8_t no_of_conversions = 0; 
    
    //wW will define two parameters to save joystick X and Y axis positions
    static uint16_t Vrx=0;
    static uint16_t Vry=0;

    //We have 2 ADC pins connected, but AVR MCU can only make one conversion each ISR.
    //This means we cannot read both A0 and A1 values simultaneously. Therefore, we need to
    //use a condition to save both values.
    static uint8_t caso=0;

    if(caso == 0){
      caso=1;
      //Saving X axis value
      Vrx = ADC;
    }else{
      caso=0;
      //Saving Y axis value
      Vry = ADC;
    }

    //Every 2 conversions, we will get complete information about Joystick's position,
    //so we will be able to process it.
    no_of_conversions++;

    if(no_of_conversions >= 2){
      no_of_conversions = 0; //Clear number of conversions
    
      //Now that we have complete information about Joystick's position,
      //we need to understand and display it
      if( Vry > 512){
        //Joystick is heading upwards
        lcd_clrscr();
        lcd_gotoxy(1, 0);
        lcd_puts("Up");
      }else if ( Vry < 512){
        //Joystick is heading downwards
        lcd_clrscr();
        lcd_gotoxy(1, 0);
        lcd_puts("Down");  
      }
      if(Vrx > 512){
        //Joystick is heading left
        lcd_gotoxy(1, 8);
        lcd_puts("Left");
      }else if(Vrx < 512){
        //Joystick is heading right
        lcd_gotoxy(1, 8);
        lcd_puts("Right");
      }
    }
}