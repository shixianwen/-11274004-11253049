//2013.1.14
#include "stm32f10x.h"
////LCD 相关
#ifndef _LCD_H
#define _LCD_H

#define LCD_VERTICAL  
//#define LCD_FONT12

//0-76800*4 in external sram as frame buffer
#define LCD_GUI               (volatile u32 *)0x68000000     //

//---------- 用户程序可以调用的常数和函数 --------------------
//LCD屏幕分辨率：象素点数
#ifdef LCD_VERTICAL 
#define MAX_X                 320
#define MAX_Y                 240
#else
#define MAX_X                 240
#define MAX_Y                 320
#endif


// 设置常用颜色宏定义 (格式: BBGGRR, )
#define   LCD_BLACK		0x000000      /* 黑色：    0,   0,   0 */
#define   LCD_NAVY		0x7F0000      /* 深蓝色：  0, 128,   0 */
#define   LCD_DGREEN	        0x007F00      /* 深绿色：  0, 128,   0 */
#define   LCD_DCYAN		0x7F7F00      /* 深青色：  128, 128, 0 */
#define   LCD_MAROON	        0x00007F      /* 深红色：  0,   0,  128*/
#define   LCD_PURPLE	        0x7F007F      /* 紫色：  128,   0, 128 */
#define   LCD_OLIVE		0x007F7F      /* 橄榄绿：  0, 128, 128 */
#define   LCD_LGRAY		0xC0C0C0      /* 灰白色：192, 192, 192 */
#define   LCD_DGRAY		0x7F7F7F      /* 深灰色：128, 128, 128 */
#define   LCD_BLUE		0xFF0000      /* 蓝色：  255,  0,   0  */
#define   LCD_GREEN		0x00FF00      /* 绿色：    0, 255,  0 */
#define   LCD_CYAN	        0xFFFF00      /* 青色：  255, 255,  0 */
#define   LCD_RED		0x0000FF      /* 红色：    0,  0,  255 */
#define   LCD_MAGENTA	        0xFF00FF      /* 品红：  255,   0, 255 */
#define   LCD_YELLOW	        0x00FFFF      /* 黄色：    0, 255, 255 */
#define   LCD_WHITE		0xFFFFFF      /* 白色：  255, 255, 255 */


//---- LCD 底层函数
//LCD相关数据结构的初始化
void LCD_Init(void);

//---- 文字终端方式输出字符串函数
int LCD_Printf(char* format,...);   //格式化字符串输出,参数格式同C语言的printf()
void LCD_Goto(u32 row, u32 col);    //指定下一次字符输出位置(行列)

//---- 图形方式文本输出
//设定文本颜色  
void gTextColor(u32 color);           
//设定文本背景颜色  
void gTextBkColor(u32 color);         
//设定文本背景填充模式 fill=1: 填充背景色   
void gTextBackFill(u32 fill);    
//用先前指定的颜色以给定像素坐标为左上角的位置开始输出字符串
void gTextOut(u32 x, u32 y,char *ch);                         
//用先前指定的颜色以给定像素坐标为左上角的位置开始输出格式化字符串
void gFormatTextOut(u32 x, u32 y, char* format,...);          
//用先前指定的颜色,两倍字体大小以给定像素坐标为左上角的位置开始输出字符串
void gTextOut2(u32 x, u32 y,char *ch);                         
//用先前指定的颜色,两倍字体大小以给定像素坐标为左上角的位置开始输出格式化字符串
void gFormatTextOut2(u32 x, u32 y, char* format,...);          

//---- 2-D 图形绘制函数
//用给定颜色清除LCD和绘图缓冲区
void gClearScreen(u32 color);               
//用指定的颜色在指定坐标画一个象素点
void gPoint(u32 x,u32 y,u32 color);                            
//用指定的颜色在两个坐标之间画一条直线
void gLine(u32 x0,u32 y0,u32 x1,u32 y1, u32 color);    
//用指定的颜色在np个坐标之间画一条折线
void gLineSegs(u32 const *points,u32 color,u32 np);    
//用指定的颜色在两个坐标之间画一个矩形
void gRect(int x0,int y0,int x1,int y1,u32 color);     
//用指定的颜色在两个坐标之间画一个填充矩形
void gRectFill(int x0,int y0,int x1,int y1, u32 color);       
//用指定的颜色在两个坐标范围内画一个椭圆
void gEllipse(u32 x0, u32 y0, u32 x1, u32 y1,u32 color);      
//用指定的颜色在两个坐标范围内画一个实心椭圆
void gEllipseFill(u32 x0, u32 y0, u32 x1, u32 y1, u32 color); 
//用指定的颜色以给定圆心坐标和半径画一个圆
void gCircle(u32 x0, u32 y0, u32 r, u32 color);               
//用指定的颜色以给定圆心坐标和半径画一个实心圆
void gCircleFill(u32 x0, u32 y0, u32 r, u32 color);  


//用缓冲区中的32位彩色数据填充LCD的一个矩形区域
void gRectCopy( u32 x1,u32 y1, u32 x2, u32 y2,u32 *pbuf);
//用缓冲区中的rgb字节数据填充LCD的一个矩形区域
void gRectCopyRGB( u32 x1,u32 y1, u32 x2, u32 y2, u8 *pbuf);


//---- 显示BMP图像
//在给定坐标为左上角的矩形区域显示BMP格式图像
int gBMPDraw(int x0, int y0,const void * pBMP ); 
//得到给定BMP图像的X方向尺寸
int gBMPGetXSize(const void * pBMP) ;
//得到给定BMP图像的Y方向尺寸
int gBMPGetYSize(const void * pBMP) ;

//jpeg
//得到jpeg图像的x尺寸
int gJPEGGetXSize(const void * pFileData, int DataSize);
//得到jpeg图像的x尺寸
int gJPEGGetYSize(const void * pFileData, int DataSize);
//在给定坐标为左上角的矩形区域显示jpeg图像
int gJPEGDraw(const void * pFileData, int DataSize, int x0, int y0);

#endif