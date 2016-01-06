/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "I2C.h"

#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void LED_Init(void);

/* iar called function */
//
//int __low_level_init(void)
//{
//  SystemInit();
//  return 1;
//}
/* Private functions ---------------------------------------------------------*/


/* RCC configuration for basic sys and io */
void RCC_Reconfig(void)
{ 
  /* system clock may be set in _low_level_init() */
  
  ErrorStatus HSEStartUpStatus;
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 	
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  
  /* Enable the FSMC Clock */
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);//|RCC_AHBPeriph_FLITF|RCC_AHBPeriph_SRAM, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC|RCC_AHBPeriph_FLITF|RCC_AHBPeriph_SRAM, ENABLE);
  /* 打开所有端口电源 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
                         RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);
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


void FSMC_Init0(void)
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
//
////  p.FSMC_AddressSetupTime = 0;
////  p.FSMC_AddressHoldTime = 0;
////  p.FSMC_DataSetupTime = 4;
////  p.FSMC_BusTurnAroundDuration = 0;
////  p.FSMC_CLKDivision = 0;
////  p.FSMC_DataLatency = 0;
////  p.FSMC_AccessMode = FSMC_AccessMode_A;
//
//  
//  
//  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
//  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
//  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
//  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
//  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
//  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
//  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
//  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
//  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
//  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
//  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
//  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
//  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
//  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

//  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* Enable FSMC Bank1_SRAM Bank */
//  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  
  
  p.FSMC_AddressSetupTime = 0;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 1;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;  
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

  /*!< Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE); 
}

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
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 |GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NOE and NWE configuration */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NE1 = PD7  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NE2 = PG9,  NE3=PG10,  NE4=PG12     */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);  
  
  /* NBL0, NBL1 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  

  /*---------   NORSRAM timing parameters  -------------------------------- */ 

  /* NORSRAM Bank1 : LCD & IOs  --------------------------------------------*/
  p.FSMC_AddressSetupTime = 3;  //1;  //0;
  p.FSMC_AddressHoldTime = 2;   //0;
  p.FSMC_DataSetupTime = 7;     //4;  //7;
  p.FSMC_BusTurnAroundDuration = 1; //0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 2;       //0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;  
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

  /*!< Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
  

  /* NORSRAM Bank4 : FSMC_USB CH375 ----------------------------------------*/
  //need long waiting ...
  p.FSMC_AddressSetupTime = 10;//0xD;
  p.FSMC_AddressHoldTime = 0;//0x0;
  p.FSMC_DataSetupTime = 90;//0x7F;
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

  /* Enable FSMC Bank4_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
  
  /*NORSRAM Bank3 : SRAM 25616 ---------------------------------------------*/
  //0,0,1 is OK.
  p.FSMC_AddressSetupTime = 0;  //0;
  p.FSMC_AddressHoldTime = 0; //0;
  p.FSMC_DataSetupTime = 1;   //1;
  p.FSMC_BusTurnAroundDuration = 0; //0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A; //A ;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;  
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

  /*!< Enable FSMC Bank3_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);   
  
  /*NORSRAM Bank2 : DM9000  ----------------------------------------------- */
  p.FSMC_AddressSetupTime = 0;    //0
  p.FSMC_AddressHoldTime = 0;     //0
  p.FSMC_DataSetupTime = 3;       //3
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM2;
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

  /* - BANK 2 (of NOR/SRAM Bank 0~3) is enabled */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);  
}



int System_Init(void)
{
  
  RCC_Reconfig();   /* 配置系统时钟, GPIO时钟, FSMC时钟 */
  
  FSMC_Init();      /* 总线初始化 */
  
  LED_Init();       /* 发光管, buzzer引脚初始化 */
  
  UART_Init();      /* 串口1，2初始化， 115200， 8，N,1 */
  
  I2C_IO_init();    /* 软件I2C协议和引脚初始化 */
  
  CH452_Init();     /* 数码管和矩阵键盘驱动芯片CH452初始化 */

#ifndef NO_LCD
  LCD_Init();       /* 外接单色液晶模块 */
#endif
  return 1;
}


/*******************************************************************************
* Function Name  : LED_Init
* Description    : Configures the LED and buzzer pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LED_Init(void)
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
//=============================================================================
//设定1位发光管显示
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
//设定6位发光管显示
//输入参数
//    mask  : 修改掩码，b0-b5 对应从右到左6个发光管, =1要修改，=0不变
//    on  = 0 : 关闭
//        = 1 : 点亮
//        = 2 : 反转
void LEDLights(int mask, int on)
{
  int n;
  
  mask&=0x3f;
  if(!mask) return;
  mask<<=6;
  
  if(!on) 
    GPIOF->BRR = mask;  // 熄灭
  else if(on&0x01)    
    GPIOF->BSRR = mask; // 点亮
  else
    GPIOF->ODR ^= mask; // 反转
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

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : None
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;   
  USART_InitTypeDef USART_InitStructure;

  /* 打开串口电源 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* IO pin for USART1 & USART2 */
//  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_2 | GPIO_Pin_3 ;
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9| GPIO_Pin_2  ;
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
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
* Function Name  : int fputc(int ch, FILE *f)
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define USE_UART_PRINTF
#ifdef USE_UART_PRINTF
int fputc(int ch, FILE *f)
{
  /* Write a character to the USART */
  USART_SendData(USART1, (u8) ch);
//  USART_SendData(USART2, (u8) ch);

  /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {
  }
//  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
//  {
//  }

  return ch;
}
#endif



