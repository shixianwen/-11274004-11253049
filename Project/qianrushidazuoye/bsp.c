
#define  BSP_GLOBALS
#include "includes.h"
#include <yfuns.h>

//#define USE_UART_STDIO



void  BSP_Init (void)
{

    //Tmr_TickInit(); // Initialize the uC/OS-II tick interrupt
}



/*
*********************************************************************************************************
*                                      GET THE CPU CLOCK FREQUENCY
*
* Description: This function reads CPU registers to determine the CPU clock frequency of the chip in KHz.
*
* Argument(s): None.
*
* Returns    : The CPU clock frequency, in Hz.
*********************************************************************************************************
*/

INT32U  BSP_CPU_ClkFreq (void)
{
    static  RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);

    return ((INT32U)rcc_clocks.HCLK_Frequency);
}



#ifdef  USE_UART_STDIO


size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  size_t nChars = 0;

  //This template only writes to "standard out" and "standard err",
  //for all other file handles it returns failure. 
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
  {
    return _LLIO_ERROR;
  }

  for ( ; size != 0; --size)
  {
    USART_SendData(USART1, *buffer++);
    // Loop until the end of transmission 
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }
    ++nChars;
  }
  return nChars;
}
size_t __read(int handle, unsigned char * buffer, size_t size)
{
  size_t nChars = 0;

  if (handle != _LLIO_STDIN)
  {
    return _LLIO_ERROR;
  }

  for ( ; size != 0; --size)
  {
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;
    *buffer++=USART_ReceiveData(USART1);
    ++nChars;
  }
  return nChars;  
}  
/*
int fputc(int ch, FILE *f)
{
  // Write a character to the USART 
  USART_SendData(USART1, (u8) ch);
  // Loop until the end of transmission 
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {
  }

  return ch;
}

int fgetc(FILE * f)
{
  while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;
  return USART_ReceiveData(USART1);
}
*/
#endif




/*
******************************************************************************************************************************
******************************************************************************************************************************
**                                         uC/OS-II Timer Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       TICKER INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
*
* Arguments   : none
*
* Note(s)     : 1) The timer is setup for output compare mode BUT 'MUST' also 'freerun' so that the timer
*                  count goes from 0x00000000 to 0xFFFFFFFF to ALSO be able to read the free running count.
*                  The reason this is needed is because we use the free-running count in uC/OS-View.
*********************************************************************************************************
*/

void  Tmr_TickInit (void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;


    RCC_GetClocksFreq(&rcc_clocks);

    cnts = (INT32U)rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC;

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);  /* misc.c */
    /* set reload register, set Priority to lowest, Enable SysTick IRQ and SysTick Timer */
    SysTick_Config(cnts);                             /* core_cm3.h */

}


/*
*********************************************************************************************************
*                                         TIMER IRQ HANDLER
*
* Description : This function handles the timer interrupt that is used to generate TICKs for uC/OS-II.
*
* Arguments   : none
*
* Note(s)     : 1) The timer is 'reloaded' with the count at compare + the time for the next interrupt.
*                  Since we are using 'unsigned' integer math, overflows are irrelevant.
*********************************************************************************************************
*/

void  Tmr_TickISR_Handler (void)
{
    OS_CPU_SR  cpu_sr;


    OS_ENTER_CRITICAL();                                        /* Tell uC/OS-II that we are starting an ISR                */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    OSTimeTick();                                               /* Call uC/OS-II's OSTimeTick()                             */

    OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR                */
}
