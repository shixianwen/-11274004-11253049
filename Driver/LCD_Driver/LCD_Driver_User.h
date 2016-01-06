//	write your header here
#include "LCD_Config.h"

extern LCDBYTE LCD_DataRead(void);
extern void LCD_RegWrite(LCDBYTE Command);
extern void LCD_DataWrite(LCDBYTE Dat);
extern void Write_Dot_LCD(LCDBYTE x,LCDBYTE y,LCDBYTE Color);
//extern LCDBYTE Get_Dot_LCD(WORD x,WORD y);
//extern void Clear_Dot_LCD(WORD x,WORD y);
extern void Set_Dot_Addr_LCD(LCDBYTE x,LCDBYTE y);
extern void LCD_Fill(LCDBYTE a);
//extern void LCD_Fill_s(WORD Number,LCDBYTE Color);
