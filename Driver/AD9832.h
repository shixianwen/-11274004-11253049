/***************************************************************
* Copyright (c) 2010 Beijing Jiaotong University
* All rights reserved.
*
* File Name	£ºAD9832.h
* Description	£ºThis file contains the headers of AD9832.c
* Version	£º0.1
* Author	£ºAuthor
* Date		£ºMM/DD/YY
****************************************************************/

/* Define to prevent recursive inclusion ------------------*/
#ifndef AD9832_H
#define AD9832_H

/* Includes -----------------------------------------------*/
#include "stm32f10x.h"
/* Exported types -----------------------------------------*/
/* Exported constants -------------------------------------*/
//#define AD9832_FMCLK 25    //Digital Clock Input fMCLK.
#define AD9832_FMCLK 25000000    //Digital Clock Input fMCLK.
/* Exported macro -----------------------------------------*/
// Command of AD9832 --------------------------------------
#define AD9832_WRPHA_CMD1       0x0000//Write 16 phase bits(Present 8 Bits + 8 Bits in Defer Register) to Selected PHASE REG.
#define AD9832_WRPHA_CMD2       0x1000//Write 8 phase bits to Defer Register.
#define AD9832_WRFREQ_CMD1      0x2000//Write 16 frequency bits (Present 8 Bits + 8 Bits in Defer Register) to Selected FREQ REG.
#define AD9832_WRFREQ_CMD2      0x3000//Write 8 frequency bits to Defer Register.
#define AD9832_SELPHASE_CMD     0x4000//To Select the PHASE REG when SELSRC = 1. 
#define AD9832_SELFREQ_CMD      0x5000//To Select the FREQ REG when SELSRC = 1.
#define AD9832_SELFP_CMD        0x6000//To control the PSEL0, PSEL1 and FSELECT bits when SELSRC = 1.
#define AD9832_CONF_CMD1        0x8000
#define AD9832_CONF_CMD2        0xC000

// Command bit of the AD9832_CONF_CMD1 --------------------
#define SYNC                    0x2000
#define SELSRC_USE_PINS         0x0000
#define SELSRC_USE_REGS         0x1000
// Command bit of the AD9832_CONF_CMD2 --------------------
#define AD9832_SLEEP            0x2000
#define AD9832_RESET            0x1000
#define AD9832_CLR              0x0800

// Addressing the Registers -------------------------------
#define FREQ0_L_LSBs  0x000
#define FREQ0_H_LSBs  0x100
#define FREQ0_L_MSBs  0x200
#define FREQ0_H_MSBs  0x300
#define FREQ1_L_LSBs  0x400
#define FREQ1_H_LSBs  0x500
#define FREQ1_L_MSBs  0x600
#define FREQ1_H_MSBs  0x700
#define PHASE0_LSBs   0x800
#define PHASE0_MSBs   0x900
#define PHASE1_LSBs   0xa00
#define PHASE1_MSBs   0xb00
#define PHASE2_LSBs   0xc00
#define PHASE2_MSBs   0xd00
#define PHASE3_LSBs   0xe00
#define PHASE3_MSBs   0xf00

/* Exported variables -------------------------------------*/
/* Exported functions -------------------------------------*/

#define DDS_Init  AD9832Init
#define DDS_WriteFreq AD9832_WriteFreq
#define DDS_WritePhase AD9832_WritePhase

void DDS_Sine(float Freqf);
void DDS_FSK(float sf1, float sf2, float mf);
void DDS_FSKClr(void);

void AD9832Init(void);
void AD9832_WriteFreq(float Freqf , u8 Reg);
void AD9832_WritePhase(float Phasef , u8 Reg);

#endif /* AD9832_H */