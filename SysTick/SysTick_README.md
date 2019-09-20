# SysTick Timer aka count down timer

## Index
 1. Initialize with or without interrupts
 2. Use in Milli or Microseconds
 3. Optional Handler
## Functions

Initialize the SysTick peripheral for use **without** interrupts (busy-waits)

    void SysTickInit_NoInterrupts(void);
Initialize the SysTick peripheral for use **with** interrupts (busy-waits)

    void SysTickInit_WithInterrupts(void)
Use the SysTick timer to delay a specified number of **milliseconds**

    void SysTick_delay_ms(uint32_t ms_delay)
Use the SysTick timer to delay a specified number of **microseconds**


    void SysTick_delay_us(uint32_t us_delay)

   
Use the SysTick Handler

     void SysTick_Handler(void)





