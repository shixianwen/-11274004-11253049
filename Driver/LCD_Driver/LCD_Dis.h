#include "LCD_Config.h"
#include "LCD_PortConfig.h"
#include "LCD_Driver_User.h"  //


//===========================================================
//用户可调用的函数格式
/* 
LCD api函数/宏调用说明

1 void LCD_Init(void);

初始化显示模块


2 void LCD_ColorSet(unsigned char color);

设置绘图色彩: 
输入参数: color=1 , 0


3 void LCD_FontSet(unsigned char Font_NUM);

ascii字符输出函数的字体大小设置
输入参数：
font =0 : 6x10英文小字体
     =1 : 8x16英文大字体


4 unsigned char LCD_FontColor(unsigned char Color);

设置字体颜色
输入参数：Color=0,1


5 void LCD_FontBackFill(int fill);

设置字符显示的覆盖或叠加模式
输入参数  fill=1: 用背景色填充字符区域, fill=0: 不填充


6 LCD_PutChar(unsigned char x,unsigned char y,char asc)

在指定位置写入一个标准字符
输入参数: x : 字符左上角象素水平坐标(0-127，0为最左侧，127为最右侧)
          y : 字符左上角象素垂直坐标(0-63，0为最上侧，63为最右下)
          asc :  要显示的字符的asc码  


7 LCD_PutString(unsigned char x,unsigned char y,char *s)

在x、y为起始坐标处写入一串标准字符
输入参数: x : 字符左上角象素水平坐标(0-127，0为最左侧，127为最右侧)
          y : 字符左上角象素垂直坐标(0-63，0为最上侧，63为最右下)
          s :  要显示的字符串

8 void LCD_TextOut(u32 x, u32 y, char *ch);

输出12x12点阵汉字的函数
输入参数 x,y : 起始字符的左上角坐标
         ch  : 字符串


9 LCD_Point(unsigned char x, unsigned char y)

在指定位置上绘制一个前景色的点。
输入参数: x : 图形点的象素水平坐标(0-127，0为最左侧，127为最右侧)
          y : 图形点的象素垂直坐标(0-63，0为最上侧，63为最右下)
  点的颜色由最近一次对函数LCD_ColorSet()的调用决定


10 LCD_Line(unsigned char x1, unsigned char y1，unsigned char x2, unsigned char y2)

在两点之间画一条直线，颜色由最近一次对函数LCD_ColorSet()的调用决定。
输入参数: x1,y1 : 直线的起点坐标(x1 =0-127，y1 = 0-63)
          x2,y2 : 直线的终点坐标(x2 =0-127，y2 = 0-63)


11 LCD_Rect(unsigned char x1, unsigned char y1，unsigned char x2, unsigned char y2,unsigned char mode)

画一个矩形，选择填充或不填充，颜色由最近一次对函数LCD_ColorSet()的调用决定。
输入参数: x1,y1 : 矩形的左上角坐标(x1 =0-127，y1 = 0-63)
          x2,y2 : 矩形的右下角坐标(x2 =0-127，y2 = 0-63)
          mode : =0 :不填充，空心矩形
                 =1 :填充，实心矩形

12 LCD_Circle(unsigned char x,unsigned char y,unsigned char r,unsigned char mode)

画一个圆形，选择填充或不填充，颜色由最近一次对函数LCD_ColorSet()的调用决定。
输入参数: x,y : 圆形的圆心坐标(x1 =0-127，y1 = 0-63)
          r   : 圆形的直径，以象素点为单位
          mode : =0 :不填充，空心圆
                 =1 :填充，实心圆

13 LCD_ClrScreen(unsigned char mode)

用制定的颜色清除屏幕
输入参数 mode = 0 : 用默认的背景色(0)
         mode = 1 : 用前面设定的前景色(0或1)

*/

//===========================================================================
//重新定义绘图api函数名，建议用户调用的名称

//---- 画直线函数，x1、y1为起始点，x2、y2为结束点
// void LCD_Line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2) 
#define LCD_Line(x1,y1,x2,y2)  Line(x1,y1,x2,y2)

//---- 设置绘图前景颜色：color=0,1
// void LCD_ColorSet(uint8_t color)
#define LCD_ColorSet(color) SetPaintMode(0,color)

//---- 在像素点坐标(x,y)为左上角的矩形位置，输出一个5x10大小的字符a。
// void LCD_PutChar(uint8_t x, uint8_t y, uint8_t a)
#define LCD_PutChar(x,y,a) PutCharPutChar(x,y,a) 

