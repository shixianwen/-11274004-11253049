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
#ifndef __FSMC_SRAM_H
#define __FSMC_SRAM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define SETLED0             GPIOF->BSRR = GPIO_Pin_6 
#define RESETLED0           GPIOF->BRR  = GPIO_Pin_6
#define SETLED1             GPIOF->BSRR = GPIO_Pin_7 
#define RESETLED1           GPIOF->BRR  = GPIO_Pin_7
#define SETLED2             GPIOF->BSRR = GPIO_Pin_8 
#define RESETLED2           GPIOF->BRR  = GPIO_Pin_8
#define SETLED3             GPIOF->BSRR = GPIO_Pin_9 
#define RESETLED3           GPIOF->BRR  = GPIO_Pin_9
#define SETLED4             GPIOF->BSRR = GPIO_Pin_10
#define RESETLED4           GPIOF->BRR  = GPIO_Pin_10


/* Exported functions ------------------------------------------------------- */
void LEDLight(int num, int on);
void Beep(int on);

void FSMC_Init(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
void LED_Configuration(void);
void USART_Configuration(void);
void FSMC_AD7302_WriteBuffer(u8* pBuffer, u32 WriteAddr, u32 NumHalfwordToWrite);
u8 FSMC_ADS7842_Read(u8 ReadAddr);
void SPI_Configuration(void);
void Fill_Buffer(u8 *pBuffer, u16 BufferLenght, u32 Offset);
//static void _int2str( u8* ptr, s32 X, u16 digit, s32 flagunsigned, s32 fillwithzero );
//void UTIL_uint2str( u8* ptr, u32 X, u16 digit, s32 fillwithzero );
//void UTIL_int2str( u8* ptr, s32 X, u16 digit, s32 fillwithzero );
#endif /* __FSMC_SRAM_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
