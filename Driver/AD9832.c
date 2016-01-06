/*******************************************************************
* Copyright (c) 2010 Beijing Jiaotong University
* All rights reserved.
*
* File Name	：AD9832.c
* Description	：This file provides all the functions of AD9832.c
* Version	：0.1
* Author	：Author
* Date		：MM/DD/YY
********************************************************************/

/* Includes ---------------------------------------------------*/
#include "AD9832.h"
#include "eios.h"

/* Private typedef --------------------------------------------*/
/* Private define ---------------------------------------------*/
/* Private macro ----------------------------------------------*/
/* Private variables ------------------------------------------*/
uc16 FreqREGAddr[2][4] = {FREQ0_L_LSBs,FREQ0_H_LSBs,FREQ0_L_MSBs,FREQ0_H_MSBs,
                            FREQ1_L_LSBs,FREQ1_H_LSBs,FREQ1_L_MSBs,FREQ1_H_MSBs};
uc16 PhaseREGAddr[4][2] = {PHASE0_LSBs,PHASE0_MSBs,
                            PHASE1_LSBs,PHASE1_MSBs,
                            PHASE2_LSBs,PHASE2_MSBs,
                            PHASE3_LSBs,PHASE3_MSBs};
/* Private function prototypes --------------------------------*/
void AD9832_SendData(u16 ad9832_data);
/* Private functions ------------------------------------------*/
/****************************************************************
* Function Name  : AD9832_SendDate
* Description    : Description of the AD9832_AD9832_SendDate
* Input          : -u16 ad9832_data: Description of the parameter.
* Output         : None
* Return         : None
*****************************************************************/
void AD9832_SendData(u16 ad9832_data)
{
  GPIO_ResetBits(GPIOG, GPIO_Pin_13);
  /* Wait for SPI1 Tx buffer empty */
  SPI_I2S_SendData(SPI1, ad9832_data);
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
  GPIO_SetBits(GPIOG, GPIO_Pin_13);
}
/****************************************************************
* Function Name  : AD9832Init
* Description    : Description of the AD9832Init
* Input          : None 
* Output         : None
* Return         : None
*****************************************************************/
void AD9832Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;        //

  //PB5 as GPIO out : FSEL
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // PG10, PG11 gpio out: PSEL0, PSEL1
  //PG10 = FSMC_NE3 与SRAM总线冲突 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;//GPIO_Pin_10 | ;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  if(!gSPI_CONFIGURED)
    SPI_Configuration();
  //
  GPIO_ResetBits(GPIOG, GPIO_Pin_10); 
  GPIO_ResetBits(GPIOG, GPIO_Pin_11);
  GPIO_ResetBits(GPIOB, GPIO_Pin_5);
  AD9832_SendData(AD9832_CONF_CMD2 | AD9832_SLEEP | AD9832_RESET | AD9832_CLR);   //复位AD9832
  AD9832_SendData(AD9832_CONF_CMD1 | SYNC | SELSRC_USE_PINS);//用位选择频率相位寄存器
  AD9832_SendData(0x3088);//分四次把频率控制字送入寄存器0
  AD9832_SendData(0x2188);
  AD9832_SendData(0x3288);
  AD9832_SendData(0x2300);
  AD9832_SendData(0x1800);//初始化相位寄存器0
  AD9832_SendData(0x0900);
  AD9832_SendData(AD9832_SELFP_CMD);//选择频率寄存器0、相位寄存器0
  AD9832_SendData(AD9832_CONF_CMD2);//AD9832开始输出
}
/****************************************************************
* Function Name  : AD9832_WriteFreq
* Description    : Write Freq Data to AD9832 (FREG = fOUT0/fMCLK*232)
* Input          : - Freqf: Description of the parameter.
*                  - Reg: Description of the parameter.
*                    This parameter can be one of the following values:
*                       - 0: FREQ REG0
*                       - 1: FREQ REG1
* Output         : None
* Return         : None
*****************************************************************/
void AD9832_WriteFreq(float Freqf , u8 Reg)
{
  u16 TempData;
  u32 Freq;

  Freqf/=AD9832_FMCLK;      //25000000
  Freqf*=65536.0;
  Freqf*=65536.0;
    
//  Freq = (u32)(Freqf*4294967296/AD9832_FMCLK);
  Freq=(u32)(Freqf+0.5) ;
  
  TempData = AD9832_WRFREQ_CMD2 | FreqREGAddr[Reg][0]
                                | (u16)(Freq & 0x0ff);
  AD9832_SendData(TempData);
  
  TempData = AD9832_WRFREQ_CMD1 | FreqREGAddr[Reg][1] 
                                | (u16)(Freq>>8 & 0x0ff);
  AD9832_SendData(TempData);

  TempData = AD9832_WRFREQ_CMD2 | FreqREGAddr[Reg][2]
                                | (u16)(Freq>>16 & 0x0ff);
  AD9832_SendData(TempData);

  TempData = AD9832_WRFREQ_CMD1 | FreqREGAddr[Reg][3]
                                | (u16)(Freq>>24 & 0x0ff);
  AD9832_SendData(TempData);
}
/****************************************************************
* Function Name  : AD9832_WritePhase
* Description    : Write Phase Data to AD9832 (Phase = 2 pi/4096 * PHASEREG)
* Input          : - Phasef: Description of the parameter.
*                  - Reg: Description of the parameter.
*                    This parameter can be one of the following values:
*                       - 0: PHASE REG0
*                       - 1: PHASE REG1
*                       - 2: PHASE REG2
*                       - 3: PHASE REG3
* Output         : None
* Return         : None
*****************************************************************/
void AD9832_WritePhase(float Phasef , u8 Reg)
{
  u16 TempData;
  u32 Phase;

  Phase = (u16)(Phasef*4096/2*3.14159);
  
  TempData = AD9832_WRPHA_CMD2 | PhaseREGAddr[Reg][0]
                               | (Phase & 0x0ff);
  AD9832_SendData(TempData);
  
  TempData = AD9832_WRPHA_CMD1 | PhaseREGAddr[Reg][1]
                               | (Phase>>8 & 0x0ff);
  AD9832_SendData(TempData);
}
//single freq wave, f=0.1 Hz -- 10MHz
void DDS_Sine(float Freqf)
{
  GPIO_InitTypeDef GPIO_InitStructure;        //
  //set PB5 as GPIO outpp
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //clear PB5 out
  GPIO_ResetBits(GPIOB, GPIO_Pin_5);
  //set DDS f0 reg
  AD9832_WriteFreq(Freqf ,0);
  AD9832_WriteFreq(Freqf ,1);
  AD9832_WritePhase(0 , 0);
  AD9832_WritePhase(0 , 1);
  AD9832_WritePhase(0 , 2);
  AD9832_WritePhase(0 , 3);
}

void DDS_FSK(float sf1, float sf2, float mf)
{
  //set PB5 as AF
  
  //remap PB5 as TIM3_CH2

  //set sf1 & sf2 reg

  //set time3 ch2 PWM output, duty =50%, f=mf

}
void DDS_FSKClr(void)
{
  //PB5 as GPIO outPP

  //set PB5=0

  //stop TIM3
  
}

