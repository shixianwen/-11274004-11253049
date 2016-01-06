/***************************************************************
* Copyright (c) 2010 Beijing Jiaotong University
* All rights reserved.
*
* File Name	£ºfilename.h
* Description	£ºThis file contains the headers of filename.c
* Version	£º0.1
* Author	£ºAuthor
* Date		£ºMM/DD/YY
****************************************************************/
/* Define to prevent recursive inclusion ------------------*/
#ifndef __I2C_H
#define __I2C_H
/* Includes -----------------------------------------------*/
#include "stm32f10x.h"
/* Exported types -----------------------------------------*/
/* Exported constants -------------------------------------*/
/* Exported macro -----------------------------------------*/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_6 
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6  
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7 
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7 
#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7 
/* Exported variables -------------------------------------*/
/* Exported functions -------------------------------------*/
void I2C_IO_init(void);
void I2C_delay(void); 
int I2C_Start(void); 
void I2C_Stop(void); 
int I2C_WaitAck(void); 
void I2C_Ack(void);
void I2C_NoAck(void);
void I2C_SendByte(u8 SendByte);
u8 I2C_ReceiveByte(void);
#endif /* __I2C_H */