/********************************************************************************************************************************
 *  Author:     Corey Moura and Xue Hua
 *  Lab:        1 input interfacing with the MSP432
 *  Date:       9/3/19
 *  Instructor: Dr. Kandalaft
 *
 *  Description:    Program uses a state machine to increment the color of an RGB LED through a sequence of colors   
 *                         based on the inputs of the button to the MSP432.  When the user presses button 1 the state 
 *                         increments front to back in
 *                         the sequence RGB.  If button 2 is pressed then the state decrements back to front in the sequence 
 *                        BGR. If both are pressed at the same time, the LED will turn off.
 *
 *  Notes:          The instance of both buttons simultaneously pressed is difficult to handle.
 *******************************************************************************************************************************/

/* Libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>          //Used for debugging
#include <stdlib.h>     //Used for debugging
#include <string.h>     //Used for debugging
#include "msp.h"
 
 

void initializeAll();			/* Initialize the pins - includes sets their initial conditions */
 
void red_On();			/* Used from the state functions below to easily track what is on and off */
void red_Off();
void green_On();
void green_Off();
void blue_On();
void blue_Off();
 
void state_1();			/* States of the program */
void state_2();
void state_3();
void state_4();

void stateControl();		/* Handles state assignments */
 
void buttonCheck();		/* Handles the logic of checking the buttons */
 
void PORT5_IRQHandler(void);
 
uint8_t DebounceSwitch_1();	/* Handle button debounce */
uint8_t DebounceSwitch_2();
 
/* Global variables */
volatile uint32_t state = 1;
volatile uint8_t btn1Pressed = 0;
volatile uint8_t btn2Pressed = 0;
volatile uint8_t stateAssigned = 0;
volatile uint8_t both = 0;
 
 
void main(void){
 
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
 
    initializeAll();                    		// Initialize the pins
 
	while(1){
 
	    /* If a button is pressed and the state has not already been assigned in that button press, then enter. */
	    if((btn1Pressed || btn2Pressed) && stateAssigned == 0){
 
	        stateControl();                 	// Assign the state
 
	            /* State Machine */
	            switch (state){
                     case(1):
                             state_1();                 	// All off
                             break;
                     case(2):
                             state_2();                 	// Red on
                             break;
                     case(3):
                             state_3();                	 // Green on
                             break;
                     case(4):
                             state_4();                	 // Blue on
                             break;
                     default: state_1();                	// All off
                }
                stateAssigned = 1;                      	// The state has been set for this button press
	    }
 
	    /* If the user has one, or both of the buttons pressed this will not allow the program to enter the state machine. It prevents the program from auto incrementing through the states by setting a flag. */
	    if(!(btn1Pressed) && !(btn2Pressed)) { stateAssigned = 0;}
	}
}
 
/* Check the state of the buttons.  If regardless where the program is when a button is pressed, it will also check the other.
 * This logic handles the case if the program has passed the first button assignment and has moved onto the next, even though the user is pressing both buttons.  Debounce is handled here also. */
void buttonCheck(){
    btn1Pressed = DebounceSwitch_1();
    btn2Pressed = DebounceSwitch_2();
 
    if(btn1Pressed) btn2Pressed = DebounceSwitch_2();
    if(btn2Pressed) btn1Pressed = DebounceSwitch_1();
 
    return;
}
 
/* Depending on which button is pressed, the state will increment or decrement.  In effect determining which LED will be lit. */
void stateControl(){
 
    if(btn1Pressed && btn2Pressed){
        state = 1;
        both = 0;
        //printf("Both state = %d\n", state);
    }
 
    else if(btn1Pressed){
        state++;
        if(state > 4)  state = 2;
        //printf("btn1 pressed state = %d\n", state);
    }
 
    else if(btn2Pressed){
        state--;
        if(state < 2)  state = 4;
        //printf("btn2 pressed state = %d\n", state);
    }
 
    return;
}
 
/* Sample code provided by Dr. Kandalaft.  This function completes bitwise OR logic and a delay to handle the debounce */
uint8_t DebounceSwitch_1(){
    static uint16_t check1 = 0;
    check1 = (check1<<1) | ((P5IN & BIT4)>>1) | 0xfc00;
    _delay_cycles(150000);
    if(check1 == 0xfc00){
        return 1;
    }
    return 0;
}
 
/* Sample code provided by Dr. Kandalaft.  This function completes bitwise OR logic and a delay to handle the debounce */
uint8_t DebounceSwitch_2(){
    static uint16_t check2 = 0;
    check2 = (check2<<1) | ((P5IN & BIT5)>>1) | 0xfc00;
    _delay_cycles(150000);
    if(check2 == 0xfc00){
        return 1;
    }
    return 0;
}

/********************************************************************************************************************/
/* LED outputs from the MSP432 */
void green_On(){    P3->OUT |= BIT5;    }
void blue_On() {    P3->OUT |= BIT6;    }
void red_On()  {    P5->OUT |= BIT2;    }
 
void green_Off(){   P3->OUT &=~ BIT5;   }
void blue_Off() {   P3->OUT &=~ BIT6;   }
void red_Off()  {   P5->OUT &=~ BIT2;   }
 
/********************************************************************************************************************/
/* States use the above functions to create easily readable logic */
void state_1(){  red_Off(); green_Off();   blue_Off();  }
void state_2(){  red_On();  green_Off();   blue_Off();  }
void state_3(){  red_Off(); green_On();    blue_Off();  }
void state_4(){  red_Off(); green_Off();   blue_On();   }
 
 
/********************************************************************************************************************/
/* Initialize all of the pins used in the program. */
void initializeAll(){
 
    //Green LED Pin
    P3SEL0 &=~ BIT5;        		 //set to GPIO
    P3SEL1 &=~ BIT5;
    P3->DIR |= BIT5;        		//set as output
    P3->OUT &=~ BIT5;
 
    //Blue LED Pin
    P3SEL0 &=~ BIT6;        		 //set to GPIO
    P3SEL1 &=~ BIT6;
    P3->DIR |= BIT6;        		//set as output
    P3->OUT &=~ BIT6;
 
    //Red LED Pin
    P5SEL0 &=~ BIT2;        		 //set to GPIO
    P5SEL1 &=~ BIT2;
    P5->DIR |= BIT2;       		 //set as output
    P5->OUT &=~ BIT2;
 
    // BTN 1 input
    P5SEL0 &=~ BIT4;         	//set to GPIO
    P5SEL1 &=~ BIT4;        		 //Clear P3.2
    P5->DIR &=~ BIT4;       		 //set as input
    P5REN |= BIT4; 		//Set to 1 enables internal resistor
    P5OUT |= BIT4; 		//Assigned as a Pull-Up resistor
 
    // BTN 2 input
    P5SEL0 &=~ BIT5;        		 //set to GPIO
    P5SEL1 &=~ BIT5;         	//Clear P3.2
    P5->DIR &=~ BIT5;        		//set as input
    P5REN |= BIT5; 		//Set to 1 enables internal resistor
    P5OUT |= BIT5; 		//Assigned as a Pull-Up resistor
}
}
