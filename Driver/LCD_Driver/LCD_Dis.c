//========================================================================
// 文件名: LCD_Dis.c
// 作  者: Xinqiang Zhang(email: Xinqiang@Mzdesign.com.cn)
//			www.Mzdesign.com.cn
// 日  期: 2007/02/24
// 描  述: 通用版LCD用户接口层程序集
//
// 参  考: 请参考具体显示器的硬件描述说明书或者是DataSheet,并以此来编写加速
//			版接口程序;适用于MCS51系列MCU
// 版  本:
//      2006/10/18      First version    Mz Design
//		2007/01/13		V1.1  去掉坐标体系变换的功能，对速度进行一定的优化
//								并使该文件的代码更加通用于各种LCD
//		2007/02/24		V1.2  为小资源的51单片机进行驱动精简，减少对RAM的占
//								用
//		2008/06/12		V2.0  裁减掉原有备用的坐标软变换的功能,对数据类型
//								进行统一的重定义(在LCD_Config.h当中),修改
//								PutChar函数
//								增加文本显示的覆盖显示功能
//========================================================================
#include "LCD_Driver_User.h"		//LCD底层驱动声明头文件
#include "LCD_Config.h"			//LCD的配置文件，比如坐标轴是否倒置等的定义


//extern unsigned int Asii8[];				//6X8的ASII字符库
extern const unsigned char Asii0610[];			//6X10的ASII字符库
extern const unsigned char Asii0816[];			//8X16的ASII字符库

DOTBYTE X_Witch=5;					//字符写入时的宽度
DOTBYTE Y_Witch=10;					//字符写入时的高度
DOTBYTE Font_Wrod=10;					//字体的每个字模占用多少个存储单元数
const unsigned char *Char_TAB= Asii0610;		//字库指针
//unsigned char Plot_Mode;				//绘图模式
LCDBYTE BMP_Color=0xff;
LCDBYTE Char_Color=1;

BYTE	Font_type=1;					//标识字符类型
BYTE	Font_Cover=0;					//字符显示的模式
LCDBYTE	Char_BackColor=0;				//设置覆盖模式时,字符的背景色 


//画直线函数，s_x、s_y为起始点，e_x、e_y为结束点
void Line(LCDBYTE s_x,LCDBYTE s_y,LCDBYTE e_x,LCDBYTE e_y);
//绘图模式、当前绘图颜色设置
void SetPaintMode(BYTE Mode,LCDBYTE Color);
//标准字符设置，包括两型号ASCII码的大小尺寸
void LCD_FontSet(DOTBYTE Font_NUM);
//字体颜色的设置,返回以前的颜色
LCDBYTE LCD_FontColor(LCDBYTE Color);
//设置字符显示的模式,并设置背景色
//void FontMode(BYTE Mode,LCDBYTE FontBackColor);
//于x、y的坐标上写入一个标准字符
void PutChar(DOTBYTE x,DOTBYTE y,char a);
//于x、y的坐标为起始写入一串标准字符串
void PutString(DOTBYTE x,DOTBYTE y,char *p);
//于x、y的坐标为中心，绘制一个点
void PutPixel(DOTBYTE x,DOTBYTE y);
//于x、y的坐标为中心，绘制一个圆边或实心圆
void Circle(DOTBYTE x,DOTBYTE y,DOTBYTE r,BYTE mode);
//绘制一个以left、top和right、bottom为两个对角的矩形框或者实心矩形
void Rectangle(DOTBYTE left, DOTBYTE top, DOTBYTE right, DOTBYTE bottom, BYTE Mode);
//清屏函数，执行全屏幕清除或填充前景色
void ClrScreen(BYTE Mode);
//========================================================================
// 函数: void FontSet(int Font_NUM)
// 描述: 文本字体设置
// 参数: Font_NUM 字体选择,以驱动所带的字库为准
//		 Color  文本颜色,仅作用于自带字库  
// 返回: 无
// 备注: 
// 版本:
//      2006/10/15      First version
//========================================================================
void LCD_FontSet(DOTBYTE Font_NUM)
{
    switch(Font_NUM)
    {
        case 1: 
            Font_Wrod = 16;	//ASII字符A
            X_Witch = 8;
            Y_Witch = 16;
            Font_type = 1;
            Char_TAB = Asii0816;
            break;
        case 0: 
            Font_Wrod = 10;	//ASII字符B
            X_Witch = 5;
            Y_Witch = 10;
            Font_type = 1;
            Char_TAB = Asii0610;
            break;	
    }
}
//========================================================================
// 函数: void LCD_FontColor(unsigned char color)
// 描述: 文本字体设置
// 参数: Font_NUM 字体选择,以驱动所带的字库为准
//		 Color  文本颜色,仅作用于自带字库  
// 返回: 原来的颜色
//========================================================================

