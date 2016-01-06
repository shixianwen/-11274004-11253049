
/* Includes ------------------------------------------------------------------*/
#include "SetSystem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Bank1_SRAM1_ADDR    ((u32)0x60000000)
#define Bank1_SRAM3_ADDR    ((u32)0x68000000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{   
  /* RCC system reset(for debug purpose) */
//  RCC_DeInit();
//
//  /* Enable HSE */
//  RCC_HSEConfig(RCC_HSE_ON);
//
//  /* Wait till HSE is ready */
//  HSEStartUpStatus = RCC_WaitForHSEStartUp();
//
//  if(HSEStartUpStatus == SUCCESS)
//  {
//    /* Enable Prefetch Buffer */
//    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
//
//    /* Flash 2 wait state */
//    FLASH_SetLatency(FLASH_Latency_2);
// 	
//    /* HCLK = SYSCLK */
//    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
//  
//    /* PCLK2 = HCLK */
//    RCC_PCLK2Config(RCC_HCLK_Div1); 
//
//    /* PCLK1 = HCLK/2 */
//    RCC_PCLK1Config(RCC_HCLK_Div2);
//
//    /* PLLCLK = 8MHz * 9 = 72 MHz */
//    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
//
//    /* Enable PLL */ 
//    RCC_PLLCmd(ENABLE);
//
//    /* Wait till PLL is ready */
//    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
//    {
//    }
//
//    /* Select PLL as system clock source */
//    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
//
//    /* Wait till PLL is used as system clock source */
//    while(RCC_GetSYSCLKSource() != 0x08)
//    {
//    }
//  }
  
  /* Enable the FSMC Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
                         RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 
#endif  
  /* Set Usb interupt, CH375 => PE3 */ 
  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//EXTI3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
  
  /* CH452 输出的按键中断 INT_KEY => PE4 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//EXTI4_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
  

}
/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : None
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;   
  USART_InitTypeDef USART_InitStructure;

/*-- GPIO Configuration ------------------------------------------------------*/
  /* IO pin for USART1 */
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_2 | GPIO_Pin_3 ;
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  /* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Configure the USARTx */ 
  USART_Init(USART2, &USART_InitStructure);
  USART_Init(USART1, &USART_InitStructure);
  /* Enable the USARTx */
  USART_Cmd(USART2, ENABLE);
  USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
* Function Name  : FSMC_SRAM_Init
* Description    : Configures the FSMC and GPIOs to interface with the SRAM memory.
*                  This function must be called before any write/read operation
*                  on the SRAM.
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef GPIO_InitStructure; 

/*-- GPIO Configuration ------------------------------------------------------*/
  /* SRAM Data lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 ;
  GPIO_InitStructure.GPIO_Pin |= (GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 ;
  GPIO_InitStructure.GPIO_Pin |= (GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /* SRAM Address lines configuration */
  //A[0..5] = PF[0..5], A[6..9] = PF[12..15]
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; 
  GPIO_InitStructure.GPIO_Pin |= (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  //A[10..15] = PG[0..5]
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  //A[16.17] =PD[11.12]
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 |GPIO_Pin_12;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NOE and NWE configuration */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NE1 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NE2 = PG9, NE3=PG10, NE4=PG12 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);  
  
  /* NBL0, NBL1 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
  GPIO_Init(GPIOE, &GPIO_InitStructure); 

//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
//  GPIO_Init(GPIOG, &GPIO_InitStructure);  
  
/*-- FSMC_LCD Configuration ------------------------------------------------------*/
  p.FSMC_AddressSetupTime = 20;
  p.FSMC_AddressHoldTime = 10;
  p.FSMC_DataSetupTime = 30;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_B;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
  
/*-- FSMC_USB Configuration ------------------------------------------------------*/
  p.FSMC_AddressSetupTime = 0xD;
  p.FSMC_AddressHoldTime = 0x0;
  p.FSMC_DataSetupTime = 0x7F;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
  
/*-- FSMC_SRAM Configuration ------------------------------------------------------*/
//  p.FSMC_AddressSetupTime = 20;
//  p.FSMC_AddressHoldTime = 10;
//  p.FSMC_DataSetupTime = 30;
//  p.FSMC_BusTurnAroundDuration = 0;
//  p.FSMC_CLKDivision = 0;
//  p.FSMC_DataLatency = 0;
//  p.FSMC_AccessMode = FSMC_AccessMode_B;

  p.FSMC_AddressSetupTime = 10;//1;//0;
  p.FSMC_AddressHoldTime = 10;//1;//0;
  p.FSMC_DataSetupTime = 30;//1;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 1;//1;//0;
  p.FSMC_AccessMode = FSMC_AccessMode_B;

  
  
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  
  
}


void SRAM_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF, ENABLE);
  
/*-- GPIO Configuration ------------------------------------------------------*/
  /* SRAM Data lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /* SRAM Address lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | 
                                GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
   
  /* NOE and NWE configuration */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NE3 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* NBL0, NBL1 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
/*-- FSMC Configuration ------------------------------------------------------*/
  p.FSMC_AddressSetupTime = 1;
  p.FSMC_AddressHoldTime = 1;
  p.FSMC_DataSetupTime = 40;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  
}

/*******************************************************************************
* Function Name  : LED_Configuration
* Description    : Configures the LED.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LED_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

  /* Configure PF6-PF11 as output : drive led light--------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 
                                | GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  //PB8 as out, =PWM, drive BUZZER
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}
/*******************************************************************************
* Function Name  : SPI_Configuration
* Description    : Configures the SPI.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Configuration(void)
{
  SPI_InitTypeDef   SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  /* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure  pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* 1st phase: SPI1 Master and SPI2 Slave */
  /* SPI1 Config -------------------------------------------------------------*/
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
u8 FSMC_ADS7842_Read(u8 ReadAddr)
{
  u8 i;
  *(vu8*) (Bank1_SRAM1_ADDR + ReadAddr)=0x00;
  for(i=0;i<255;i++) asm("NOP");
  while(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6)==Bit_RESET)
  {
    *(vu8*) (Bank1_SRAM1_ADDR + ReadAddr)=0x00;
    for(i=0;i<255;i++) asm("NOP");
  }
  return *((vu8*) (Bank1_SRAM1_ADDR));
}
/*******************************************************************************
* Function name : Fill_Buffer
* Description   : Fill the global buffer
* Input         : - pBuffer: pointer on the Buffer to fill
*                 - BufferSize: size of the buffer to fill
*                 - Offset: first value to fill on the Buffer
* Output param  : None
*******************************************************************************/
void Fill_Buffer(u8 *pBuffer, u16 BufferLenght, u32 Offset)
{
  u8 IndexTmp = 0;

  /* Put in global buffer same values */
  for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
  {
    pBuffer[IndexTmp] = IndexTmp*Offset;
  }
}

//=============================================================================
//设定发光管显示
//输入参数
//    num = 0 - 5 ，发光管编号，最右面为0
//    on  = 0 : 关闭
//        = 1 : 点亮
//        = 2 : 反转
void LEDLight(int num, int on)
{
  int n;
  
  if(num>5) return;
  
  n=1<<(num+6);
  
  if(!on) GPIOF->BRR = n;
  else if(on&0x01) 
    GPIOF->BSRR = n;
  else
    GPIOF->ODR ^= n; 
}

//=============================================================================
//蜂鸣器发声
//输入参数
//    on = 1 : 发声
//       = 0 : 关闭
void Beep(int on)
{
  if(on) 
    GPIOB->BSRR = GPIO_Pin_8;   //H 
  else
    GPIOB->BRR = GPIO_Pin_8;    //L
}




/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
