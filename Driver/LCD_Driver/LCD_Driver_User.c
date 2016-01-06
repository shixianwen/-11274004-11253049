//========================================================================
// 文件名: LCD_Driver_User.c
// 作  者: Xinqiang Zhang(email: Xinqiang@Mzdesign.com.cn)
//			www.Mzdesign.com.cn
// 日  期: 2008/06/11
// 描  述: 底层接口驱动扩展程序集
//
// 参  考: 请参考具体显示器的硬件描述说明书或者是DataSheet,并以此来编写底
//			层接口驱动程序，以及LCD的初始化代码
// 版  本:
//      2007/02/25      First version    Mz Design
//		2008/06/11		V2.0			 Mzdesign
// 注  意: 
//			该驱动程序当中有很多已经被屏蔽掉的子程序，用户可根据自己的应用
//			取消屏障并作一定的修改（如果可能的话）
//			该文件与LCD的特性有关,为MzDesign通用版LCD驱动的V2.0版本,内含的
//			函数是标准的函数,每款LCD对应的通用版LCD驱动程序包都是一样的,只
//			是会针对不同的LCD模块会有一些区别,用户可以参考我们介绍V1.0版本
//			驱动的书<LCD驱动显控原理>.
// 相  关: 本例为针对MzDesign的MzL02 128X64点阵LCD屏作的移植改去,适用于MCS-51
//			系列MCU
//========================================================================
#include "LCD_Config.h"
#include "LCD_PortConfig.h"
//#include <stdio.h>
LCDBYTE LCD_DataRead(void);
void LCD_RegWrite(LCDBYTE Command);
void LCD_DataWrite(LCDBYTE Dat);
void Write_Dot_LCD(LCDBYTE x,LCDBYTE y,LCDBYTE Color);
//LCDBYTE Get_Dot_LCD(WORD x,WORD y);
//void Clear_Dot_LCD(WORD x,WORD y);
void Set_Dot_Addr_LCD(LCDBYTE x,LCDBYTE y);
void LCD_Fill(LCDBYTE a);
//void LCD_Fill_s(WORD Number,LCDBYTE Color);
LCDBYTE LCD_ReadStatus(void);
LCDBYTE LCD_TestStatus(LCDBYTE bitMatch);
void TimeDelay(unsigned int Time);
void LCD_Init(void);
LCDBYTE LCDDATA[128][9];
//========================================================================
// 函数: void LCD_DataWrite(unsigned int Data)
// 描述: 写一个字节的显示数据至LCD中的显示缓冲RAM当中
// 参数: Data 写入的数据 
// 返回: 无
// 备注: Mz 通用版LCD驱动程序 标准子函数
// 版本:
//      2007/01/09      First version
//========================================================================
void LCD_DataWrite(LCDBYTE Dat)
{
    LCD_Data_BUS = Dat;
}
//========================================================================
// 函数: LCDBYTE LCD_DataRead(void)
// 描述: 从LCD中的显示缓冲RAM当中读一个字节的显示数据
// 参数: 无  
// 返回: 读出的数据，
// 备注: Mz 通用版LCD驱动程序 标准子函数 
// 版本:
//      2007/01/09      First version
// 注意: 
//========================================================================
LCDBYTE LCD_DataRead(void)
{
//    LCDBYTE Read_Data;
//    Read_Data = LCD_Data_BUS;
    return LCD_Data_BUS;
}	
//========================================================================
// 函数: void LCD_RegWrite(LCDBYTE Command)
// 描述: 写一个字节的数据至LCD中的控制寄存器当中
// 参数: Command		写入的数据，低八位有效（byte） 
// 返回: 无
// 备注: Mz 通用版LCD驱动程序 标准子函数
// 版本:
//      2007/01/09      First version
//========================================================================
void LCD_RegWrite(LCDBYTE Command)
{
    LCD_CMD_BUS = Command;
}
//========================================================================
// 函数: LCDBYTE LCD_ReadStatus(void)
// 描述: 读取LCD模块的指定状态位
// 参数: 无
// 返回: 读取到的状态位
// 备注: Mz 通用版LCD驱动程序 标准子函数
// 版本:
//      2007/01/09      First version
//========================================================================
LCDBYTE LCD_ReadStatus(void)
{
	return LCD_CMD_BUS;
}
//========================================================================
// 函数: void LCD_TestStatus(LCDBYTE bitMatch)
// 描述: 测试LCD模块的指定状态位
// 参数: bitMatch
// 返回: 如测试有效则返回1  如超时则返回0
// 备注: Mz 通用版LCD驱动程序 标准子函数
// 版本:
//      2007/01/09      First version
//========================================================================
LCDBYTE LCD_TestStatus(LCDBYTE bitMatch)
{
	bitMatch = bitMatch;
	return 1;			//return 1  as test status bit ok
}
//========================================================================
// 函数: void Write_Dot_LCD(int x,int y,unsigned int i)
// 描述: 在LCD的真实坐标系上的X、Y点绘制填充色为i的点
// 参数: x 		X轴坐标
//	 y 		Y轴坐标
//	 i 		要填充的点的颜色 
// 返回: 无
// 备注: Mz 通用版LCD驱动程序 标准子函数
// 版本:
//      2006/10/15      First version
//		2007/01/09		V1.2 
//========================================================================
void Write_Dot_LCD(DOTBYTE x,DOTBYTE y,LCDBYTE i)
{
	DOTBYTE x_low,x_hight;		//定义列地址的高低位指令
	DOTBYTE Dot_Mask_Buf=0x01;
	DOTBYTE y_Page;			//用于存放要画点的位置所在的byte数据位置
        
        if(x>127) x=127;
        if(y>63) y=63;
        
	x = x+1;
	x_low = (x&0x0f);		//定位列地址设置的低位指令
	x_hight = ((x>>4)&0x0f)+0x10;	//定位列地址设置的高位指令
//	y_Page = y&0x07;		//取低三位
        
        Dot_Mask_Buf = 1<<(y&0x07);     //wen
//	switch(y&0x07)
//	{
//		case 0: Dot_Mask_Buf = 0x01;break;
//		case 1: Dot_Mask_Buf = 0x02;break;
//		case 2: Dot_Mask_Buf = 0x04;break;
//		case 3: Dot_Mask_Buf = 0x08;break;
//		case 4: Dot_Mask_Buf = 0x10;break;
//		case 5: Dot_Mask_Buf = 0x20;break;
//		case 6: Dot_Mask_Buf = 0x40;break;
//		case 7: Dot_Mask_Buf = 0x80;break;
//	}
	y_Page = (y>>3)+0xb0;		//Get the page of the byte
	LCD_RegWrite(y_Page);
	LCD_RegWrite(x_low);
	LCD_RegWrite(x_hight);
//	LCD_RegWrite(0xe0);		//设置为读写改模式
//	y_Page = LCD_DataRead();	//
//	y_Page = LCD_DataRead();	//
//      y_Page = LCD_DataRead();
        y_Page = LCDDATA[x][(y>>3)];//+0xb0]; //wen
	if(i) y_Page |= Dot_Mask_Buf;
	else y_Page &= ~Dot_Mask_Buf;
        //LCDDATA[x][(y>>3)+0xb0]=y_Page;
        LCDDATA[x][(y>>3)]=y_Page;            //wen
	LCD_DataWrite(y_Page);	//
        //LCD_DataWrite(Dot_Mask_Buf);	
//	LCD_RegWrite(0xee);		//读写改模式结束
}
/*
//========================================================================
// 函数: void Clear_Dot_LCD(WORD x,WORD y)
// 描述: 清除在LCD的真实坐标系上的X、Y点
// 参数: x 		X轴坐标
//		 y 		Y轴坐标
// 返回: 无
// 备注: 暂不使用该函数
// 版本:
//      2006/10/15      First version
//========================================================================
void Clear_Dot_LCD(WORD x,WORD y)
{
	//可根据自己的需要填写代码
	x = y;						//无意义，仅为了让系统不提示警告
}
//========================================================================
// 函数: unsigned int Get_Dot_LCD(int x,int y)
// 描述: 获取在LCD的真实坐标系上的X、Y点上的当前填充色数据
// 参数: x 		X轴坐标
//		 y 		Y轴坐标
// 返回: 该点的颜色
// 备注: 暂不使用该函数
// 版本:
//      2006/10/15      First version
//========================================================================
LCDBYTE Get_Dot_LCD(DOTBYTE x,DOTBYTE y)
{
	LCDBYTE Read_Data;
	//可根据自己的需要填写代码
	x = y;						//无意义，仅为了让系统不提示警告
	return Read_Data;
}	   */

