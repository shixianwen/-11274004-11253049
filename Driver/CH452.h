/***************************************************************
* Copyright (c) 2010 Beijing Jiaotong University
* All rights reserved.
*
* File Name	：CH452.h
* Description	：This file contains the headers of LM75.c
* Version	：0.1
* Author	：Author
* Date		：MM/DD/YY
****************************************************************/

/* Define to prevent recursive inclusion ------------------*/
#ifndef CH452_H
#define CH452_H
/* Includes -----------------------------------------------*/
#include "stm32f10x.h"
#include "CH452CMD.h"	// CH452常量定义

#define LEDDisp(d,c) LEDDispBCD(d,c)
#define LEDBlink(m) LEDDispBlink(m)

/* Exported functions -------------------------------------*/
extern volatile int gKeyPressed;


u8 GetKey(void);
void LEDDispBlink(u8 digmap);
void LEDDispBCD(u8 digit, u8 code);

/* implementation calls */
void CH452_Init(void);
void CH452_Write(unsigned short cmd);
unsigned char CH452_Read(void);
//u8 KEY_VALUE(vu8 value);
//u8 CH452_KeyCode(u8 value);
//u8 keyName(u8 code);

#endif /* FILENAME_H */