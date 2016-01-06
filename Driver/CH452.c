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
#include "CH452.h"
#include "I2C.h"

volatile int gKeyPressed=0;

/* Private functions ------------------------------------------*/
// ???
static void delay_mS(unsigned int t)
{
  int i,j;
  for(i=0;i<t;i++){
    for(j=0;j<50000;j++)
      asm("nop");
  }
}
/*******************************************************************************
* Function Name  : CH452_Init
* Description    : CH452 init for keypad and LED displays.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH452_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 
#endif  
  
  /* CH452 ����İ����ж� INT_KEY => PE4 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  GPIO_EXTILineConfig(GPIO_PinSource4, GPIO_PortSourceGPIOE);
  
  /* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
  EXTI_InitStructure.EXTI_Line = EXTI_Line4 ;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  CH452_Write(CH452_RESET);
  delay_mS(50);
  CH452_Write(CH452_SYSON2);	//�����Ʒ�ʽ�����SDA���������ж��������ô����Ӧ��ΪCH452_SYSON2W(0x04,0x23)
  CH452_Write(CH452_BCD | 4);    // BCD����,4�������
  CH452_Write(CH452_DIG0 | 0); 
  CH452_Write(CH452_DIG1 | 0);
  CH452_Write(CH452_DIG2 | 0);
  CH452_Write(CH452_DIG3 | 0);
  CH452_Write(CH452_DIG4 | 0);
  CH452_Write(CH452_DIG5 | 5);

  /* CH452 ����İ����ж� INT_KEY => PE4 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//EXTI4_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 

}

/*******************************************************************************
* Function Name  :  void CH452_Write(unsigned short cmd)
* Description    : ���մ��������������ݣ����ж����ߴ��󣨲���Ӧ���źţ��������
* Input          : u16 ������
* Output         : None
* Return         : None
*******************************************************************************/
void CH452_Write(unsigned short cmd)	//д����
{
	I2C_Start();               /*��������*/
   	I2C_SendByte((unsigned char)(cmd>>7)&CH452_I2C_MASK|CH452_I2C_ADDR1);  // CH452��ADDR=1ʱ(Ĭ��)
        I2C_NoAck();
   	I2C_SendByte((unsigned char)cmd);     /*��������*/
        I2C_NoAck();
  	I2C_Stop();                 /*��������*/ 
}
/*******************************************************************************
* Function Name  :  unsigned char CH452_Read(void)	
* Description    : ���մ��������������ݣ����ж����ߴ��󣨲���Ӧ���źţ��������
* Input          : None
* Output         : u8 ������
* Return         : None
*******************************************************************************/
unsigned char CH452_Read(void)		//��ȡ����
{
	unsigned char keycode;
   	I2C_Start();                /*��������*/
   	I2C_SendByte((unsigned char)(CH452_GET_KEY>>7)&CH452_I2C_MASK|0x01|CH452_I2C_ADDR1);    // ��������CH452����,��ADDR=0ʱ,���޸�ΪCH452_I2C_ADDR0
   	I2C_NoAck();
        keycode=I2C_ReceiveByte();      /*��ȡ����*/
        I2C_NoAck();
	I2C_Stop();                /*��������*/ 
	return(keycode);
}

/*******************************************************************************
* Function Name  : CH452_KeyCode
* Description    : �����й��ɵİ�����
* Input          : ����CH452��ԭʼ��������
* Output         : 
* Return         : ������ : 0-15; 0xff��Ϊ�Ƿ��İ�������
*******************************************************************************/
/*
name & code       "1" (0x0)     "2" (0x4)     "3" (0x8)   "A" (0xc)
raw data          0x60          0x68          0x70        0x78

                  "4" (0x1)     "5" (0x5)     "6" (0x9)   "B" (0xd)
                  0x61          0x69          0x71        0x79

                  "7" (0x2)     "8" (0x6)     "9" (0xa)   "C" (0xe)
                  0x62          0x6a          0x72        0x7a

                  "*" (0x3)     "0" (0x7)     "#" (0xb)   "D" (0xf)
                  0x63          0x6b          0x73        0x7b
*/
u8 CH452_KeyCode(u8 value)
{
  if((value<0x60)||(value>0x7b))
     return 0xff;
  return (value&0x03)|((value>>1)&0x0c) ;
}
      
/*******************************************************************************
* Function Name  : KeyName
* Description    : ���ɼ�������ascii��
* Input          : ������, 0-15
* Output         : 
* Return         : ascii����,0Ϊ�Ƿ�����
*******************************************************************************/
u8 keyName(u8 code)
{
  const u8 name[16]={'1','4','7','*','2','5','8','0','3','6','9','#','A','B','C','D'};
  if(code>15) 
    return 0;
  return name[code];
}
//����CH452��ȡ�������а�������ʱ���ذ�����ASC��
//û�а�������ʱ����0
u8 GetKey(void)
{
  const u8 name[16]={'1','4','7','*','2','5','8','0','3','6','9','#','A','B','C','D'};
  u8 k=CH452_Read();      //get raw scan code b0-b5
  if(!(k&0x40))           //b6=1 : key pressed
    return 0;
  if((k<0x60)||(k>0x7b))  //check for valid scan code
     return 0;
  k=(k&0x03)|((k>>1)&0x0c); //turn it into 0-15
  return  name[k];        //get key name in ascii
}
//�趨LED�������˸
//����: digmap bit0-bit5��Ӧ���ҵ���6λ����ܣ�Ϊ1�趨��λ��˸��Ϊ0�ر���˸
void LEDDispBlink(u8 digmap)
{
  CH452_Write(CH452_TWINKLE |digmap);
}
//��LED����ܵ�ĳһλ��ʾBCD��
//����: digit : =0-5,��Ӧ���ҵ���6λ������е�һλ
//      code  : Ҫ��ʾ���ֵ�bcd����
void LEDDispBCD(u8 digit, u8 code)
{
  unsigned short n=0x800+(digit<<8);
  
  CH452_Write( n | code);
}
