
/* Includes ------------------------------------------------------------------*/
#include "Eios.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Bank1_SRAM1_ADDR    ((u32)0x60000000)
#define Bank1_SRAM3_ADDR    ((u32)0x68000000)

int gSPI_CONFIGURED=0;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : SPI_Configuration
* Description    : Configures the SPI.(needed for SD and AIC23)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Configuration(void)      
{
  SPI_InitTypeDef   SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  //turn on SPI1  power  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  
  /* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* PG13: gpio out: FSYNC, PG10, PG11 : gpio out: PSEL0, PSEL1 */
  //PG10 = FSMC_NE3 与SRAM总线冲突 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13; //GPIO_Pin_10 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  // PB5 as gpio out : FSEL
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  
  /* SPI1 Config : master-------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 0;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1 */
  SPI_Cmd(SPI1, ENABLE);
  
  gSPI_CONFIGURED=1;
}
/*******************************************************************************
* Function Name  : FSMC_AD7302_WriteBuffer
* Description    : Writes a Half-word buffer to the AD7302. 
* Input          : - pBuffer : pointer to buffer. 
*                  - WriteAddr : AD7302 internal address from which the data 
*                    will be written.
*                  - NumHalfwordToWrite : number of half-words to write. 
*                    
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_AD7302_WriteBuffer(u8* pBuffer, u32 WriteAddr, u32 NumHalfwordToWrite)
{
    u16 i;
  for(; NumHalfwordToWrite != 0; NumHalfwordToWrite--) /* while there is data to write */
  {
    /* Transfer data to the memory */
    *(u8 *) (Bank1_SRAM1_ADDR + WriteAddr) = *pBuffer++;

    for(i=0;i<650;i++) asm("NOP");
  }   
}
/*******************************************************************************
* Function Name  : FSMC_ADS7842_Read
* Description    : Reads data from the ADS7842.
* Input          : - ReadAddr : ADS7842 internal address to read from.
* Output         : - AD data.
* Return         : None
*******************************************************************************/
#define ADS7842_BASE_ADDR 0x60000000
u8 AD_ConvChannel(int ch)
{
  int i=0;
  *(vu8*) (ADS7842_BASE_ADDR + ch)=0x00;
  *(vu8*) (ADS7842_BASE_ADDR + ch)=0x00;
  asm("nop");                         //CS to BUSY low : max 20ns
  asm("nop");  
  asm("nop");  
  asm("nop");  
 
//  for(i=0;i<(700);i++) asm("NOP");
  i=0;
  while(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6)==Bit_RESET)
  {
    asm("nop");  
    asm("nop");  
    if(++i>500) break;
////    *(vu8*) (Bank1_SRAM1_ADDR + ReadAddr)=0x00;
//    for(i=0;i<255;i++) asm("NOP");
  }
  return *((vu8*) (ADS7842_BASE_ADDR));
}
void AD_StartChannel(int ch)
{
  ch=ch&0x03;                         //ch=0-3
  *(vu8*) (ADS7842_BASE_ADDR + ch)=0;  //write channel addr to start converting
  *(vu8*) (ADS7842_BASE_ADDR + ch)=0;  //write channel addr to start converting
  asm("nop");                         //CS to BUSY low : max 20ns
  asm("nop");  
  asm("nop");  
}
int AD_IsBusy(void)
{
  if(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6))   //BUSY is active low
    return 1;
  else 
    return 0;
}
u8 AD_Read(void)
{
  return *((vu8*) (ADS7842_BASE_ADDR));
}

void AD_Init(void)
{
  // PD6 as ADS7842 busy signal
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
}

//===================================================================
//AD7302 2-channel 8-bit DAC

#define AD7302_BASE_ADDR 0x60000008

void DA_Init(void)
{
  // PG14, PG15 set as input, use pulling resistors to pull LDAC low and CLR high
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;       //LDAC pull down: active
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure); 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //CLR pull up : inactive
  GPIO_Init(GPIOG, &GPIO_InitStructure); 

}

void DA_Output(int ch, u8 value)
{
  ch &= 0x01;
  *(vu8*) (AD7302_BASE_ADDR + ch)=value;
}

/*******************************************************************************
* Function Name  : Usb_PinSet
* Description    : Usb ch375 pin configure.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_PinSet(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
  //中断I/O设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 

  /* Connect Key Button EXTI Line to Key Button GPIO Pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);

  /* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
  EXTI_InitStructure.EXTI_Line = EXTI_Line3 ;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  //SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);   //?????
}
/*******************************************************************************
* Function Name  : CH375_IntSet
* Description    : Set CH375 interrupt channel.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_IntSet(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
//#ifdef  VECT_TAB_RAM  
//  /* Set the Vector Table base location at 0x20000000 */ 
//  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
//#else  /* VECT_TAB_FLASH  */
//  /* Set the Vector Table base location at 0x08000000 */ 
//  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 
//#endif  
  /* Set Usb interupt, CH375 => PE3 */ 
  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//EXTI3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
