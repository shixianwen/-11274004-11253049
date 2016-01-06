/*******************************************************************
* Copyright (c) 2010 Beijing Jiaotong University
* All rights reserved.
*
* File Name	：AIC23.c
* Description	：This file provides all the functions of AIC23
* Version	：0.1
* Author	：suiris
* Date		：MM/DD/YY
********************************************************************/

/* Includes ---------------------------------------------------*/
#include "AIC23.h"
/* Private typedef --------------------------------------------*/
/* Private define ---------------------------------------------*/
#define SPI2_DR_Address    0x4000380C
u16 DigiEnable =0x1201;
/* Private macro ----------------------------------------------*/
/* Private variables ------------------------------------------*/
extern u8 bufferx[0x4000]; 
/* Extern variables -------------------------------------------*/
u16 RestReg           =0x1e00;
u16 LeftLineIn        =0x0197;  //Left line input volume:0 dB;Muted
u16 RightLineIn       =0x0397;  //Right Line Input Volume:0 dB;Muted
u16 LeftHPVolume      =0x04f9;  //Left Channel Headphone Volume:0db
u16 RightHPVolume     =0x06f9;  //Right Channel Headphone Volume:0db
u16 AnalogAudioCtrReg =0x08f2;  //Added SideTone Disabled,Bypass Off,ADC Input from Line,Microphone muted,Microphone boost 0dB
u16 DigiAudioCtrReg   =0x0a00;  //DAC Soft mute Disabled,De-emphasis control=44.1 kHz,ADC high-pass filter Enabled
u16 PowerCtrReg       =0x0c02;  //Microphone input Off
u16 DigiInterReg      =0x0e02;  //Slave mode，I2S 16位模式
u16 SampleCtrReg      =0x10a2;  //Normal mode，Sampling rate=44.1K
/* Private function prototypes --------------------------------*/
void I2S_Configuration(void);
void DMA_Configuration(void);
/* Private functions ------------------------------------------*/
/****************************************************************
* Function Name  : I2S_Configuration
* Description    : Description of the I2S_Configuration
* Input          : None
* Output         : None
* Return         : None
*****************************************************************/
void I2S_Configuration(void)
{
  I2S_InitTypeDef I2S_InitStructure;
  
  /* I2S peripheral configuration */
  I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16bextended;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
  I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_44k;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  I2S_Init(SPI2, &I2S_InitStructure);
  
  /* Enable the I2S2 */
  I2S_Cmd(SPI2, ENABLE);
}
/****************************************************************
* Function Name  : DMA_Configuration
* Description    : Description of the DMA_Configuration
* Input          : None
* Output         : None
* Return         : None
*****************************************************************/
void DMA_Configuration(void)
{
  DMA_InitTypeDef    DMA_InitStructure;
  /* DMA1 Channel5 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel5);  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)SPI2_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)bufferx;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 0x4000;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);

  /* Enable SPI2 DMA Tx request */
  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE); 
    
}
/****************************************************************
* Function Name  : I2S_WriteAICReg
* Description    : Description of the I2S_WriteAICReg
* Input          : - Reg: Address of the Reg.
* Output         : None
* Return         : None
*****************************************************************/
void I2S_WriteAICReg(u16 Reg)
{
  GPIO_ResetBits(GPIOA, GPIO_Pin_4);
  /* Send a data from SPI2 */
  SPI_I2S_SendData(SPI1, Reg);
  /* Wait the Tx buffer to be empty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

/****************************************************************
* Function Name  : AIC23_Init
* Description    : Description of the AIC23_Init
* Input          : None
* Output         : None
* Return         : None
*****************************************************************/
void AIC23_Init(void)
{  
  SPI_InitTypeDef SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  /* GPIOA, GPIOB and SPI1 clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                         RCC_APB2Periph_SPI1, ENABLE);
  /* SPI2 and SPI3 clocks enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE);
  
  /* Configure SPI2 pins: WS, CK and SD ---------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure SPI1 pins: NSS, SCK and MOSI ----------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //CS23
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* Enable SPI1 NSS output for master mode */
  SPI_SSOutputCmd(SPI1, ENABLE);
  
  /* Enable SPI1 */
  SPI_Cmd(SPI1, ENABLE);

  I2S_Configuration();
  
  DMA_Configuration();
  
  I2S_WriteAICReg(RestReg);                //清零寄存器
  I2S_WriteAICReg(LeftLineIn);             //写左通道音频输入线控制寄存器
  I2S_WriteAICReg(RightLineIn);            //写右通道音频输入线控制寄存器
  I2S_WriteAICReg(LeftHPVolume);           //写左耳机音量控制寄存器
  I2S_WriteAICReg(RightHPVolume);          //写右耳机音量控制寄存器
  I2S_WriteAICReg(AnalogAudioCtrReg);      //写模拟通道控制寄存器
  I2S_WriteAICReg(DigiAudioCtrReg);        //写数字通道控制寄存器
  I2S_WriteAICReg(DigiInterReg);           //写数字音频接口设置寄存器
  I2S_WriteAICReg(PowerCtrReg);            //写电源控制寄存器
  I2S_WriteAICReg(SampleCtrReg);           //写采样速率控制寄存器
  I2S_WriteAICReg(DigiEnable);             //写数字通道激活控制寄存器
  
  //-------------------------------------------------------  
  /* Disable SPI1 NSS output for master mode */
  SPI_SSOutputCmd(SPI1, DISABLE);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
}