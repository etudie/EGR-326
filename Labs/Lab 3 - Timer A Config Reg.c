/********************************************************************************************************************
 * TIMER A INITIALIZATIONS: 
 *   	
 *   	
 *   	PWM Pin Config:	1. Set the pin as the alternate SEL mode for the timer (SEL0 & SEL1)
 *                      	2. Set the pin as an output (Px->DIR  |=  BITx)
 *                      	3. Set the pin high (Px->OUT  |=  BITx) 
 *   	
 *   	PWM Timer Config:  1. TIMER_A2->CTL  =  0x0214;                	Set the configuration of the clock
 *                      	2. TIMER_A2->CCR[0] = 60000;                    PWM Period (# cycles of the clock)
 *                      	3. TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;  Output mode, count up
 *                      	4. TIMER_A2->CCR[1] = 0;                        CCR1 PWM duty cycle in 10ths of percent                 	
 *                       	
 *                       	
 *  	Intpt Pin Config:   1. Set the pin as the alternate SEL mode for the timer (SEL0 & SEL1)
 *                      	2. Set pin as input 
 *                                          	
 *  	Intpt Timer Config: 1. Set the CTL reg (TIMER_A0->CTL  =  0x0220)      SMCLK, interrupts disabled, continuous mode, no divider
 *                      	2. Set the CCTL reg (TIMER_A0->CCTL[2] = 0xCD78)   Intrpt en, set/rst, capture, sync, CCIxA signal, rising edge
 *                      	
 *  	
 *  --------------------------------------REGISTER INFORMATION------------------------------------------------- 
 *                       	
 *                                  	
 *  TAxCTL Register -   Timer Control Register: The control register will setup the clock souorce configuration. 
 *                  	This clock source configuration will be consistent throughout all of the timer instances.
 *                      	
 *  	TIMER_A0->CTL 	BIT0 = 0   	TAIFG   -   0 = No interrupt pending 1 = Interrupt pending
 *  	TIMER_A0->CTL 	BIT1 = 0   	TAIE    -   Timer_A interrupt enable 0 = Interrupt dis. 1 = Interrupt enabled
 *  	TIMER_A0->CTL 	BIT2 = 1   	TACLR   -   Timer_A clear,  bit is automatically reset and is always read as zero
 *  	TIMER_A0->CTL 	BIT3 = 0   	RESERVED
 * 
 *  	TIMER_A0->CTL 	BIT4 = 1   	MC      -   00 = Stop mode 01 = Up mode 10 = Continuous mode 11 = Up/down mode
 *  	TIMER_A0->CTL 	BIT5 = 0   	MC     	
 *  	TIMER_A0->CTL 	BIT6 = 0   	ID      -   CLK input divider (00b = /1) (01b = /2) (10b = /4) (11b = /8)
 *  	TIMER_A0->CTL 	BIT7 = 0   	ID     	
 *
 *  	TIMER_A0->CTL 	BIT8 = 0   	TASSEL  -   Timer_A CLK source select
 *  	TIMER_A0->CTL 	BIT9 = 1   	TASSEL  -   (00b = TAxCLK) (01b = ACLK) (10b = SMCLK) (11b = INCLK)	
 *  	TIMER_A0->CTL	BIT10 = 0   	RESERVED
 *  	TIMER_A0->CTL	BIT11 = 0   	RESERVED       	
 *       	
 *  	
 *  	
 *  	
 *  TAxCCTL Register - Capture/Compare Control Register: Controls the configurations of each timer instance.  The instance will
 *                     also be driven by the COMMON timer control register above.
 *  	
 *  	TIMER_A0->CCTL[1]	BIT0  = 0	CCIFG	-   Capture compare: 0 = No interrupt pending 1 = Interrupt pending
 *  	TIMER_A0->CCTL[1]	BIT1  = 0	COV	  -   Indicates a capture overflow occurred overflow = 1, rst in software
 *  	TIMER_A0->CCTL[1]	BIT2  = 0	OUT  	-   Output. For output mode 0, (output low = 0) (output High = 1)
 *  	TIMER_A0->CCTL[1]	BIT3  = 1	CCI  	-   Capture/compare input. The selected input signal can be read by this bit.
 *
 *  	TIMER_A0->CCTL[1]	BIT4  = 1	CCIE 	-   Capture/compare interrupt enable, (0 = disabled) (1 = enabled)
 *  	TIMER_A0->CCTL[1]	BIT5  = 1	OUTMOD   -   Output mode. (000 = OUT bit value) (001 = Set) (010 = Toggle/reset)
 *  	TIMER_A0->CCTL[1]	BIT6  = 1	OUTMOD   -     (011 = Set/reset) (100 = Toggle)(101 = Reset) (110 = Toggle/set)
 *  	TIMER_A0->CCTL[1]	BIT7  = 0	OUTMOD   - 	(111 = Reset/set) (011 = Set/reset)
 *
 *  	TIMER_A0->CCTL[1]	BIT8  = 1	CAP  	-   Capture mode (0 = Compare mode) (1 = Capture mode)
 *  	TIMER_A0->CCTL[1]	BIT9  = 0	RESERVED
 *  	TIMER_A0->CCTL[1]	BIT10 = 1    SCCI 	-   Synchronized capture/compare input. The selected CCI input signal can be read via this bit.
 *  	TIMER_A0->CCTL[1]	BIT11 = 1    SCS  	-   Synchronize capture source. This bit is used to synchronize the capture
 *                                  	                input signal with the timer clock (0 = Asynchronous capture) (1 = Synchronous capture)
 *
 *  	TIMER_A0->CCTL[1]	BIT12 = 0    CCIS 	-   Capture/compare input select. These bits select the TAxCCR0 input signal.
 *  	TIMER_A0->CCTL[1]	BIT13 = 0    CCIS 	- 	00b = CCIxA 01b = CCIxB 10b = GND 11b = VCC
 *  	TIMER_A0->CCTL[1]	BIT14 = 0    CM   	-   Capture mode 00b = No capture (01b = Capture on rising edge) 10b = Capture on falling edge
 *  	TIMER_A0->CCTL[1]	BIT14 = 1    CM   	- 	11b = Capture on both rising and falling edges
 *  
 *  
 *  
 *  TAxCCR Register - Timer_Ax Capture/Compare Register: Capture mode is usually used for inpt signals, compare is usually used for PWM outputs
 * 
 *    	Compare mode: TAxCCRn holds the data for the comparison to the timer value in the Timer_A Register, TAxR
 *    	Capture mode: The Timer_A Register, TAxR, is copied into the TAxCCRn register when a capture is performed.
 *  	
 *  	
 *   TAxIV Register - Timer_Ax Interrupt Vector Register: Used when many interrupts need priority control
 *
 *    	Timer_A interrupt vector value 00h = No interrupt pending 02h = Interrupt Source: Capture/compare 1;
 *    	Interrupt Flag: TAxCCR1 CCIFG;
 *    	Interrupt Priority: Highest 04h   = Interrupt Source: Capture/compare 2;
 *    	Interrupt Flag: TAxCCR2 CCIFG 06h = Interrupt Source: Capture/compare 3;
 *    	Interrupt Flag: TAxCCR3 CCIFG 08h = Interrupt Source: Capture/compare 4;
 *    	Interrupt Flag: TAxCCR4 CCIFG 0Ah = Interrupt Source: Capture/compare 5;
 *    	Interrupt Flag: TAxCCR5 CCIFG 0Ch = Interrupt Source: Capture/compare 6;
 *    	Interrupt Flag: TAxCCR6 CCIFG 0Eh = Interrupt Source: Timer overflow;
 *    	Interrupt Flag: TAxCTL TAIFG; Interrupt Priority: Lowest * 
 *    	
 *    	
 *   TAxEX Register - Timer_Ax Expansion 0 Register: Use when the timer max value needs to be increased
 *  
 *    	15-3 Reserved R 0h Reserved. Reads as 0. 2-0 TAIDEX RW 0h Input divider expansion. These bits along with the ID bits select the divider for the input clock.
 *    	000b = Divide by 1
 *    	001b = Divide by 2
 *    	010b = Divide by 3
 *    	011b = Divide by 4
 *    	100b = Divide by 5
 *    	101b = Divide by 6
 *    	110b = Divide by 7
 *    	111b = Divide by 8	
 *  	
 ********************************************************************************************************************/
