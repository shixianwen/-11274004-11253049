#ifndef __LCD_CONFIG_H__
#define __LCD_CONFIG_H__

typedef	unsigned char LCDBYTE;			//重定义有关LCD的,也即LCD当中的数据位宽
typedef unsigned char DOTBYTE;			//重定义有关LCD的,也即LCD的点阵数量
typedef signed short  DISWORD;			//重定义在LCD_Dis.c文件中一些中间处理的数据的宽度

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   BOOL;

#define LCD_X_MAX			128-1			//屏幕的X轴的物理宽度
#define LCD_Y_MAX			64-1			//屏幕的Y轴的物理宽度
			
#define LCD_XY_Switch		0				//显示时X轴和Y由交换
#define LCD_X_Rev			0				//显示时X轴反转
#define LCD_Y_Rev			0				//显示时Y轴反转

#if LCD_XY_Switch == 0
	#define Dis_X_MAX		LCD_X_MAX
	#define Dis_Y_MAX		LCD_Y_MAX	
#endif

#if LCD_XY_Switch == 1
	#define Dis_X_MAX		LCD_Y_MAX
	#define Dis_Y_MAX		LCD_X_MAX	
#endif

#define LCD_INITIAL_COLOR	0x00			//定义LCD屏初始化时的背景色

#define LCD_DIS_CIRCLE		1				//定义是否需要绘制圆形的功能

//========================================================================
// 函数: void Pos_Switch(unsigned int * x,unsigned int * y)
// 描述: 将画面的坐标变换为实际LCD的坐标体系，以便于快速画圆形以及矩形
// 参数: x  X轴坐标     y  Y轴坐标      
// 返回: 无 
// 备注: 这里以及之前的所有x和y坐标系都是用户层的，并不是实际LCD的坐标体系
//		 本函数提供可进行坐标变换的接口
// 版本:
//      2006/10/15      First version
//========================================================================
//========================================================================
// 函数: void Writ_Dot(int x,int y,unsigned int Color)
// 描述: 填充以x,y为坐标的象素
// 参数: x  X轴坐标     y  Y轴坐标      Color  像素颜色 
// 返回: 无
// 备注: 这里以及之前的所有x和y坐标系都是用户层的，并不是实际LCD的坐标体系
//		 本函数提供可进行坐标变换的接口
// 版本:
//      2006/10/15      First version
//========================================================================
//========================================================================
// 函数: unsigned int Get_Dot(int x,int y)
// 描述: 获取x,y为坐标的象素
// 参数: x  X轴坐标     y  Y轴坐标      
// 返回: Color  像素颜色 
// 备注: 这里以及之前的所有x和y坐标系都是用户层的，并不是实际LCD的坐标体系
//		 本函数提供可进行坐标变换的接口
// 版本:
//      2006/10/15      First version
//========================================================================
//========================================================================
// 函数: void Set_Dot_Addr(int x,int y)
// 描述: 设置当前需要操作的象素地址
// 参数: x  X轴坐标     y  Y轴坐标      
// 返回: 无 
// 备注: 这里以及之前的所有x和y坐标系都是用户层的，并不是实际LCD的坐标体系
//		 本函数提供可进行坐标变换的接口
// 版本:
//      2006/10/15      First version
//========================================================================
#if LCD_XY_Switch==0&&LCD_X_Rev==0&&LCD_Y_Rev==0
	#define Pos_Switch(x,y)	
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(x,y,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(x,y)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(x,y)	
#endif
#if LCD_XY_Switch==0&&LCD_X_Rev==1&&LCD_Y_Rev==0
	#define Pos_Switch(x,y)			x = Dis_X_MAX-x
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(Dis_X_MAX-x,y,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(Dis_X_MAX-x,y)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(Dis_X_MAX-x,y)		
#endif
#if LCD_XY_Switch==0&&LCD_X_Rev==0&&LCD_Y_Rev==1
	#define Pos_Switch(x,y)			y = Dis_Y_MAX-y	
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(x,Dis_Y_MAX-y,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(x,Dis_Y_MAX-y)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(x,Dis_Y_MAX-y)		
#endif
#if LCD_XY_Switch==0&&LCD_X_Rev==1&&LCD_Y_Rev==1
	#define Pos_Switch(x,y)			x = Dis_X_MAX-x;\
									y = Dos_Y_MAX-y	
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(Dis_X_MAX-x,Dis_Y_MAX-y,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(Dis_X_MAX-x,Dis_Y_MAX-y)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(Dis_X_MAX-x,Dis_Y_MAX-y)	
#endif

#if LCD_XY_Switch==1&&LCD_X_Rev==0&&LCD_Y_Rev==0
	#define Pos_Switch(x,y)			x = x+y;\
									y = x-y;\
									x = x-y;
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(y,x,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(y,x)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(y,x)		
#endif
#if LCD_XY_Switch==1&&LCD_X_Rev==1&&LCD_Y_Rev==0
	#define Pos_Switch(x,y)			x = Dis_X_MAX-x;\
									x = x+y;\
									y = x-y;\
									x = x-y;
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(y,Dis_X_MAX-x,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(y,Dis_X_MAX-x)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(y,Dis_X_MAX-x)		
#endif
#if LCD_XY_Switch==1&&LCD_X_Rev==0&&LCD_Y_Rev==1
	#define Pos_Switch(x,y)			y = Dis_Y_MAX-y;\
									x = x+y;\
									y = x-y;\
									x = x-y;
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(Dis_Y_MAX-y,x,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(Dis_Y_MAX-y,x)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(Dis_Y_MAX-y,x)		
#endif
#if LCD_XY_Switch==1&&LCD_X_Rev==1&&LCD_Y_Rev==1
	#define Pos_Switch(x,y)			x = Dis_X_MAX-x;\
									y = Dis_Y_MAX-y;\
									x = x+y;\
									y = x-y;\
									x = x-y;
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(Dis_Y_MAX-y,Dis_X_MAX-x,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(Dis_Y_MAX-y,Dis_X_MAX-x)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(Dis_Y_MAX-y,Dis_X_MAX-x)		
#endif
//
#define Match_xy(s,e)	if(s>e){s = s+e;e = s-e;s = s-e;}

#endif
