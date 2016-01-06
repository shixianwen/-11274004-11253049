/*******************************************************************
* Copyright (c) 2010 Beijing Jiaotong University
* All rights reserved.
*
* File Name	��filename.c
* Description	��This file provides all the functions of filename.c
* Version	��0.1
* Author	��Author
* Date		��MM/DD/YY
********************************************************************/

/* Includes ---------------------------------------------------*/
#include "stm32f10x.h"
#include"LM75.h"
#include "I2C.h"
/* Private typedef --------------------------------------------*/
/* Private define ---------------------------------------------*/
/* Private macro ----------------------------------------------*/
/* Private variables ------------------------------------------*/
/* Extern variables -------------------------------------------*/
/* Private function prototypes --------------------------------*/
/* Private functions ------------------------------------------*/
/*******************************************************************************
* Function Name  :  u8 iic_read_temperature(void)
* Description    : ���մ��������������ݣ����ж����ߴ��󣨲���Ӧ���źţ��������
                   ��Ҫ����Ӧ������
* Input          : None
* Output         : int ���¶����ݣ������š�*temp=10*�¶�ֵ
* Return         : 1=OK, 0=error
*******************************************************************************/
/* S,  1 0 0 1 A2 A1 A0  1;  (A),  (MSBYTE),  A,  (LSBYTE),  NA,  P */
u8 LM75_read_temperature(int *temp)///i2c��LM75 
{  
  int t;
  *temp = 0;
//  I2C_Start();
//  I2C_SendByte(LM75_W_ADDRESS);
//  if(!I2C_WaitAck())
//    return(0);
//  I2C_SendByte( (u8)0x00 );
//  if(!I2C_WaitAck())
//    return(0);
  I2C_Start();
  I2C_SendByte(LM75_R_ADDRESS);
  if(!I2C_WaitAck())
    return(0);
  t =  I2C_ReceiveByte()<<3;      //take 11-bit MSB
  I2C_Ack();
  t |=  (I2C_ReceiveByte()>>5);
  I2C_NoAck();
  I2C_Stop();
  
  //t=200 => 25 degree
  t=t<<21;            //form a signed 32-bit integer
  t=((t>>21)*5)>>2;   //t=10*degree
  
  *temp=t;
  return(1);
} 

