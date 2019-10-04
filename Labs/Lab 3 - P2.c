/********************************************************************************************************************************
 *  Author:     	Corey Moura & Xue Hue
 *  Lab:        	3.2  - "Creating alert for proximity"
 *  Date:       	9/4/19
 *  Instructor: 	Dr. Kandalaft
 *
 *  Description:	In this part, you will use the results from part one to indicate position by varying the intensity of an LED.
 *
 *              	1.  Connect an external LED to one of the TIMER_A output pins.
 *              	2.  Connect and run the proximity sensor as in part one of this lab
 *              	3.  Vary the intensity of the LED so that 1 inch distance will result in maximum intensity, while any distance
 *                   	greater than 10 inches will turn the LED off.
 *                              	1 inch or less  = max intensity (max duty cycle) (blinking)
 *                              	2 inches	= 9/10 max intensity (9/10 duty cycle)
 *                              	3 inches	= 8/10 max intensity (8/10 duty cycle)
 *                              	4 inches	= 7/10 max intensity (7/10 duty cycle)
 *                              	5 inches	= 6/10 max intensity (6/10 duty cycle)
 *                              	...
 *                              	9 inches	= 2/10 max intensity (2/10 duty cycle)
 *                              	10 inches   = off
 *
 *              	4.  If an object is within 1 inch of the sensor, the LED should blink at 2 Hz.
 *
 *
 *  Notes:          No Driver Lib, Custom Lib used
 *
 *
 *******************************************************************************************************************************/
#include <stdint.h>                             	// A set of typedefs that specify exact-width integer types
#include <stdbool.h>                            	// Allow boolean variable to be used
#include <stdio.h>                              	// Standard input output for printf etc.
#include <stdlib.h>                             	// Standard C-programming library
#include <string.h>                             	// Allows the use of strings
#include "msp.h"                                	// Another TI library of some sort
#include <math.h>                               	// MathLib used for the calculation of distance
#include <EGR326Lib.h>                          	// Custom made Library linked to the project
 
void inititialize();                            	// Called to initialize all of the MSP features used in the program.
void timerA_Init();                             	// Initializes two instances of timer A
void checkTimeout();                            	// Check to see if the Systick Flag has been set
void checkNumCaptured();                        	// Check to see if both captured numbers have been stored
void triggerPulse();                            	// Pulses the output trigger pin for 10uS
void calcDistance();                            	// Calculates the distance to the object using the time interval of captures
void assignPWM();                               	// Used to modulate the intensity of the LED based on the distance value
void toggleControl();                           	// Toggles an LED based on the current state
 
volatile float numClkCycles = 0;                	// Number of clock cycles in the captured interval
volatile float totalTime = 0;                   	// Total time elapsed in the interval
volatile float distanceInMeters = 0;            	// Distance in meters
volatile float speedOfSound = 340;              	// Speed of sound constant ~340 m/s
volatile float distanceInCm = 0;                	// Converted distance to cm
volatile uint16_t pulseLength = 0;              	// Number of cycles the clock took
 
volatile uint8_t  numCapture = 0;               	// Incremented to track the incoming interrupt values for the CCR reg.
volatile uint16_t captureVal = 0;               	// Assigned the value from the CCR register every interrupt
volatile uint16_t capturedVal_1 = 0;            	// First captured value from the CCR reg is stored here
volatile uint16_t capturedVal_2 = 0;            	// second captured value from the CCR reg is stored here
volatile uint32_t timeout = 0;                  	// Flag set from the syystic interrupt
volatile uint32_t sys_loadVal = 1500000;        	// 1/2Hz = 6600000: Initial value assigned to the systic timer
 
volatile uint32_t CCR1Value = 0;                	// Used to reassign the value of the second timerA
volatile uint8_t  toggleFlag = 0;               	// If the object is too close it will set the flag
 
 
 void main(void){
 
	inititialize();                                         // Initialize all the components of the program
 
	while(1){
 
    	checkTimeout();                                 	      // Check to see if the Systick Flag has been set
    	checkNumCaptured();                                     // Check to see if both captured numbers have been stored
 
	}
}
 
