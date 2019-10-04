
/********************************************************************************************************************************
 *  Author:     	Corey Moura
 *  Lab:        	2.2  -  "Sequencing colors of a RGB LED using a timer interrupt and pushbutton"
 *  Date:       	9/4/19
 *  Instructor: 	Dr. Kandalaft
 *
 *  Description:	Use the three on-board RGB LEDs for this exercise (with one external pushbutton)The pushbutton should be
 *              	triggered by interrupts. On RESET, the LEDs should be OFF. When you press and hold down the pushbutton the
 *              	red LED (and only the red LED) lights for one second followed by only the green LED ON for one second and then
 *              	the blue LED is ON for one second.   The timing should be controlled by the systick timer interrupt set at 1
 *              	second intervals.This sequence should repeat as long as the pushbutton is depressed. When the pushbutton is
 *              	released the sequencing pauses with the current LED remaining ON. Upon the next press of the pushbutton, the
 *              	LED sequence will start up again in reverse order and continue for as long as the pushbutton is held down.
 *
 *  Notes:      	DriverLib used to initialize the interrupts
 *
 *
 *******************************************************************************************************************************/
#include "driverlib.h"                          // Driverlib library
#include <stdint.h>                             // A set of typedefs that specify exact-width integer types
#include <stdbool.h>                            // Allow boolean variable to be used
#include <stdio.h>                              // Standard input output for printf etc.
#include <stdlib.h>                             // Standard C-programming library
#include <string.h>                             // Allows the use of strings
#include "msp.h"                                // Another TI library of some sort
 
 
void initializeAll();                       	// Called to initialize all of the MSP features used in the program.
void SysTick_Init_interupt();               	// Called from initialize all, initialization of the systic timer
void gpioHandler_Init();                    	// Called from initialize all, initialization of the GPIO buttons as interrupts
void LED_init();                            	// Called from initialize all, used to initialize the LED pins
void button_init();          	               // Called from initialize all, used to initialize the button pins
 
 
void red_On();                              	// Used to turn on the specified LED
void red_Off();                             	// Used to turn off the specified LED
 
void green_On();                            	// Used to turn on the specified LED
void green_Off();                           	// Used to turn off the specified LED
 
void blue_On();                        	          // Used to turn on the specified LED
void blue_Off();                            	// Used to turn off the specified LED
 
void state_1();                             	// Used in the state machine to complete processes of this state
void state_2();                             	// Used in the state machine to complete processes of this state
void state_3();                             	// Used in the state machine to complete processes of this state
void state_4();                             	// Used in the state machine to complete processes of this state
 
void stateControl();                        	// Handles state assignments
 
void buttonCheck();   	                      // Uses the debounce to confirm a button is pressed
uint8_t DebounceSwitch_1();                 	// Debounce switch 1
 
void PORT5_IRQHandler(void);
 
 
volatile uint8_t buttonFlag_1 = 0;                  	// Flag set in GPIO handler when the button is pressed
volatile uint8_t btn1Pressed = 0;                   	// Assigned 1 when the button has been confirmed to be pressed
 
volatile uint32_t timeout = 0;                      	// Flag set from the syystic interrupt
volatile uint32_t sys_loadVal = 6000000;            	// Initial value assigned to the systic timer
 
volatile uint8_t stateAssigned = 0;                 	// Prevents the continuous incrementation of the state
volatile uint32_t state = 1;               	         // Assigned the current state value
volatile uint32_t numPressed = 0;                   	// Tracks the number of presses of the user
 