//========================================================================
// 函数: void Set_Dot_Addr_LCD(int x,int y)
// 描述: 设置在LCD的真实坐标系上的X、Y点对应的RAM地址
// 参数: x 		X轴坐标
//	 y 		Y轴坐标
// 返回: 无
// 备注: 仅设置当前操作地址，为后面的连续操作作好准备
//		 Mz 通用版LCD驱动程序 标准子函数
// 版本:
//      2006/10/15      First version
//	2007/01/09	V1.2 
//========================================================================
//void Set_Dot_Addr_LCD(DOTBYTE x,DOTBYTE y)
//{
//	DOTBYTE x_low,x_hight;			//定义列地址的高低位指令
//	DOTBYTE y_Page;				//用于存放要画点的位置所在的byte数据位置
//	x = x+4;
//	x_low = (x&0x0f);			//定位列地址设置的低位指令
//	x_hight = ((x>>4)&0x0f)+0x10;		//定位列地址设置的高位指令
//	y_Page = (y>>3)+0xb0;			//Get the page of the byte
//	LCD_RegWrite(y_Page);
//	LCD_RegWrite(x_low);
//	LCD_RegWrite(x_hight);
//}
/*
//========================================================================
// 函数: void LCD_Fill_s(WORD Number,LCDBYTE Color)
// 描述: 连续填充以Color色调的Number个点
// 参数: Number 填充的数量    Color  像素点的颜色  
// 返回:
// 备注: 该函数可能会使用到
// 版本:
//      2006/10/15      First version
//		2007/01/09		V1.2 
//========================================================================
void LCD_Fill_s(WORD Number,LCDBYTE Color)
{
	while(Number!=0)
	{
		LCD_DataWrite(Color);						//
		Number--;
	}
} */
//========================================================================
// 函数: void LCD_Fill(unsigned int Data)
// 描述: 会屏填充以Data的数据至各点中
// 参数: Data   要填充的颜色数据
// 返回: 无
// 备注: Mz 通用版LCD驱动程序 标准子函数
// 版本:
//      2006/10/15      First version
//		2007/01/09		V1.2 
//========================================================================
void LCD_Fill(LCDBYTE Data)
{
	DOTBYTE i,j=0;
	DOTBYTE uiTemp;  
        uiTemp = Dis_Y_MAX;
	uiTemp = uiTemp>>3;
	for(i=0;i<=uiTemp;i++)			//往LCD中填充初始化的显示数据
	{
		LCD_RegWrite(0xb0+i);
		LCD_RegWrite(0x00 +1);
		LCD_RegWrite(0x10);
		for(j=0;j<=Dis_X_MAX;j++)
		{
			LCD_DataWrite(Data);
		}
	}
        
	LCD_RegWrite(0xe0);
        //--- wen
        for(int i=0;i<128;i++)
          for(int j=0;j<8;j++)
            LCDDATA[i][j]=Data;
}
//========================================================================
// 函数: void LCD_PortInit(void)
// 描述: 与LCD连接的端口初始化代码
// 参数: 无  
// 返回: 无
// 备注: Mz 通用版LCD驱动程序 标准子函数
// 版本:
//      2007/01/09      First version
// 注意: 
//========================================================================
//void LCD_PortInit(void)
//{
//	LCD_A0 = 1;
//	LCD_EP = 1;
//	LCD_CS = 1;
//	LCD_WR = 1;
//	LCD_RE = 1;
//}
//========================================================================
// 函数: void LCD_Init(void)
// 描述: LCD初始化程序，在里面会完成LCD初始所需要设置的许多寄存器，具体如果
//		 用户想了解，建议查看DataSheet当中各个寄存器的意义
// 参数: 无 
// 返回: 无
// 备注: Mz 通用版LCD驱动程序 标准子函数
// 版本:
//      2006/10/15      First version
//		2007/01/09		V1.2 
//========================================================================
//延时程序

