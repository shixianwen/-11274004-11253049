/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : I2C.c
* Author             : Suiris
* Version            : V1.0.0
* Date               : 09/22/2010
* Description        : 用软件控制IO实现I2C主控器的通信时序
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "I2C.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : I2C_init
* Description    : 将PB6,PB7设定为OD输出方式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_IO_init(void) 
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  /*设置 SCL SDA 管脚端口*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
} 
/*******************************************************************************
* Function Name  : I2C_delay
* Description    : 延时函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_delay(void) 
{ 
   u16 i=0xff; //这里可以优化速度 ，经测试最低到5还能写入 
   while(i)  
   {  
     i--;  
   }  
} 
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : 启动I2C总线，即发送I2C初始条件
* Input          : None
* Output         : None
* Return         : =1 发出了起始信号, =0 失败
*******************************************************************************/
int I2C_Start(void)
{
  SDA_H;
  SCL_H;
  I2C_delay();
  if(!SDA_read) return 0;	//SDA线为低电平则总线忙,退出
  SDA_L;
  I2C_delay();
  if(SDA_read) return 0;	//SDA线为高电平则总线出错,退出
  SDA_L; //???   SCL_L
  I2C_delay();
  return 1;
}
/*******************************************************************************
* Function Name  :I2C_Stop
* Description    : 结束I2C总线，即发送I2C结束条件
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
* Description    : 检验I2C总线应答信号，有应答则返回TRUE-1，否则返回FALSE-0
* Input          : None
* Output         : 0（无应答），1（有应答）
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
* Description    : 主控制器应答信号
* Input          : 0或1
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
* Description    : 主控制器非应答信号
* Input          : 0或1
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
* Description    : 字节数据传送函数，将数据 c 发送出去，可以是地址，也可以是数据，
                   发完后等待应答，并对此状态位进行操作
* Input          : u8 型数据
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_SendByte(u8 SendByte) //数据从高位到低位//
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
* Description    : 接收从器件传来的数据，并判断总线错误（不发应答信号），收完后
                   需要调用应答函数。
* Input          : None
* Output         : u8 型数据
* Return         : None
*******************************************************************************/
u8 I2C_ReceiveByte(void)  //数据从高位到低位//
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