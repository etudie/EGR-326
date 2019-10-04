Objective 
The objective of this lab was to use two pushbuttons to control sequence of an RGB LED with the MSP 432. To develop a program that uses SysTick timer to generate precise time intervals for controlling duration of LEDs illumination. Furthermore, to accomodate switch bounce into the program. 
Equipment
MSP432 P401R Microcontroller
RGB LED
two momentary push buttons
two 100 ohm resistor 
220 ohm resistor
Results
Part 1:  Sequencing RGB using a single momentary push button
A state machine was developed to map out the button as the input and the three different colors as the states, to include the “off” state.  The state machine can be seen in the figure below.



Figure 1. State Transition Diagram for Part 1


The program skeleton was created in order to accommodate for the logic of the program.  The skeleton included all of the necessary programming items needed to communicate with the MSP432, things like: libraries, main(), pin declarations, while(1), etc.

Three pins were assigned as outputs, and one as an input.  The input pin was assigned with a pull up resistor though the SEL bits.  This means the pin will see logic 3.3V until the button is pressed and the pin is grounded.  This is the safest way to prevent damage to the board.  

The circuit was created next using the 2N7000 MOSFETs.  A single white LED was wired first to test out the circuit and the initial program.  The picture can be seen below.  A small program with simple logic was used to confirm the wiring of the LED.
 


Figure 2. Establishing the circuit with white LED


The program uses a switch case statement to handle the state machine.  The program is kept minimalistic via function call outs.  There are many functions in this program despite the programs overall complexity.  This is done for readability, debugging, and reusability.


The program was then confirmed to perform according to the guidelines specified by the lab instructions.  When the button was pressed the LEDs would increment to the next color.  
Part 2:  Sequencing colors using two push buttons in reverse direction

The code from part 1 was used and expanded to accommodate for the new requirements of part 2.  An additional input, button, and switch debounce was added to the circuit and program.  Since the state machine from part 1 was robust enough, no additional logic was required to allow it to operate correctly.  

New logic was written to handle the second button and the increment and decrement of the states, which depends on the button pressed.  This is controlled in the “stateControl()” function.  It will assign the state, and then the program will move into the switch case and assign the outputs.  The difficult part was handling both buttons to be pressed at the same time. 

The “buttonCheck()” function handles the checking of the buttons.  Logically it was written to prevent only one of the buttons from being checked.  If one is checked, then the other is checked.  The response of both buttons pressed was extremely poor before this logic was re-worked.  It was seen that the program would just happen to land on the second button press, skip the first button, and enter the state-machine.  This poor response could also be mitigated by incorporating interrupts, which would be the next improvement to be made.  Once the program was seen operating correctly with the three LED hooked up in the circuit, they were removed and the RGB was installed.

The RGB circuit required a different circuit all together.  The common cathode characteristic of the RGB LED was investigated and the LED was installed using the resistor values calculated in the prelab.  Using the 5V output of the MSP432, and the pin output voltages, the circuit was created and tested.  It was observed that the output currents would not allow the voltage to pass the forward bias of the LEDs.  Once this was discovered, the circuit was remade, to exclude the MOSFETs.  The working circuit can be seen below.     





Figure 3. Establishing the circuit with white LED


Using this circuit, and the program found in the Appendix, part 2 of the lab was tested, and verified.  One button will run the sequence as RGB, and the other will run the sequence BGR.  Both buttons pressed results in the LED turning off.

Table 1. Pin Table
 PINS
Component
3.4
Red
3.5
Green
3.6
Blue
6.4
Pushbutton 1
5.5
Pushbutton 2


Conclusion
The features of CCS and the MSP432 were refreshed, and the lab was completed successfully.  Two push buttons were integrated with the RGB LED  and MSP432 using a program with a state machine and software debouncers to handle the physical properties of metallic contact switches.Additional time was taken to get back up to speed from where EGR226 had left off.  When interrupts were added to the program, it was realized that MSP432-Ware had not been installed during the initial installation of CCS.  Without MSP432-Ware installed, IT Resource Explorer could not be used, and therefore, a driverLib project could not be opened.  Since the finish of this lab, all supporting software for CCS has been installed in preparation for the next labs.  
































Appendix
Figure A1: Part 1 additional work


Figure A1: Part 1 additional work

Figure A2: Part 2 additional work

Figure A2: Part 2 additional work
