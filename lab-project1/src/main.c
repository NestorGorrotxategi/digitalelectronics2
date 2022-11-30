/***********************************************************************
 * Teammates: 
 *    NESTOR GORROTXATEGI AGIRRE
 *    DRAGOS ...
 *
 * Project 1
 * 
 * Read ADC signals from Joystick, read signal from rotary encoder and both
 * push buttons and display on LCD screen results.
 * 
 * Application: Control of direction and acceleration of a system. Joystick
 *              is used to control direction. When Joystick's push button is
 *              pressed, a buzzer activates.
 *              The rotary encoder is used to control acceleration and braking.
 *              When encoder's push button is pressed, the rear camera activates.
 **********************************************************************/

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
    //We set prescaler to 33ms and enable overflow interrupt
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
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    static uint8_t no_of_overflows = 0;
    static uint8_t caso = 0;

    no_of_overflows++;
    if (no_of_overflows >= 3)
    {
      //Select channel
      if(caso==0){
      caso=1;
      ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
      }else{
      caso=0;
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
 * Purpose: Every 200ms, read encoder's and push buttons' values and display them
 *          on the LCD screen according to application.
 *          Timer has been set to 200ms for one reason: ADC conversion is started
 *          every 100ms, and gets both X and Y axis values of Joystick every 200ms. 
 *          So, we are interested to read values from encoder and push buttons
 *          only when we have both X and Y values, that is, every 200ms.
 *********************************************************************************/
ISR(TIMER2_OVF_vect)
{

    //Encoder definitions
    static uint8_t aLastState = 0;
    static uint8_t aState=0;
    static uint8_t bState=0;
    char string1[4];
    char string2[4];
    //Push button definitions
    uint8_t Push_Joystick;
    uint8_t Push_Encoder;

    //Timer2 OVFs every 16ms, so we need approximately 12 OVFs
    //to reach 200ms.

      //Read push button values
      Push_Joystick = GPIO_read(&PINB, JOYSTICK_PUSH);
      Push_Encoder = GPIO_read(&PINB, ENCODER_PUSH);
      /*
      itoa(Push_Joystick,string1, 10);
      itoa(Push_Encoder, string2, 10);


      lcd_clrscr();
      lcd_gotoxy(8,0);
      lcd_puts(string1);      
      lcd_gotoxy(8,1);
      lcd_puts(string2);
      */

      if (Push_Joystick == 0){
            //Actions to be done if buttons are pushed
             lcd_clrscr();
             lcd_gotoxy(1,6);
             lcd_puts("BUZZER");
      }else if(Push_Encoder == 0){       
             lcd_clrscr();
             lcd_gotoxy(1,6);
             lcd_puts("REAR CAMERA");   
      }else{

      //Loop to know in which direction is encoder turning
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
 * Purpose:  Display converted value on LCD screen.
 **********************************************************************/
ISR(ADC_vect)
{
    static uint8_t no_of_conversions = 0;
    char string1[4];
    char string2[4];    
    
    //we will define two parameters to save joystick X and Y axis positions
    static uint16_t Vrx=0;
    static uint16_t Vry=0;

    //We have 2 ADC pins connected, but AVR MCU can only make one conversion each ISR.
    //This means we cannot read both A0 and A1 values simultaneously. Therefore, we need to
    //use a condition to read both values.
    static uint8_t caso=0;

    if(caso == 0){
      caso=1;
      Vrx = ADC;
    }else{
      caso=0;
      Vry = ADC;
    }

    //Every 2 conversions, we will get complete information about Joystick's position,
    //so we will be able to process it.
    no_of_conversions++;

    if(no_of_conversions >= 2){
      no_of_conversions = 0; //Clear number of conversions
      itoa(Vrx,string1, 10);
      itoa(Vry, string2, 10);
      lcd_clrscr();
      lcd_gotoxy(1,0);
      lcd_puts("Vrx");      
      lcd_gotoxy(5,0);
      lcd_puts(string1);
      lcd_gotoxy(1,1);
      lcd_puts("Vry");     
      lcd_gotoxy(5,1);      
      lcd_puts(string2);
    
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