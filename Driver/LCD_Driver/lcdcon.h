////LCD 相关
#ifndef _LCD_H
#define _LCD_H

//预定义的常用文本颜色，其他可用的颜色范围：0-0xffffff
//可以指定这些颜色为前景或背景色
#define LCD_BLACK 	0x000000                  //黑
#define LCD_RED		0x0000ff                  //红
#define LCD_GREEN 	0x00ff00                  //绿
#define LCD_BLUE	0xff0000                  //蓝
#define LCD_YELLOW	(LCD_RED+LCD_GREEN)       //黄
#define LCD_MAGENTA	(LCD_RED+LCD_BLUE)        //紫
#define LCD_CYAN	(LCD_BLUE+LCD_GREEN)      //青
#define	LCD_WHITE	0xffffff                  //白

//setup

//LCD相关数据结构的初始化
void LCD_Init(void);

//用指定的颜色清除屏幕
void LCD_ClearScr(u32 color);

//设定文字的颜色
void LCD_SetColor(u32 color);

//设置背景颜色
void LCD_SetBkColor(u32 color);

//text functions

//移动到指定的行列位置，下一个字符将从这个位置开始显示
//行 row=0-19,  列 col=0-29
void LCD_Goto(u32 row, u32 col);	//row=0..19; col=0..29

//格式化的字符串输出，参数格式同C语言的printf()
int LCD_Printf(char* format,...);


#endif