void TimeDelay(unsigned int Time)
{
	unsigned int i;
	while(Time > 0)
	{
		for(i = 0;i < 1600;i++) //1600
		{
			asm("nop");
		}
		Time --;
	}
}
void LCD_Init(void)
{
	//LCD驱动所使用到的端口的初始化（如果有必要的话）
	//LCD_PortInit();
	//LCD_RE = 0;
       
	TimeDelay(200);
	//LCD_RE = 1;
	//TimeDelay(20);
	//设置LCD内部寄存器
        LCD_RegWrite(0xe2);
        TimeDelay(200);
	LCD_RegWrite(0xaf);
        TimeDelay(200);//LCD On
        LCD_RegWrite(0x2f);//设置上电控制模式;	
        TimeDelay(200);//LCD On 

	LCD_RegWrite(0x81);//电量设置模式（显示亮度）
	LCD_RegWrite(0x0f);//亮度数据0x0000~0x003f
	
	LCD_RegWrite(0x27);//V5内部电压调节电阻设置
	LCD_RegWrite(0xa2);//LCD偏压设置，V3时选
	
	LCD_RegWrite(0xc8);//Com 扫描方式设置
	LCD_RegWrite(0xa0);//Segment方向选择
        LCD_RegWrite(0xa5);//全屏变暗指令
	LCD_RegWrite(0xa4);//全屏点亮/变暗指令
	LCD_RegWrite(0xa6);//正向反向显示控制指令
	
	LCD_RegWrite(0xac);//关闭静态指示器
	LCD_RegWrite(0x00);
	
	LCD_RegWrite(0x40+32);//设置显示起始行对应RAM
	LCD_RegWrite(0xe0);//指令数据
	LCD_Fill(LCD_INITIAL_COLOR);
}