// ---- 在像素点坐标(x,y)为左上角的矩形位置开始，输出一个字符串s。
// void LCD_PutString(uint8_t x, uint8_t y, uint8_t *s)
#define LCD_PutString(x,y,s) PutString(x,y,s)

// ---- 在像素点坐标(x,y)处描绘一个点，颜色由之前LCD_ColorSet()设定。
// void LCD_Point(uint8_t x, uint8_t y) 
#define LCD_Point(x,y) PutPixel(x,y)

// ---- 画一个矩形，左上角坐标(x1,y1)，右下角坐标(x2,y2), 颜色由之前LCD_ColorSet()设定。
//      m=0: 内部不填充， m=1: 内部填充
// void LCD_Rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t m)
#define LCD_Rect(x1,y1,x2,y2,m) Rectangle(x1,y1,x2,y2,m)

// ---- 画一个园，圆心坐标(x,y)，半径r， 颜色由之前LCD_ColorSet()设定。
//      m=0: 内部不填充， m=1: 内部填充
// void LCD_Circle(uint8_t x, uint8_t y, uint8_t r, uint8_t m)
#define LCD_Circle(x,y,r,m) Circle(x,y,r,m)

// ---- 用指定的颜色清除屏幕 : c =0,1 为颜色
// void LCD_ClrScreen(uint8_t c) 
#define LCD_ClrScreen(c) ClrScreen(c)


// ---- LCD 初始化函数, 用于LCD屏的寄存器初始化
extern void LCD_Init(void);	//在LCD_Driver_User.c中定义的函数

// ---- 标准字符设置，包括两型号ASCII码的大小尺寸，0，1
extern void LCD_FontSet(unsigned char Font_NUM);

// ---- 设置字体颜色0，1
extern unsigned char LCD_FontColor(unsigned char Color);

// ---- 设置字符显示的覆盖或叠加模式, fill=1: 用背景色填充字符区域, fill=0: 不填充
extern void LCD_FontBackFill(int fill);

// ---- 输出12x12点阵汉字的函数，x,y : 起始字符的左上角坐标，ch :字符串
extern void LCD_TextOut(u32 x, u32 y, char *ch);









//===================================================================
//以下这些函数内部使用，用户不需调用
//画直线函数，s_x、s_y为起始点，e_x、e_y为结束点
extern void Line(LCDBYTE s_x,LCDBYTE s_y,LCDBYTE e_x,LCDBYTE e_y);
//绘图模式、当前绘图颜色设置
extern void SetPaintMode(BYTE Mode,LCDBYTE Color);

//于x、y的坐标上写入一个标准字符
extern void PutChar(DOTBYTE x,DOTBYTE y,char a);
//于x、y的坐标为起始写入一串标准字符串
extern void PutString(DOTBYTE x,DOTBYTE y,char *p);
//于x、y的坐标为中心，绘制一个点
extern void PutPixel(DOTBYTE x,DOTBYTE y);
//于x、y的坐标为中心，绘制一个圆边或实心圆
extern void Circle(DOTBYTE x,DOTBYTE y,DOTBYTE r,BYTE mode);
//绘制一个以left、top和right、bottom为两个对角的矩形框或者实心矩形
extern void Rectangle(DOTBYTE left, DOTBYTE top, DOTBYTE right, DOTBYTE bottom, BYTE Mode);
//清屏函数，执行全屏幕清除或填充前景色
extern void ClrScreen(BYTE Mode);

//以下函数以及变量的声明一般建议用户不要调用，仅供高级用户在自行编写特殊显示效果以及特性应用程序时使用
//

extern DOTBYTE X_Witch;						//字符写入时的宽度
extern DOTBYTE Y_Witch;						//字符写入时的高度
extern DOTBYTE Font_Wrod;					//字体的每个字模占用多少个存储单元数
extern const unsigned char *Char_TAB;				//字库指针
//unsigned char Plot_Mode;					//绘图模式
extern LCDBYTE BMP_Color;
extern LCDBYTE Char_Color;

extern BYTE	Font_type;
extern BYTE	Font_Cover;								//
extern LCDBYTE	Char_BackColor;							//

//extern void Pos_Switch(unsigned int * x,unsigned int * y);
//extern void Writ_Dot(int x,int y,unsigned int Color);
//extern void Line_f(int s_x,int s_y,int e_x);
//extern unsigned int Get_Dot(int x,int y);
//extern void Clear_Dot(int x,int y);
//extern void Set_Dot_Addr(int x,int y);
