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
#ifndef __EIOS_H
#define __EIOS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#include "LM75.h"
#include "AD9832.h"
#include "AIC23.h"

extern int gSPI_CONFIGURED;
/* Exported functions ------------------------------------------------------- */

//void FSMC_AD7302_WriteBuffer(u8* pBuffer, u32 WriteAddr, u32 NumHalfwordToWrite);
u8 FSMC_ADS7842_Read(u8 ReadAddr);
void SPI_Configuration(void);

/* AD7842 4-channel, 8-bit ADC*/
u8 AD_ConvChannel(int ch);
void AD_StartChannel(int ch);
int AD_IsBusy(void);
u8 AD_Read(void);
void AD_Init(void);

/* AD7302 2-ch, 8-bit DAC */
void DA_Init(void);
void DA_Output(int ch, u8 value);

/* CH375 USB M/S chip */
void USB_PinSet(void);
void USB_IntSet(void);

#endif /* __EIOS_H */

