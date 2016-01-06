/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

// wen, 2009.5.20

#ifndef  __BSP_H__
#define  __BSP_H__


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_GLOBALS
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void            BSP_Init               (void);
void            BSP_IntDisAll          (void);
INT32U          BSP_CPU_ClkFreq        (void);


void  Tmr_TickInit  (void);         //config & start Systick clock

void  Tmr_TickISR_Handler(void);    //SysTick ISR


#endif         /* End of module include.                               */
