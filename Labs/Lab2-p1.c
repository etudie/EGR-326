/********************************************************************************************************************************
 *  Author:     	Corey Moura & Xue Hue
 *  Lab:        	2.1 Controlling the blink rate of an LED using push button switches.
 *  Date:       	9/4/19
 *  Instructor: 	Dr. Kandalaft
 *
 *  Description:	In this part, you will drive an external LED (your choice of color) interfaced to a port pin on the MSP432
 *              	(similar to lab #1). You will also use two external pushbuttons to interface with other port pins on the
 *              	MSP432.  These two pushbuttons will control the blink rate of the LED using interrupts.
 *
 *              	The first button will start the LED to blink at 2 sec intervals (on one second, off one second).  Each
 *              	press of the button will double the blink rate. The second button will stop the LED from blinking (if the
 *              	blink rate is .5 Hz), or reduce the blink rate by a factor of 2 if the blink rate is faster than .5 Hz.
 *              	Pressed buttons should be implemented using INTERRUPTS.
 *
 *              	Holding down a button will not change the blink rate more than pressing and releasing a button.
 *
 *  Notes:      	Using driverlib to initialize the GPIO and Systic interrupts.
 *
 *
 *******************************************************************************************************************************/
 
 
#include "driverlib.h"                  // Driverlib library
#include <stdint.h>                 	// A set of typedefs that specify exact-width integer types
#include <stdbool.h>                	// Allow boolean variable to be used
#include <stdio.h>                  	// Standard input output for printf etc.
#include <stdlib.h>                 	// Standard C-programming library
#include <string.h>                 	// Allows the use of strings
#include "msp.h"                    	// Another TI library of some sort
 
 
 
void initializeAll();                   // Called to initialize all of the MSP features used in the program.
void SysTick_Init_interupt();       	// Called from initialize all, initialization of the systic timer
void gpioHandler_Init();            	// Called from initialize all, initialization of the GPIO buttons as interrupts
void LED_init();                    	// Called from initialize all, used to initialize the LED pins
void button_init();                 	// Called from initialize all, used to initialize the button pins
 
void toggleControl();               	// Used to toggle the LED
void blue_On();	                     // Used to turn on the specified LED
void blue_Off();                    	// Used to turn off the specified LED
 
void speedControl();                    // Function used to increase the period or decrease
void decreaseSpeed(); 	              // Does math to increase the systic period load value
void increaseSpeed();               	// Does math to decrease the systic period load value
 
void PORT5_IRQHandler(void);        	// Interrupt handler used to set the button pressed flags
 
void buttonCheck();                 	// Uses the debounce to confirm a button is pressed
uint8_t DebounceSwitch_1();         	// Debounce switch 1
uint8_t DebounceSwitch_2();         	// Debounce switch 2
 
 
volatile uint8_t buttonFlag_1 = 0;                  	// Flag set in GPIO handler when the button is pressed
volatile uint8_t buttonFlag_2 = 0;                  	// Flag set in GPIO handler when the button is pressed
 
volatile uint8_t btn1Pressed = 0;           	        // Assigned 1 when the button has been confirmed to be pressed
volatile uint8_t btn2Pressed = 0;                   	// Assigned 1 when the button has been confirmed to be pressed
 
volatile uint8_t speedAssigned = 0;                 	// Prevents continual increment if the period and state
 
