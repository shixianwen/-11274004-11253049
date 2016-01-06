/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : fsmc_sram.h
* Author             : MCD Application Team
* Version            : V2.0.3
* Date               : 09/22/2008
* Description        : Header for fsmc_sram.c file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "CH452.h"
//use IAR C compiler preprocessor constant definition
#ifdef LCD_GRAPH
#include "lcdlib.h"   //ºº×Ö×Ö¿â£¬Ìá¹©Í¼ÎÄÏÔÊ¾º¯Êý
#else
#include "lcdcon.h"     //Ó¢ÎÄ×Ö¿â£¬Ä£Äâ×Ö·ûÖÕ¶Ë
#endif

#include "lm75.h"

/* Exported functions ------------------------------------------------------- */
int System_Init(void);
void LEDLight(int num, int on);
void LEDLights(int mask, int on);

void Beep(int on);

void UART_Init(void);


#endif /* __BSP_H */

