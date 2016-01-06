#include "stm32f10x.h"
#include "sys.h"
#include "eios.h"

#define MAX_X 128
#define MAX_Y 64
//#include "stdlib.h"
#define _MAX_X	127       //(MAX_X-1)
#define _MAX_Y	63        //(MAX_Y-1)

//defined in LCD_Dis.h
extern unsigned char Char_Color;
extern unsigned char Font_Cover;		//字符显示的覆盖模式
extern unsigned char Char_BackColor;		//设置覆盖模式时,字符的背景色 
extern void Write_Dot_LCD(DOTBYTE x,DOTBYTE y,LCDBYTE i);


// 12*12 font
#define MAXCOL	40
#define FONTXSIZE	6
#define FONTYSIZE	12
#define DBITTEST	0x8     //00
#define BITTEST	0x02	
#define ASCSIZE	2304	//96*24	
#define ASCBYTES	24    //24
#define FONTPT	0x60000004

	
// 14*14 font
//#define MAXCOL	34
//#define FONTXSIZE	7
//#define FONTYSIZE	14
//#define ASCBYTES	14
//#define DBITTEST	0x2
//#define BITTEST	0x01
//#define ASCSIZE	1344	//96*14
////#define FONTPT	0x60031004
//#define FONTPT	0x60000004



// -----------------------------------------------------------------
//	LCD_Printf : formatted string output on LCD
//  input : 
//    variable number of inputs, like those of printf() in C lib
//  note:
//  1 Characters are displayed using text colors and background colors
//    set by gSetColor() and gSetBkColor().
//  2 Characters are on the screen grids in the manner of a console,
//    they are positioned according to rows and columns, long strings
//    are wrapped back to next row, and the text screen will scroll up
//    when strings are printed on the last row.
//    Use LCD_GotoXY() to set a new starting pos of a string. 
// -----------------------------------------------------------------
//static char lcdpch[256];
//void gFormatTextOut(u32 x, u32 y, char* format,...)
//{
//	int n;
//	va_list ap;
//	va_start(ap, format);
//	n = vsprintf(lcdpch,format,ap);
//
//	if(!n) return;
//	gTextOut(lcdpch,x,y);
//	va_end(ap);
//}

//int LCD_Printf(char* format,...)
//{
//	char d[MAXCOL];//d[30];
//	int m, k, n;
//	int c0,c;
//	
//	va_list ap;
//	va_start(ap, format);
//	n= vsprintf(lcdpch,format,ap);
//	
//	if(n==0) return 0;
//	
//	c=c0=dc.col;
//	k=0;m=0;
//	while(lcdpch[k]!=0){
//		if(lcdpch[k]==0x09){	//tab
//			d[m++]=0x20;
//			c++;
//			if((c+4)>(MAXCOL-1)){	//29){
//				while(c++<MAXCOL)	//30) 
//					d[m++]=0x20;
//				_OutLine(c0,c,d);
//				c0=c=0;
//				dc.col=0;
//				m=0;
//			}
//			else{
//				while((c&0x03)!=0){
//					d[m++]=0x20;
//					c++;
//				}
//			}
//			k++;
//		}
//		else if(lcdpch[k]==0x0a){	//LF
//			_OutLine(c0,c,d);
//			c0=c=0;
//			dc.col=0;
//			m=0;
//			dc.row++;
//			k++;
//		}
//		else if(lcdpch[k]>0x7f){	//hanzi
//			if(c>=(MAXCOL-1)){
//				d[m]=' ';			//pad a space
//				_OutLine(c0,++c,d);
//				c0=c=0;
//				dc.col=0;
//				m=0;
//			}
//			else{
//				c++;
//				d[m++]=lcdpch[k++];
//				d[m++]=lcdpch[k++];
//				if(c++>=(MAXCOL-1)){
//					_OutLine(c0,c,d);
//					c0=c=0;
//					dc.col=0;
//					m=0;
//				}
//			}
//		}
//		else{					// ascii
//			d[m++]=lcdpch[k];
//			if(c++>=(MAXCOL-1)){	//29){
//				_OutLine(c0,c,d);
//				c0=c=0;
//				dc.col=0;
//				m=0;
//			}
//			k++;
//		}
//	}
//	if(c>c0){
//		_OutLine(c0,c,d);
//		dc.col=c;
//	} 
//	
//	va_end(ap);
//
//	return n;
//
//}
//// -----------------------------------------------------------------
////	LCD_GotoXY : set a new starting position for a text string
////  input : 
////    col : column, horizontal character pos, 0-maxcol,(maxcol depends on font size)
////    row : row, vertical char pos, 0-19 
//// -----------------------------------------------------------------
//void LCD_GotoXY(u16 row, u16 col)
//{
//	dc.row=row;
//	dc.col=col;
//	
//	if(dc.row>19) dc.row=19;
//	if(dc.col>(MAXCOL-1)) dc.col=(MAXCOL-1);	//29
//} 