LCDBYTE LCD_FontColor(LCDBYTE Color)
{
    LCDBYTE c=Char_Color;
    Char_Color = Color;
    Char_BackColor = (Color==0)?1:0;
    return c;
}
//========================================================================
// 函数: void FontBackFill(int fill) 
// 设置背景覆盖模式
// fill =1 : 覆盖模式，用字体背景色填充
// fill =0 : 叠加模式，不填充，保留原有内容
// 备注: 仅对字符显示有效
void LCD_FontBackFill(int fill)
{
  Font_Cover = fill;
}
//========================================================================
// 函数: void PutChar(unsigned char x,unsigned char y,char a)  
// 描述: 写入一个标准字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 a  要显示的字符在字库中的偏移量  
// 返回: 无
// 备注: ASCII字符可直接输入ASCII码即可
// 版本:
//      2006/10/15      First version
//		2007/01/11		V1.1
//		2008/06/12		V2.0
//========================================================================
void PutChar(DOTBYTE x,DOTBYTE y,char a)       
{
	DOTBYTE i,j;		//数据暂存
	const unsigned char *p_data;
	LCDBYTE Temp;
	BYTE Index = 0;
	if(Font_type==1)
		p_data = Char_TAB + (a-32)*Font_Wrod;
	else
		p_data = Char_TAB + a*Font_Wrod;	//要写字符的首地址
	j = 0;
	while((j ++) < Y_Witch)
	{
		if(y > Dis_Y_MAX) break;
		i = 0;
		while(i < X_Witch)
		{
			if((i&0x07)==0)
			{
//				Temp = *(p_data + (Index>>1));
//				if((Index&0x01)==0)Temp = Temp>>8; 
				Temp = *(p_data+Index);
				Index++;
			}
			if((Temp & 0x80) > 0) Writ_Dot/*Write_Dot_LCD*/((x+i),y,Char_Color);
			else if(Font_Cover) Writ_Dot/*Write_Dot_LCD*/((x+i),y,Char_BackColor);
			Temp = Temp << 1;
			if((x+i) >= Dis_X_MAX) 
			{
				Index += (X_Witch-i)>>3;
				break;
			}
			i++;
		}
		y ++;
	}
}

