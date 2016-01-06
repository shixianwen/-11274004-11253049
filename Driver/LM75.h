/***************************************************************
* Copyright (c) 2010 Beijing Jiaotong University
* All rights reserved.
*
* File Name	��LM75.h
* Description	��This file contains the headers of LM75.c
* Version	��0.1
* Author	��Author
* Date		��MM/DD/YY
****************************************************************/
/* Define to prevent recursive inclusion ------------------*/
#ifndef LM75_H
#define LM75_H
/* Includes -----------------------------------------------*/
//#include "LM75.h"
//#include "stm32f10x.h"
/* Exported types -----------------------------------------*/
/* Exported constants -------------------------------------*/
/* Exported macro -----------------------------------------*/
#define LM75_R_ADDRESS          0x91
#define LM75_W_ADDRESS          0x90
/* Exported variables -------------------------------------*/
/* Exported functions -------------------------------------*/
u8 LM75_read_temperature(int *temp);
#endif /* FILENAME_H */