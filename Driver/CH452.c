/*******************************************************************
* Copyright (c) 2010 Beijing Jiaotong University
* All rights reserved.
*
* File Name	：filename.c
* Description	：This file provides all the functions of filename.c
* Version	：0.1
* Author	：Author
* Date		：MM/DD/YY
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
  
  /* CH452 输出的按键中断 INT_KEY => PE4 */
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
  CH452_Write(CH452_SYSON2);	//两线制方式，如果SDA用做按键中断输出，那么命令应该为CH452_SYSON2W(0x04,0x23)
  CH452_Write(CH452_BCD | 4);    // BCD译码,4个数码管
  CH452_Write(CH452_DIG0 | 0); 
  CH452_Write(CH452_DIG1 | 0);
  CH452_Write(CH452_DIG2 | 0);
  CH452_Write(CH452_DIG3 | 0);
  CH452_Write(CH452_DIG4 | 0);
  CH452_Write(CH452_DIG5 | 5);

  /* CH452 输出的按键中断 INT_KEY => PE4 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//EXTI4_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 

}

/*******************************************************************************
* Function Name  :  void CH452_Write(unsigned short cmd)
* Description    : 接收从器件传来的数据，并判断总线错误（不发应答信号），收完后
* Input          : u16 型数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH452_Write(unsigned short cmd)	//写命令
{
	I2C_Start();               /*启动总线*/
   	I2C_SendByte((unsigned char)(cmd>>7)&CH452_I2C_MASK|CH452_I2C_ADDR1);  // CH452的ADDR=1时(默认)
        I2C_NoAck();
   	I2C_SendByte((unsigned char)cmd);     /*发送数据*/
        I2C_NoAck();
  	I2C_Stop();                 /*结束总线*/ 
}
/*******************************************************************************
* Function Name  :  unsigned char CH452_Read(void)	
* Description    : 接收从器件传来的数据，并判断总线错误（不发应答信号），收完后
* Input          : None
* Output         : u8 型数据
* Return         : None
*******************************************************************************/
unsigned char CH452_Read(void)		//读取按键
{
	unsigned char keycode;
   	I2C_Start();                /*启动总线*/
   	I2C_SendByte((unsigned char)(CH452_GET_KEY>>7)&CH452_I2C_MASK|0x01|CH452_I2C_ADDR1);    // 若有两个CH452并连,当ADDR=0时,需修改为CH452_I2C_ADDR0
   	I2C_NoAck();
        keycode=I2C_ReceiveByte();      /*读取数据*/
        I2C_NoAck();
	I2C_Stop();                /*结束总线*/ 
	return(keycode);
}

/*******************************************************************************
* Function Name  : CH452_KeyCode
* Description    : 生成有规律的按键码
* Input          : 来自CH452的原始按键数据
* Output         : 
* Return         : 按键码 : 0-15; 0xff作为非法的按键数据
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
* Description    : 生成键盘名的ascii码
* Input          : 按键码, 0-15
* Output         : 
* Return         : ascii键名,0为非法按键
*******************************************************************************/
u8 keyName(u8 code)
{
  const u8 name[16]={'1','4','7','*','2','5','8','0','3','6','9','#','A','B','C','D'};
  if(code>15) 
    return 0;
  return name[code];
}
//经过CH452读取按键，有按键按下时返回按键名ASC码
//没有按键按下时返回0
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
//设定LED数码管闪烁
//输入: digmap bit0-bit5对应从右到左6位数码管，为1设定该位闪烁，为0关闭闪烁
void LEDDispBlink(u8 digmap)
{
  CH452_Write(CH452_TWINKLE |digmap);
}
//在LED数码管的某一位显示BCD码
//输入: digit : =0-5,对应从右到左6位数码管中的一位
//      code  : 要显示数字的bcd编码
void LEDDispBCD(u8 digit, u8 code)
{
  unsigned short n=0x800+(digit<<8);
  
  CH452_Write( n | code);
}
