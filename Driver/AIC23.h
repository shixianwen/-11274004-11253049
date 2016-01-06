/***************************************************************
* Copyright (c) 2010 Beijing Jiaotong University
* All rights reserved.
*
* File Name	��AIC23.h
* Description	��This file contains the headers of AIC23.c
* Version	��0.1
* Author	��suiris
* Date		��MM/DD/YY
****************************************************************/

/* Define to prevent recursive inclusion ------------------*/
#ifndef AIC23_H
#define AIC23_H
/* Includes -----------------------------------------------*/
#include "stm32f10x.h"
#include "AIC23.h"
/* Exported types -----------------------------------------*/
//typedef struct
//{
//u16 RestReg;
//u16 LeftLineIn;
//u16 RightLineIn;
//u16 LeftHPVolume;
//u16 RightHPVolume;
//u16 AnalogAudioCtrReg;
//u16 DigiAudioCtrReg;
//u16 PowerCtrReg;
//u16 DigiInterReg;
//u16 SampleCtrReg;
//}AIC23Reg_TypeDef;
/* Exported constants -------------------------------------*/
/* Exported macro -----------------------------------------*/
/* Exported variables -------------------------------------*/
extern u16 RestReg,LeftLineIn, RightLineIn, LeftHPVolume, RightHPVolume,
           AnalogAudioCtrReg, DigiAudioCtrReg, PowerCtrReg, DigiInterReg, 
           SampleCtrReg, DigiEnable;
/* Exported functions -------------------------------------*/
void AIC23_Init(void);
void I2S_WriteAICReg(u16 Reg);
#endif /* AIC23_H */