// -----------------------------------------------------------------
// gTextOut : 
// 	   drawing a text string ch, at pixel pos x,y
// -----------------------------------------------------------------
extern const char data_hzk12_bin[];

void LCD_TextOut(u32 x, u32 y, char *ch)
{
	char *pHanzi=(char *)(data_hzk12_bin);
	char *pFont;
	int code,m,j,k;
	int sx,sy; 
	u8 bm;
	
	if(x>=MAX_X || y>=MAX_Y) return;

	sy=FONTYSIZE;
	k=0;
	while(ch[k]!=0){
		if(ch[k]>0xa0){	//hanzi
                        //LCD_ColorSet(0);
                        //LCD_Rect(x,y,x+11,y+11,1);
                        //LCD_ColorSet(1);
			code=((ch[k]-0xa1)*94)+(ch[k+1]-0xa1);
			k++;
			pFont=(char *)(pHanzi+ASCSIZE+(code*FONTYSIZE*2));
			for(m=0;m<sy;m++){
				bm=*pFont++;
				code=128*y;
				for(j=0x80,sx=0;j>0&&x<MAX_X;j=j>>1,sx++,x++){
					if((bm&j)!=0){
                                            Write_Dot_LCD(x,y,Char_Color);
					    //LCD_Point(x,y);
					}
					else if(Font_Cover){
                                            Write_Dot_LCD(x,y,Char_BackColor);
					}
				}
				bm=*pFont++;
				for(j=0x80;j>DBITTEST&&x<MAX_X;j=j>>1,sx++,x++){
					if((bm&j)!=0){
                                            Write_Dot_LCD(x,y,Char_Color);
//						LCD_Point(x,y);
					}
					else if(Font_Cover){
                                            Write_Dot_LCD(x,y,Char_BackColor);
					}
//					else if(dc.textbkmode){
//						*(LCD_GUI+(code)+x)=dc.backcolor;
//					}
				}
				y++;
				x-=sx;	//12
			}
			x+=sx;	//12;
		}
		else{
                        //LCD_ColorSet(0);
                        //LCD_Rect(x,y,x+5,y+11,1);
                        //LCD_ColorSet(1);

			code=ch[k]-0x20;
			if(code<0 || code >95) code=0;
			pFont=(char *)(pHanzi+(code*ASCBYTES));
			for(m=0;m<sy;m++){
				bm=*pFont++;
                                pFont++;
				//code=240*y;
			   	for(j=0x80,sx=0;j>BITTEST&&x<MAX_X;j=j>>1,sx++,x++){ 
					if(((bm)&j)!=0){
                                            Write_Dot_LCD(x,y,Char_Color);
					//	LCD_Point(x,y);//*(LCD_GUI+(code)+x)=dc.color;
				 	}	
					else if(Font_Cover){
                                            Write_Dot_LCD(x,y,Char_BackColor);
					}
//					else if(dc.textbkmode){
//						*(LCD_GUI+(code)+x)=dc.backcolor;
//					}
			     	//{LCD_Point1(x,y,color);}
			     	//x++;
			   	}
			   	y++;
			   	x-=sx;	//6;
			}
			x+=sx;	//6;
		}
		y-=sy;	//12;
		k+=1;
		if(x>=MAX_X) break;
	}

//	LCD_Copy((u8*)LCD_GUI, x0,y,x-1,y+sy-1);

}	
	