volatile uint32_t timeout = 0;                      	// Flag set from the syystic interrupt
volatile uint32_t sys_loadVal = 375000;             	// Initial value assigned to the systic timer
 
 
void main(void){
 
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;         	// stop watchdog timer
 
	initializeAll();                                    	// Initialize the pins
 
         	while(1){
 
         	    buttonCheck();                                  	// Check the buttons
         	    toggleControl();                                	// Toggle the LED
 
 
         	    if((btn1Pressed || btn2Pressed) && speedAssigned == 0){
 
         	    	speedControl();                             // Increment or decrement the period of the Systick Load value
         	    	speedAssigned = 1;                      	// The speed has been set for this button press
 
         	    }
 
         	    if(!(btn1Pressed) && !(btn2Pressed))    speedAssigned = 0; // Reset the speedAssigned if the button is released
         	}
}
 
 
/********************************************************************************************************************/
/* Check the debounce of the buttons.  Assigns the variables seen below and resets the interrupt flags.
/********************************************************************************************************************/
void buttonCheck(){
	if (buttonFlag_1 && DebounceSwitch_1()){
    	buttonFlag_1 = 0;              	     // Reset the interrupt flag
    	btn1Pressed = 1;                    	// Set the button variable to the pressed condition
	}
	else btn1Pressed = 0;
 
	if (buttonFlag_2 && DebounceSwitch_2()){
    	buttonFlag_2 = 0;               	    // Reset the interrupt flag
    	btn2Pressed = 1;                    	// Set the button variable to the pressed condition
	}
	else btn2Pressed = 0;                   	// If the above conditions are not met, then the button is not pressed
 
	return;
}
 
 
/********************************************************************************************************************/
/* DEBOUNCER 1 - Sample code provided by Dr. Kandalaft  */
/********************************************************************************************************************/
uint8_t DebounceSwitch_1(){
 
	static uint16_t check1 = 0;                             	// Variable reset every time function is called
 
	check1 = (check1<<1) | ((P5IN & BIT4)>>1) | 0xfc00;     	// Bitwise OR with shifts to complete in 1 cycle
 
	_delay_cycles(15000);                                   	// Delay the second check to confirm debounce
 
	if(check1 == 0xfc00){
    	return 1;          	                                 // If it is the correct value then button is pressed
	}
	return 0;                                          	     // If it is not the correct value then return 0
}
 
 
/********************************************************************************************************************/
/* DEBOUNCER 2 - Sample code provided by Dr. Kandalaft  */
/********************************************************************************************************************/
uint8_t DebounceSwitch_2(){
 
	static uint16_t check2 = 0;                             	// Variable reset every time function is called
 
	check2 = (check2<<1) | ((P5IN & BIT5)>>1) | 0xfc00;     	// Bitwise OR with shifts to complete in 1 cycle
 
    _delay_cycles(15000);                                   	// Delay the second check to confirm debounce
 
	if(check2 == 0xfc00){                               	
    	return 1;                                           	// If it is the correct value then button is pressed
	}
	return 0;                                                   // If it is not the correct value then return 0
}
 
 
/********************************************************************************************************************/
/* Controls the toggle of the LED based on the flag set by the systic interrupt.
/********************************************************************************************************************/
void toggleControl(){
	if(timeout){
    	P3->OUT ^= BIT6;                                    	// Toggle pin 4 every second
    	timeout = 0;                                        	// Reset systic timer flag
	}
}
 
 
/********************************************************************************************************************/
/* Depending on which button is pressed, the functions below that control speed will be called.
/********************************************************************************************************************/
void speedControl(){
 
	if(btn1Pressed) increaseSpeed();                            // Decrease the period, increase the blink rate
	if(btn2Pressed) decreaseSpeed();   	                     // Increase the period, decrease the blink rate
 
	return;
}
 
 
/********************************************************************************************************************/
/* Increases the speed of the LED blink rate by decreasing the period of the systic interrupt.
/********************************************************************************************************************/
void increaseSpeed(){
 
	if(sys_loadVal < 46872){
    	SysTick->CTRL = 0x00000005;                 	// Systic interrupt is disabled when cutoff value reached
    	blue_On();                                  	// Turn on the LED to prevent odd behavior
	}
 
	else{
    	SysTick->CTRL = 0x00000007;                             // Ensure the interrupt is enabled
    	sys_loadVal = sys_loadVal / 2;                      	// Half the period via division by 2	
	}
 
	SysTick->LOAD = sys_loadVal;                                // Assign the Load value of the timer
 
}
 
 
/********************************************************************************************************************/
/* Decreases the speed of the LED blink rate by increasing the period of the systic interrupt.
/********************************************************************************************************************/
void decreaseSpeed(){
 
	if(sys_loadVal > 2400000){          	
    	SysTick->CTRL = 0x00000005;                 	// Systic interrupt is disabled when cutoff value reached
    	blue_Off();                                     // If the freq gets to 0.5Htz, the LED is turned off
	}
 
	else{
    	SysTick->CTRL = 0x00000007;                             // Ensure the interrupt is enabled
    	sys_loadVal = sys_loadVal * 2;                      	// Double the period via multiplication by 2
	}
 
	SysTick->LOAD = sys_loadVal;                                // Assign the Load value of the timer
 
}
 
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
	uint32_t status;                    	                    // Declare variable of flag of the interrupt
 
	status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);  // Assign status a value if the interrupt was called
	MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, status);      	// After assignment above, clear the interrupt flag
 
   	if(status & GPIO_PIN4){
       	buttonFlag_1 = 1;                                	// If the first button was pressed, set the flag
   	}
   	if(status & GPIO_PIN5){
       	buttonFlag_2 = 1;                                	// If the second button was pressed, set the flag
   	}
}
 
 
/********************************************************************************************************************/
/* LED outputs from the MSP432.
/********************************************************************************************************************/
void blue_On(){ 	P3->OUT |= BIT6;	}               	// Turn on the LED
void blue_Off(){	P3->OUT &=~ BIT6;   }     	          // Turn off the LED	
void blue_Toggle(){ P3->OUT ^= BIT6;	}               	// Toggle the LED
 
 
/********************************************************************************************************************/
/* Call out to the functions and initialize the pins and timers used in the program.
/********************************************************************************************************************/
void initializeAll(){
 
	SysTick_Init_interupt();                	// Systic initialization
	gpioHandler_Init();                     	// GPIO interrupt initialization
	LED_init();                             	// LED initialization (outputs)
	button_init();                              // Button initializations (inputs)
 
}
 
