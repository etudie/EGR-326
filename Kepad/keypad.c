#include "msp.h"

#include <stdio.h>

/*********

P4.0 - Row 0
P4.1 - Row 1
P4.2 - Row 2
P4.3 - Row 3
P4.4 - Col 0
P4.5 - Col 1
P4.6 - Col 2

***********/

void Print_Keys        (void);
void keypad_init       (void);          //initialize keypad
int  Read_Keypad       (void);          //function to read keys
void SysTick_Initialize(void);          //initializes systick timer
void SysTick_delay(uint16_t delay);     //systick timer function
uint8_t num, pressed;                   //Global Variables



void main(void)
{
       WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;              // stop watchdog timer
       keypad_init();
       SysTick_Initialize();
       while(1)
       {
           pressed = Read_Keypad( );         // Call Function to read Keypad
           if ( pressed )
           Print_Keys( );
           SysTick_delay (10);
       }
}

void keypad_init(void) {        //initialize rows and columns
    P4->SEL0 &=~(0xFF);
    P4->SEL1 &=~(0xFF);
    P4->DIR  &=~(0xFF);         //inputs
    P4->REN  |= (0x0F);         //enable resistor for rows but not columns
    P4->OUT  |= (0x0F);         //pull up
}

int Read_Keypad(void){
    int col, row;
    for ( col = 0; col < 3; col++ ){
        P4->DIR = 0x00;                     // Set Columns to inputs
        P4->DIR |=  BIT( 4 + col );         // Set column 3 to output
        P4->OUT &=~ BIT( 4 + col );         // Set column 3 to LOW
        SysTick_delay (10);                 // Delay the while loop
        row = P4->IN & 0x0F;                // read all rows
        while ( !(P4IN & BIT0) | !(P4IN & BIT1) | !( P4IN & BIT2) | !( P4IN & BIT3) );
        if (row != 0x0F)
            break;                          // if one of the input is low, some key is pressed.
    }

    P4->DIR = 0x00;                         // Set Columns to inputs
    if ( col == 3)
         return 0;
    if (row == 0x0E) num =     col +1;     // key in row 0
    if (row == 0x0D) num = 3 + col +1;     // key in row 1
    if (row == 0x0B) num = 6 + col +1;     // key in row 2
    if (row == 0x07) num = 9 + col +1;     // key in row 3
    return 1;
}



void SysTick_Initialize(void){              //Initialize systic timer
    SysTick -> CTRL = 0;
    SysTick -> LOAD = 0x0FFFFFF;
    SysTick -> VAL  = 0;
    SysTick -> CTRL = 0x00000005;
}

void SysTick_delay(uint16_t delay){         //systick delay function
    SysTick -> LOAD = ((delay * 3000) - 1);
    SysTick -> VAL = 0;
    while ((SysTick -> CTRL & 0x0001000) == 1);
}

void Print_Keys        (void)
{
    if(num<10){
       printf("%d\n",num);
    }
    if(num==10){

        printf("*\n",num);

        }

    if(num==11){

        printf("0\n",num);

    }

    if(num==12){

        printf("#\n",num);

        }

}