//========================================================================
// 函数: void PutString(unsigned char x,unsigned char y,char *p)
// 描述: 在x、y为起始坐标处写入一串标准字符
// 参数: x  X轴坐标     y  Y轴坐标
//	 p  要显示的字符串  
// 返回: 无
// 备注: 仅能用于自带的ASCII字符串显示
// 版本:
//      2006/10/15      First version
//========================================================================
void PutString(DOTBYTE x,DOTBYTE y,char *p)
{
	while(*p!=0)
	{
		PutChar(x,y,*p);
		x += X_Witch;
		if((x + X_Witch) > Dis_X_MAX)
		{
			x = 0;
			if((Dis_Y_MAX - y) < Y_Witch) break;
			else y += Y_Witch;
		}
		p++;
	}
}
//========================================================================
// 函数: void SetPaintMode(int Mode,unsigned int Color)
// 描述: 绘图模式设置
// 参数: Mode 绘图模式    Color  像素点的颜色,相当于前景色  
// 返回: 无
// 备注: Mode无效
// 版本:
//      2006/10/15      First version
//========================================================================
void SetPaintMode(BYTE Mode,LCDBYTE Color)
{
	Mode = Mode;//Plot_Mode = Mode;		//仅仅是为了保持与其它驱动的一至性，绘图模式在该版驱动中未用
        //BMP_Color = Color;
        //wen
        if(Color) 
          BMP_Color =0xff;
        else
          BMP_Color=0;
}
//========================================================================
// 函数: void PutPixel(int x,int y)
// 描述: 在x、y点上绘制一个前景色的点
// 参数: x  X轴坐标     y  Y轴坐标
// 返回: 无
// 备注: 使用前景色
// 版本:
//      2006/10/15      First version
//========================================================================
void PutPixel(DOTBYTE x,DOTBYTE y)
{
	Writ_Dot/*Write_Dot_LCD*/(x,y,BMP_Color);
}
//========================================================================
// 函数: void Line(unsigned char s_x,unsigned char s_y,unsigned char e_x,unsigned char e_y)
// 描述: 在s_x、s_y为起始坐标，e_x、e_y为结束坐标绘制一条直线
// 参数: x  X轴坐标     y  Y轴坐标
// 返回: 无
// 备注: 使用前景色
// 版本:
//      2006/10/15      First version
//========================================================================
void Line(LCDBYTE s_x,LCDBYTE s_y,LCDBYTE e_x,LCDBYTE e_y)
{  
	signed short Offset_x,Offset_y,Offset_k = 0;
	signed short Err_d = 1;
	if(s_y>e_y)
	{
		Offset_x = s_x;
		s_x = e_x;
		e_x = Offset_x;
		Offset_x = s_y;
		s_y = e_y;
		e_y = Offset_x;
	} 
	Offset_x = e_x-s_x;
	Offset_y = e_y-s_y;
	Writ_Dot(s_x,s_y,BMP_Color);
    if(Offset_x<0)
	{
		Offset_x = s_x-e_x;
       // Err_d = s_x;
       // s_x = e_x;
       // e_x = Err_d;
		Err_d = -1;
	}
    if(Offset_x==0)
    {
        while(s_y<e_y)
        {
            s_y++;
            if(s_y>Dis_Y_MAX) return;
            Writ_Dot/*Write_Dot_LCD*/(s_x,s_y,BMP_Color);
        }
        return;
    }
    else if(Offset_y==0)
    {
        while(s_x!=e_x)
        {
            s_x+=Err_d;
            if(s_x>Dis_X_MAX) return;
            Writ_Dot/*Write_Dot_LCD*/(s_x,s_y,BMP_Color);
        }
        return;
    }
	if(Offset_x>Offset_y)
	{
		Offset_k += Offset_y;
		while(s_x!=e_x)
		{
			if(Offset_k>0)
			{
				s_y+=1;
				Offset_k += (Offset_y-Offset_x);
			}
			else Offset_k += Offset_y; 
			s_x+=Err_d;
			if(s_x>Dis_X_MAX||s_y>Dis_Y_MAX) break;
			Writ_Dot/*Write_Dot_LCD*/(s_x,s_y,BMP_Color);
		}	
	}
	else
	{
		Offset_k += Offset_x;
		while(s_y!=e_y)
		{
			if(Offset_k>0)
			{
				s_x+=Err_d;
				Offset_k += (Offset_x-Offset_y);
			}
			else Offset_k += Offset_x;
			s_y+=1;
			if(s_x>Dis_X_MAX||s_y>Dis_Y_MAX) break;
			Writ_Dot/*Write_Dot_LCD*/(s_x,s_y,BMP_Color);
		}
	}    
}
#if LCD_DIS_CIRCLE==1
//========================================================================
// 函数: void W_Red_Dot(unsigned char x,unsigned char y,char a,char b,unsigned char mode)
// 描述: 绘制圆的各个像限中的点和线
// 参数: 
// 返回: 无
// 备注: 该函数对用户不可见，使用前景色
// 版本:
//      2006/10/15      First version
//========================================================================
void W_Red_Dot(DOTBYTE x,DOTBYTE y,DISWORD a,DISWORD b,BYTE mode)
{
    if(mode > 0)
    {
       Line(x+a,y+b,x-a,y+b);
       Line(x+a,y-b,x-a,y-b);
    }
    else
    {
       PutPixel(x+a,y+b);
       PutPixel(x-a,y+b);
       PutPixel(x+a,y-b);
       PutPixel(x-a,y-b);
    }
}
//========================================================================
// 函数: void W_Red_Err(int *a,int *b,int *r)
// 描述: 画圆误差计算
// 参数: 
// 返回: 无
// 备注: 该函数对用户不可见
// 版本:
//      2006/10/16      First version
//========================================================================
void W_Red_Err(DISWORD *a,DISWORD *b,DOTBYTE *r)
{
	DISWORD R_Error;
	WORD uiTemp;
	R_Error = (*a+1)*(*a+1);
	uiTemp = (*b)*(*b);
	R_Error += uiTemp;
	uiTemp = (*r)*(*r);
	R_Error -= uiTemp;
	if(R_Error>=0)
	{
		R_Error = R_Error-*b;
		if(R_Error>=0) *b = *b-1;
	}
	*a = *a+1;
}
//========================================================================
// 函数: void Circle(unsigned char x,unsigned char y,unsigned char r,unsigned char mode)
// 描述: 以x,y为圆心R为半径画一个圆(mode = 0) or 圆面(mode = 1)
// 参数: 
// 返回: 无
// 备注: 画圆函数执行较慢，如果MCU有看门狗，请作好清狗的操作
// 版本:
//      2006/10/16      First version
//========================================================================
void Circle(DOTBYTE x,DOTBYTE y,DOTBYTE r,BYTE mode)
{
	DISWORD arx1=0,ary1,arx2,ary2=0;
//	Pos_Switch(&x,&y);						//坐标变换
//	x += 4;
	ary1=r;
	arx2=r;
	while(1)
	{
        W_Red_Dot(x,y,arx1,ary1,mode);
        W_Red_Err(&arx1,&ary1,&r);
		if(arx1 == arx2)
		{
			W_Red_Dot(x,y,arx1,ary1,mode);
			break; 
		}
		W_Red_Dot(x,y,arx2,ary2,mode);
		W_Red_Err(&ary2,&arx2,&r);
		if(arx1 == arx2) 
		{
			W_Red_Dot(x,y,arx2,ary2,mode);
			break;
		}
	}
}	
#endif
//========================================================================
// 函数: void Rectangle(DOTBYTE left, DOTBYTE top, DOTBYTE right, 
//						DOTBYTE bottom, BYTE Mode)
// 描述: 以x,y为圆心R为半径画一个圆(mode = 0) or 圆面(mode = 1)
// 参数: left - 矩形的左上角横坐标，范围0到118
//		 top - 矩形的左上角纵坐标，范围0到50
//		 right - 矩形的右下角横坐标，范围1到119
//		 bottom - 矩形的右下角纵坐标，范围1到51
//		 Mode - 绘制模式，可以是下列数值之一：
//				0:	矩形框（空心矩形）
//				1:	矩形面（实心矩形）
// 返回: 无
// 备注: 画圆函数执行较慢，如果MCU有看门狗，请作好清狗的操作
// 版本:
//      2005/05/21      First version
//========================================================================
void Rectangle(DOTBYTE left, DOTBYTE top, DOTBYTE right, DOTBYTE bottom, BYTE Mode)
{
	DOTBYTE uiTemp;
	
	if(Mode==0)
	{
		Line(left,top,left,bottom);
		Line(left,top,right,top);
		Line(right,bottom,left,bottom);
		Line(right,bottom,right,top);
	}
	else
	{
//		Pos_Switch(&left,&top);						// 坐标变换
//		Pos_Switch(&right,&bottom);					//坐标变换
		if(left>right)
		{
			uiTemp = left;
			left = right;
			right = uiTemp;
		}
		if(top>bottom)
		{
			uiTemp = top;
			top = bottom;
			bottom = uiTemp;
		}
		for(uiTemp=top;uiTemp<=bottom;uiTemp++)
		{
			Line(left,uiTemp,right,uiTemp);
		}
	}
}
//========================================================================
// 函数: void ClrScreen(BYTE Mode)
// 描述: 清屏函数，执行全屏幕清除或填充前景色
// 参数: Mode   0:全屏除屏
//		1:全屏填充前景色
// 返回: 无
// 备注: 
// 版本:
//      2007/01/11      First version
//========================================================================
//这个函数有问题，调用之后后面绘图不正常 ???
void ClrScreen(BYTE Mode)
{
	if(Mode==0)
		LCD_Fill(LCD_INITIAL_COLOR);
	else
		LCD_Fill(BMP_Color);
}