/********************************************************************************************************************/
/* Systick timer initialization with interrupts.
/********************************************************************************************************************/
void SysTick_Init_interupt(){
	SysTick->CTRL = 0;                      	// Disable SysTick during step
	SysTick->LOAD = sys_loadVal;            	// Max reload value 0xFFFFFFFF but: for 1ms use 3000 - flag set every millisecond
	SysTick->VAL = 0;                       	// Any write to current clears it
	SysTick->CTRL = 0x00000007;             	// Enable SysTick, 3MHz, 0x00000007 interrupts & 0x00000005 no interrupts
}
 
 
/********************************************************************************************************************/
/* Port GPIO interrupt initialization via Driverlib
/********************************************************************************************************************/
void gpioHandler_Init(){
 
	MAP_WDT_A_holdTimer();                                                          //Halting the Watchdog
 
	MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN4 | GPIO_PIN5);  // 5.4 & 5.5 pull up inputs  	
	MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN4 | GPIO_PIN5);           	// Clear interrupt flag   
	MAP_GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN4 | GPIO_PIN5);              	// Enable the interrupt on the pins  
	MAP_Interrupt_enableInterrupt(INT_PORT5);                                   	// Additional interrupt enable
 
	MAP_SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);                      	// Enabling SRAM Bank Retention
	MAP_Interrupt_enableMaster();                                                   // Enabling MASTER interrupts
 
}
 
 
/********************************************************************************************************************/
/* LED initialization function (outputs).
/********************************************************************************************************************/
void LED_init(){
 
    	//Blue LED Pin
    	P3->SEL0 &=~ BIT6;                  	// Select the pin as a GPIO
    	P3->SEL1 &=~ BIT6;                  	// Select the pin as a GPIO
    	P3->DIR |= BIT6;         	           // Set pin as output
    	P3->OUT &=~ BIT6;                   	// Assign the initial condition as "off"
}
 
 
/********************************************************************************************************************/
/* Button initialization function (inputs).
/********************************************************************************************************************/
void button_init(){
 
    	// BTN 1 input
    	P5->SEL0 &=~ BIT4;                  	// Set to GPIO
    	P5->SEL1 &=~ BIT4;                  	// Clear pin
    	P5->DIR &=~ BIT4;                   	// set as input
    	P5->REN |= BIT4;        	            // Set to 1 enables internal resistor
    	P5->OUT |= BIT4;                    	// 1 Assigned as a Pull-Up resistor (the pull-Up resistor)
 
    	// BTN 2 input
    	P5->SEL0 &=~ BIT5;    	              // set to GPIO
    	P5->SEL1 &=~ BIT5;                  	// Clear pin
    	P5->DIR &=~ BIT5;                   	// set as input
    	P5->REN |= BIT5;                    	// Set to 1 enables internal resistor (the pull-Up resistor)
    	P5->OUT |= BIT5;     	               // 1 Assigned as a Pull-Up resistor
}
 
 
/********************************************************************************************************************/
/*                            ---------------------END OF PROGRAM---------------------
/********************************************************************************************************************/
 
 