void main(void){
 
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;         	// stop watchdog timer
 
	initializeAll();                                    	// Initialize the pins
 
         	while(1){
 
         	    if(buttonFlag_1) buttonCheck();                 	// If the interrupt is triggered then check button
 
         	    if(btn1Pressed && timeout){                	     // Waits here until the systic flag is set
 
         	    	stateControl();                                 // Assigns the state before entering state-machine
 
         	    	switch (state){
         	             	case(1):
         	                    	state_1();    	              // All off
         	                     	break;
         	             	case(2):
         	                    	state_2();                  	// Red on
         	                    	break;
         	             	case(3):
         	                    	state_3();      	            // Green on
         	                    	break;
         	             	case(4):
         	                    	state_4();                  	// Blue on
         	                    	break;
         	             	default: state_1();                	// All off
         	             }
         	   	timeout = 0;
         	    }
 
         	    if(!buttonFlag_1 && !(DebounceSwitch_1())) btn1Pressed = 0; 	// Resets the btnpressed variable if true
         	}
}
 
 
 
/********************************************************************************************************************/
/* Check the debounce of the buttons.  Assigns the variables seen below and resets the interrupt flags.
/********************************************************************************************************************/
void buttonCheck(){
 
	if(DebounceSwitch_1()){
    	buttonFlag_1 = 0;                               // Reset the interrupt flag
    	btn1Pressed = 1;        	                    // Set the button variable to the pressed condition
    	numPressed++;
	}
	else btn1Pressed = 0;                           	// If conditions are not met, then the button is not pressed
 
	if(numPressed > 2) numPressed = 1;              	// Monitor the number of button presses
 
	return;
 
}
 
/********************************************************************************************************************/
/* DEBOUNCERS - Sample code provided by Dr. Kandalaft
/********************************************************************************************************************/
uint8_t DebounceSwitch_1(){
 
	static uint16_t check1 = 0;                             	// Variable reset every time function is called
 
	check1 = (check1<<1) | ((P5IN & BIT4)>>1) | 0xfc00;     	// Bitwise OR with shifts to complete in 1 cycle
 
	_delay_cycles(15000);                                   	// Delay the second check to confirm debounce
 
	if(check1 == 0xfc00){
    	return 1;                                           	// If it is the correct value then button is pressed
	}
	return 0;                                                   // If it is not the correct value then return 0
 
}
 
 
/********************************************************************************************************************/
/* Depending on the number of times the button is pressed will determine the state.
/********************************************************************************************************************/
void stateControl(){
 
	if(numPressed == 1){
    	state++;                                                // Increment the state
    	if(state > 4)  state = 2;                               // Cycle the state
	}
 
	if(numPressed == 2){
    	state--;                                                // Decrement the state
    	if(state < 2) state = 4;               	             // Cycle the state
	}
 
	return;
 
}
 
 
/********************************************************************************************************************/
/* Functions of the different states of the state machine
/********************************************************************************************************************/
void state_1(){  red_Off(); green_Off();   blue_Off();  }       	// All LEDs off
void state_2(){  red_On();  green_Off();   blue_Off();  }       	// Red LED on
void state_3(){  red_Off(); green_On();	blue_Off();  }       	// Green LED on
void state_4(){  red_Off(); green_Off();   blue_On();   }       	// Blue LED on
 
 
/********************************************************************************************************************/
/* Systic Interrupt Handler
/********************************************************************************************************************/
void Systick_Handler(void){
	timeout = 1 ;                                           	// set flag for timeout of SysTick, rest in main
	//intervalCnt++ ;                                           // increment interval timer by 1ms, reset in main ( )
 
}
 
 
/********************************************************************************************************************/
/* GPIO ISR handler for the buttons.
/********************************************************************************************************************/
void PORT5_IRQHandler(void){
	uint32_t status;                                        	// Declare variable of flag of the interrupt
 
	status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);  // Assign status a value if the interrupt was called
	MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, status);      	// After assignment above, clear the interrupt flag
 
   	if(status & GPIO_PIN4){
       	buttonFlag_1 = 1;                                	// If the first button was pressed, set the flag
   	}
 
}
 
 
/********************************************************************************************************************/
/* LED outputs from the MSP432.
/********************************************************************************************************************/
void red_On()  {	P2->OUT |= BIT0;	}                   	// Turn on the LED
void red_Off()  {   P2->OUT &=~ BIT0;   }                   	// Turn off the LED
 
