/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : I2C.c
* Author             : Suiris
* Version            : V1.0.0
* Date               : 09/22/2010
* Description        : ���������IOʵ��I2C��������ͨ��ʱ��
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "I2C.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : I2C_init
* Description    : ��PB6,PB7�趨ΪOD�����ʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_IO_init(void) 
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  /*���� SCL SDA �ܽŶ˿�*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
} 
/*******************************************************************************
* Function Name  : I2C_delay
* Description    : ��ʱ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_delay(void) 
{ 
   u16 i=0xff; //��������Ż��ٶ� ����������͵�5����д�� 
   while(i)  
   {  
     i--;  
   }  
} 
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : ����I2C���ߣ�������I2C��ʼ����
* Input          : None
* Output         : None
* Return         : =1 ��������ʼ�ź�, =0 ʧ��
*******************************************************************************/
int I2C_Start(void)
{
  SDA_H;
  SCL_H;
  I2C_delay();
  if(!SDA_read) return 0;	//SDA��Ϊ�͵�ƽ������æ,�˳�
  SDA_L;
  I2C_delay();
  if(SDA_read) return 0;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
  SDA_L; //???   SCL_L
  I2C_delay();
  return 1;
}
/*******************************************************************************
* Function Name  :I2C_Stop
* Description    : ����I2C���ߣ�������I2C��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_Stop(void)
{
  SCL_L;
  I2C_delay();
  SDA_L;
  I2C_delay();
  SCL_H;
  I2C_delay();
  SDA_H;
  I2C_delay();
}
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : ����I2C����Ӧ���źţ���Ӧ���򷵻�TRUE-1�����򷵻�FALSE-0
* Input          : None
* Output         : 0����Ӧ�𣩣�1����Ӧ��
* Return         : None
*******************************************************************************/
int I2C_WaitAck(void) 
{ 
  SCL_L;
  I2C_delay();
  SDA_H;			
  I2C_delay();
  SCL_H;
  I2C_delay();
  if(SDA_read) 	{
    SCL_L;
    return 0;
  }
  SCL_L;
  return 1;
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : ��������Ӧ���ź�
* Input          : 0��1
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_Ack(void)
{	
  SCL_L;
  I2C_delay();
  SDA_L;
  I2C_delay();
  SCL_H;
  I2C_delay();
  SCL_L;
  I2C_delay();
}
/*******************************************************************************
* Function Name  : I2C_Ackn(u8 a)
* Description    : ����������Ӧ���ź�
* Input          : 0��1
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_NoAck(void)
{	
  SCL_L;
  I2C_delay();
  SDA_H;
  I2C_delay();
  SCL_H;
  I2C_delay();
  SCL_L;
  I2C_delay();
}
/*******************************************************************************
* Function Name  :I2C_SendByte(u8 SendByte)
* Description    : �ֽ����ݴ��ͺ����������� c ���ͳ�ȥ�������ǵ�ַ��Ҳ���������ݣ�
                   �����ȴ�Ӧ�𣬲��Դ�״̬λ���в���
* Input          : u8 ������
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
  u8 i=8;
  while(i--) {
    SCL_L;
    I2C_delay();
    if(SendByte&0x80) SDA_H;  
    else  SDA_L;   
    SendByte<<=1;
    I2C_delay();
    SCL_H;
    I2C_delay();
  }
  SCL_L;
}
/*******************************************************************************
* Function Name  :u8 I2C_ReceiveByte(void)
* Description    : ���մ��������������ݣ����ж����ߴ��󣨲���Ӧ���źţ��������
                   ��Ҫ����Ӧ������
* Input          : None
* Output         : u8 ������
* Return         : None
*******************************************************************************/
u8 I2C_ReceiveByte(void)  //���ݴӸ�λ����λ//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--) {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
      SCL_H;
      I2C_delay();	
      if(SDA_read)
        ReceiveByte|=0x01;
    }
    SCL_L;
    return ReceiveByte;
}