/********************************************************************************************************************/
/*  DETERMINING IF THE INTERVAL HAS BEEN COMPLETED:  If the timer interupt has been called twice, then it can be
 *  implied that the interval is ready to be calculated, and the PWM value to be assigned from the new distance val.*/
/********************************************************************************************************************/
void checkNumCaptured(){
	if(numCapture == 2){                                    	// Enter if the interval values have been captured
    	numCapture = 0;                                     	// Reset the counter
 
    	calcDistance();                                     	// Calculate the distance to the object
    	assignPWM();                      	                    // Assign the brightness of the LED
 
    	printf("distanceInCm = %f\n\n", distanceInCm);      	// Display the distance to the object in the console
 
	}
}
 
/********************************************************************************************************************/
/*  TRIGGER CONTROL / TOGGLE CONTROL / SYSTIC FLAG CONTROL  	*/
/********************************************************************************************************************/
void checkTimeout(){
	if(timeout){
    	timeout = 0;                                       // Reset the Systick timer flag
    	triggerPulse();                                    // Sends the pulse to the trigger every second, based on timer
 
    	if(toggleFlag){
        	P5->SEL0 &=~ BIT6;                      	 // Need to reset the definition of the output pin to toggle
        	P5->SEL1 &=~ BIT6;                      	 // Set to GPIO
  	      P5->OUT  ^=  BIT6;                      	 // Toggle the LED
    	}
    	else{
        	P5->SEL0 |=   BIT6;                     	 // Need to set back to alternate pin function for PWM
        	P5->SEL1 &=~  BIT6;                   	           // Timer A0.2 re-assigned
    	}
	}
}
 
 
/********************************************************************************************************************/
/*  PWM CONTROL:  The led intensity is controlled by the value of the distance var.  The flag is set when the LED
 *  needs to be toggled and reset when the distqnce is greater than 1 inch ~ 2.5cm      */
/********************************************************************************************************************/
void assignPWM(){
 
	if(distanceInCm <= 2.5)     	{   TIMER_A2->CCR[1] = 60000;  	toggleFlag = 1;	}  // less than 1 inch
	else if(distanceInCm <= 5)  	{   TIMER_A2->CCR[1] = 54000;  	toggleFlag = 0;	}  // Less than 2 inches
	else if(distanceInCm <= 7.5)	{   TIMER_A2->CCR[1] = 48000;  	toggleFlag = 0;	}  // Less than 3 inches
	else if(distanceInCm <= 10) 	{   TIMER_A2->CCR[1] = 42000;  	toggleFlag = 0;	}  // Less than 4 inches
	else if(distanceInCm <= 12.5) {   TIMER_A2->CCR[1] = 36000;  	toggleFlag = 0;	}  // Less than 5 inches
	else if(distanceInCm <= 15) 	{   TIMER_A2->CCR[1] = 30000;  	toggleFlag = 0;	}  // Less than 6 inches
	else if(distanceInCm <= 17.5) {   TIMER_A2->CCR[1] = 24000;  	toggleFlag = 0;	}  // Less than 7 inches
	else if(distanceInCm <= 20) 	{   TIMER_A2->CCR[1] = 18000;  	toggleFlag = 0;	}  // Less than 8 inches
	else if(distanceInCm <= 22.5) {   TIMER_A2->CCR[1] = 12000;  	toggleFlag = 0;	}  // Less than 9 inches
	else if(distanceInCm <= 25) 	{   TIMER_A2->CCR[1] = 6000;   	toggleFlag = 0;	}  // Less than 10 inches
	else                        	{   TIMER_A2->CCR[1] = 0;      	toggleFlag = 0;	}  // Greater than 10
 
}
 
 
/********************************************************************************************************************/
/*  CALCULATING THE DISTANCE:  Calculates the distance from the timer interrupt values.  The number of clock cycles
 *  between reads is calculated into a time value by finding the period of a clock cycle.  The time is multiplied by
 *  the speed of sound and divided by two to compensate for the travel time to and from the object   */