void green_On(){	P2->OUT |= BIT1;	}                   	// Turn on the LED
void green_Off(){   P2->OUT &=~ BIT1;   }                   	// Turn off the LED
 
void blue_On() {	P2->OUT |= BIT2;	}                   	// Turn on the LED
void blue_Off() {   P2->OUT &=~ BIT2;   }                   	// Turn off the LED
 
 
/********************************************************************************************************************/
/* Call out to the functions and initialize the pins and timers used in the program.
/********************************************************************************************************************/
void initializeAll(){
 
	SysTick_Init_interupt();                                	// Systic initialization
	gpioHandler_Init();                                     	// GPIO interrupt initialization
	LED_init();                                                 // LED initialization (outputs)
	button_init();                                          	// Button initializations (inputs)
 
}
 
 
/********************************************************************************************************************/
/* Systick timer initialization with interrupts.
/********************************************************************************************************************/
void SysTick_Init_interupt(){
	SysTick->CTRL = 0;          	// Disable SysTick during step
	SysTick->LOAD = sys_loadVal;	// Max reload value 0xFFFFFFFF but: for 1ms use 3000 - flag set every millisecond
	SysTick->VAL = 0;           	// Any write to current clears it
	SysTick->CTRL = 0x00000007; 	// Enable SysTick, 0x00000007 interrupts & 0x00000005 no interrupts
 
}
 
 
/********************************************************************************************************************/
/* Port GPIO interrupt initialization via Driverlib
/********************************************************************************************************************/
void gpioHandler_Init(){
 
	MAP_WDT_A_holdTimer();                                                  //Halting the Watchdog
 
	MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN4);  	// 5.4 pull up inputs
	MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN4);               	// Clear interrupt flag
	MAP_GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN4);                  	// Enable the interrupt on the pins
	MAP_Interrupt_enableInterrupt(INT_PORT5);                               // Additional interrupt enable
 
	MAP_SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);              	// Enabling SRAM Bank Retention
	MAP_Interrupt_enableMaster();                                       	// Enabling MASTER interrupts
 
}
 
 
/********************************************************************************************************************/
/* LED initialization function (outputs).
/********************************************************************************************************************/
void LED_init(){
 
	//Red LED Pin
	P2->SEL0 &=~ BIT0;                                  // Select the pin as a GPIO
	P2->SEL1 &=~ BIT0;                                  // Select the pin as a GPIO
	P2->DIR |= BIT0;                  	              // Set pin as output
	P2->OUT &=~ BIT0;                               	// Assign the initial condition as "off"
 
	//Green LED Pin
	P2->SEL0 &=~ BIT1;                                  // Select the pin as a GPIO
	P2->SEL1 &=~ BIT1;                                  // Select the pin as a GPIO
	P2->DIR |= BIT1;                                	//set as output
	P2->OUT &=~ BIT1;                               	// Assign the initial condition as "off"
 
	//Blue LED Pin
	P2->SEL0 &=~ BIT2;                                  // Select the pin as a GPIO
	P2->SEL1 &=~ BIT2;                                  // Select the pin as a GPIO
	P2->DIR |= BIT2;  	                              //set as output
	P2->OUT &=~ BIT2;                               	// Assign the initial condition as "off"
 
}
 
 
/********************************************************************************************************************/
/* Button initialization function (inputs).
/********************************************************************************************************************/
void button_init(){
 
	// BTN 1 input
	P5->SEL0 &=~ BIT4;                                  // Set to GPIO
	P5->SEL1 &=~ BIT4;                                  // Clear pin
	P5->DIR &=~ BIT4;                               	// set as input
	P5->REN |= BIT4;                                	// Set to 1 enables internal resistor
	P5->OUT |= BIT4;                      	          // 1 Assigned as a Pull-Up resistor (the pull-Up resistor)
 
}
 
 
 
 
/********************************************************************************************************************/                     
---------------------END OF PROGRAM---------------------
/********************************************************************************************************************/
