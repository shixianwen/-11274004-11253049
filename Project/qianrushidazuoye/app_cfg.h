/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                       ADDITIONAL uC/MODULE ENABLES
*********************************************************************************************************
*/

#define  uC_PROBE_OS_PLUGIN              DEF_ENABLED            /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
#define  uC_PROBE_COM_MODULE             DEF_ENABLED

/*
*********************************************************************************************************
*                                            TASK PRIORITIES 
*********************************************************************************************************
*/

//#define  TASK_START_PRIO                  4
//#define  TASK1_PRIO                      5
//#define  TASK2_PRIO                      6
//#define  TASK3_PRIO                      7



//#define  OS_PROBE_TASK_PRIO                    8
//#define  OS_PROBE_TASK_ID                      8

//#define  OS_TASK_TMR_PRIO              (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/

//#define  TASK_START_STK_SIZE            128
//#define  TASK1_STK_SIZE                64
//#define  TASK2_STK_SIZE                64
//#define  TASK3_STK_SIZE                64



//#define  OS_PROBE_TASK_STK_SIZE             256

/*
*********************************************************************************************************
*                               uC/Probe plug-in for uC/OS-II CONFIGURATION
*********************************************************************************************************
*/

#define  OS_PROBE_TASK                         1                /* Task will be created for uC/Probe OS Plug-In             */
#define  OS_PROBE_TMR_32_BITS                  0                /* uC/Probe OS Plug-In timer is a 16-bit timer              */
#define  OS_PROBE_TIMER_SEL                    2
#define  OS_PROBE_HOOKS_EN                     1




#endif