/********************************************************************************************************************/
void calcDistance(){
	numClkCycles = abs(capturedVal_1 - capturedVal_2);                 // The absolute value of the two captures
	totalTime = 3*pow(10,-7) * numClkCycles;             	       // period of CLK cycle * number of CLK cycles
	distanceInMeters = (speedOfSound * totalTime) / 2;                 // Speed of sound * total time / 2
	distanceInCm = distanceInMeters * 100;                             // Conversion from m->cm for displaying
}
 
 
/********************************************************************************************************************/
/*  TIMER A0.2 ISR: This is called whenever the timer object detects an input signal changes.  	*/
/********************************************************************************************************************/
void TA0_N_IRQHandler(void){
	TIMER_A0->CCTL[2] &=~ BIT0;                                 	// Clear the interrupt flag
 
    captureVal = TIMER_A0->CCR[2];                                              // Read the value of the captured TAxR number
 
	numCapture++;                                                         // Track the beginning and end of interval
 
	if(numCapture == 1) capturedVal_1 = captureVal;             	// Beginning of interval CCR val is assigned here
	if(numCapture == 2) capturedVal_2 = captureVal;             	// End of interval CCR val is assigned here
}
 
/********************************************************************************************************************/
/*  TRIGGER PULSE: It is required to toggle the trigger pin of the Ultrasonic to initiate a reading   */
/********************************************************************************************************************/
void triggerPulse(){
	P5->OUT  |=  BIT5;                                          	// Toggle bit ON
	_delay_cycles(33);                                          	// Delay set for 10uS as per data sheet of sensor
	P5->OUT  &=~  BIT5;                                         	// Toggle bit OFF
}
 
 
/********************************************************************************************************************/
/* SYSTICK INTERRUPT HANDLER:  This is called when the systick timer reaches zero from its load value, sets flag  */
/********************************************************************************************************************/
extern void SysTick_Handler(void){
	timeout = 1 ;                                                   // set flag for timeout of SysTick, rest in main
}
 
 
/********************************************************************************************************************/
/*  INITIALIZATIONS: Initialize the pins and timers used in the program.  */
/********************************************************************************************************************/
void inititialize(){
 
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;                       // stop watchdog timer
 
	timerA_Init();
	Init_SysTick(sys_loadVal, 7);                            	      // Systick init located in the custom library
 
	Init_OutputPin(55);                                         	// GPIO ouput pin located in the custom library
	Init_InputPin(25, 1, 0, 0, 0, 0, 0, 0);                     	// Pin2.4, TA, input
 
 
	P5->DIR  |=  BIT6;                                          	// P2.4 set TA0.1 P2->SEL0 |= BIT4;
	P5->SEL0 |=  BIT6;                                          	// Set to zero
	P5->SEL1 &=~ BIT6;                    	                              // Enable timer (set to 1)
	P5->OUT  |=  BIT6;                                          	//turns on pin
 
	__enable_irq();                                                       // enable global interrupts
	NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);                   	// Enable TA0_N_IRQn interrupt in NVIC vector
}
 
/********************************************************************************************************************/
/*  TIMER A INITIALIZATIONS:
 *    	Timer AO.2 used as the input interrupt timer
 *    	Timer A2.1 used as the ouptut timer for LED PWM                    	*/
/********************************************************************************************************************/
void timerA_Init(){
 
	TIMER_A0->CTL  =  0x0220;                   	// SMCLK, interrupts disabled, continuous mode, no divider
	TIMER_A0->CCTL[2] = 0xCD78;                 	// Intrpt en, set/rst, capture, sync, CCIxA signal, rising edge
 
	TIMER_A2->CTL  =  0x0214;                           	// Set the configuration of the clock
	TIMER_A2->CCR[0] = 60000;                           	// PWM Period (# cycles of the clock)
	TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;         	// Output mode, count up
	TIMER_A2->CCR[1] = 0;                               	// CCR1 PWM duty cycle in 10ths of percent
 
}
