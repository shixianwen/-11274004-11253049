//2013.1.14
#include "stm32f10x.h"

#include "lcdcon.h"

#include "stdarg.h"
#include "stdio.h"
#include "locale.h"

#define LCD_VERTICAL  

#define ADR_LCD_CMD 0x60000010
#define ADR_LCD_DAT 0x60000011

#define LCD_CMD               (*(volatile u8 *)ADR_LCD_CMD)
#define LCD_DATA              (*(volatile u8 *)ADR_LCD_DAT)

//0-76800*4 in external sram as frame buffer
#define LCD_GUI               (volatile u32 *)0x68000000     //

#ifdef LCD_VERTICAL 
#define MAX_X                 320
#define MAX_Y                 240
#else
#define MAX_X                 240
#define MAX_Y                 320
#endif
#define MAX_X_1 (MAX_X-1)
#define MAX_Y_1 (MAX_Y-1)
#define MAXROWS (MAX_Y/16)
#define MAXCOLS (MAX_X/8)

//font data in ascii.c
extern  const unsigned char *Font8x12ASCII_CharInfo[96];


//====local functions :LCD module low level IO ==================================
//internal 
void TextOut1(u8 *ch,u16 x, u16 y);
int LCD_Copy(u32* pbuf, int x1,int y1, int x2, int y2);

//hardware access
void write_cmd(u16);//单次命令
void write_reg(u16);
void write_data(u8);
u16 read_reg(void);


typedef struct //tDispContext
{
	u32 color;		//fore color
	u32 backcolor;		//background color
	u32 row,col;		//row=0-29, col=0-19
//	u16 *lut;		//pointer to color LUT
}GDC, *pGDC;
GDC dc;
//u16 GLut[256];



//locale functions
void ssd1297_init(void);
void ssd1297_rect_set(int xs, int ys, int xe,int ye);
void DelayNS(u32 dly)
{
	u32 i;
	for(;dly>0;dly--)
		for(i=0;i<50000;i++);
}

inline u32 RGB2Int(u8 r,u8 g,u8 b)
{
  return (b<<16)|(g<<8)|r;
}

//================LCD display setup =======================================
void LCD_Init(void)
{
	//int i;	
	dc.row=dc.col=0;
	dc.backcolor=LCD_BLACK;
	dc.color=LCD_WHITE;
	//dc.lut=GLut;
	
	//for(i=0;i<256;i++){			//set default lut
	//	dc.lut[i]=i;
	//}
	
        //low level init
        ssd1297_init();

	//LCD_ClearScr(dc.lut[dc.backcolor]);	//clear screen
	LCD_ClearScr(dc.backcolor);	//clear screen
	//setlocale(LC_ALL,"C");
	
}

void LCD_SetColor(u32 color)
{
	dc.color=color;
}
void LCD_SetBkColor(u32 color)
{
	dc.backcolor=color;
}

//===============block ops =================================================
void LCD_ClearScr(u32 color)
{
  int j,k;
 
   for (j=0;j<MAX_Y;j++)
   {
     for (k=0;k<MAX_X;k++)
       {
         *(LCD_GUI+(MAX_X*j)+k)=color;
       }
    }

   ssd1297_rect_set(0,0,MAX_X_1,MAX_Y_1);
   //write_cmd(0x22);
				
   for (k=0;k<(MAX_Y*MAX_X);k++)    //76800
   {
       write_data((u8)color);
       write_data((u8)(color>>8));
       write_data((u8)(color>>16));
   }	

  dc.row=dc.col=0;

  //test
/*
  ssd1297_rect_set(10,40,100-1,150-1);
   for (k=0;k<15000;k++)
   {
       write_data((u8)0x00);
       write_data((u8)0x00);
       write_data((u8)0xff);
   }	
*/  

}  

//copy a rect area of frame buffer into LCD GRAM
int LCD_Copy(u32 *pbuf, int x1,int y1, int x2, int y2)
{
//	u16 xs,xe,ys,ye,k;//,p1;
	int n,j,k ;
	int p1;
 
	if(x2<x1){
		k=x2; x2=x1; x1=k;
	}
	//xs=MAX_X_1-x2;xe=MAX_X_1-x1;
	
	if(y2<y1){
		k=y2;y2=y1;y1=k;
	}
	//ys=MAX_Y_1-y2;ye=MAX_Y_1-y1;
        
        ssd1297_rect_set( x1, y1, x2, y2);
        
	//fill buffer
	n=y2*MAX_X;
	for (j=(y1*MAX_X);j<=n;j+=MAX_X){
            for (k=x1;k<=x2;k++){
                p1=*(LCD_GUI+(j)+k);
                //p1=*(pbuf+(j)+k);
                write_data((u8)p1);
                write_data((u8)(p1>>8));
                write_data((u8)(p1>>16));
            }
	}
/* 
        ssd1297_rect_set( x1, x2-x1, y1,y2-y1);
        
	//fill buffer
	n=y2*240;
	for (j=(y1*240);j<=n;j+=240){
            for (k=x1;k<=x2;k++){
                p1=*(LCD_GUI+(j)+k);
                //p1=*(pbuf+(j)+k);
                write_data((u8)p1);
                write_data((u8)(p1>>8));
                write_data((u8)(p1>>16));
            }
	}
*/     
	return 1;
}  
//==============text mode ===========================================
void OutLine(int c0,int c, char *d)
{
	int j,k;
	u32 *p=(u32*)LCD_GUI;  //?
        u32 bk=dc.backcolor;
	
//	if((dc.row)>=19){
	if((dc.row)>(MAXROWS-1)){    //19
		for(k=0;k<(MAX_X*(MAX_Y-16));k+=(MAX_X*16)){  //3840
			for(j=0;j<(MAX_X*16);j++){
				p[k+j]=p[k+j+(MAX_X*16)];
			}
		}
		for(j=0;j<(MAX_X*16);j++){
			p[k+j]=bk;
		}
		dc.row--;
		//dc.color=LCD_GREEN;
		LCD_Copy((u32*)LCD_GUI, 0,0,MAX_X_1,MAX_Y_1);

	}
	d[c-c0]=0; 
	TextOut1((u8*)d,c0<<3,dc.row<<4);
	LCD_Copy((u32*)LCD_GUI, 0,dc.row<<4,MAX_X_1,(dc.row<<4)+15);
	if(c>(MAXCOLS-1))   //29
//		if(dc.row<19)
		if(dc.row<MAXROWS) //20
			dc.row++;	

}

static char lcdpch[256];
int LCD_Printf(char* format,...)
{
	char d[30];
	int m, k, n;
	int c0,c;
	
	va_list ap;
	va_start(ap, format);
	n= vsprintf(lcdpch,format,ap);
	
	if(n==0) return 0;
	
	c=c0=dc.col;
	k=0;m=0;
	while(lcdpch[k]!=0){
		if(lcdpch[k]==0x09){	//tab
			d[m++]=0x20;
			c++;
			if((c+5)>(MAXCOLS-1)){ //29
				while(c++<(MAXCOLS)) d[m++]=0x20;
				OutLine(c0,c,d);
				c0=c=0;
				dc.col=0;
				m=0;
			}
			else{
				while(c%5!=0){
					d[m++]=0x20;
					c++;
				}
			}
		}
		else if(lcdpch[k]==0x0a){	//LF
			OutLine(c0,c,d);
			c0=c=0;
			dc.col=0;
			m=0;
			dc.row++;
		}
		else if(lcdpch[k]==0x0d){	//CR
			OutLine(c0,c,d);
			c0=c=0;
			dc.col=0;
			m=0;
			//dc.row++;
		}
		else{
			d[m++]=lcdpch[k];
			if(c++>=(MAXCOLS-1)){ //29
				OutLine(c0,c,d);
				c0=c=0;
				dc.col=0;
				m=0;
			}
		}
			
		k++;
	}
	if(c>c0){
		OutLine(c0,c,d);
		dc.col=c;
	} 
	
	va_end(ap);

	return n;

}

void LCD_Goto(u32 row, u32 col)
{
	dc.row=row;
	dc.col=col;
	
	if(dc.row>(MAXROWS-1)) dc.row=(MAXROWS-1);  //19
	if(dc.col>(MAXCOLS-1)) dc.col=(MAXCOLS-1);  //29
} 

//12*8 textout
void TextOut1(u8 *ch,u16 x, u16 y)
{
	u8* pch;
	int code,m,j,k;
        u32 bk=dc.backcolor;
        u32 color=dc.color;

    //u16 const TAB[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	k=0;
	while(ch[k]!=0){
		code=ch[k]-0x20;
		if(code<0 || code >95) code=0;
		pch=(u8*)Font8x12ASCII_CharInfo[code];

		for(m=0;m<12;m++){
			code=MAX_X*y;
		   	for(j=0x80;j>0;j=j>>1){ 
				if((pch[m]&j)!=0){
					*(LCD_GUI+(code)+x)=color;
			 	}	
			 	else{
			 		*(LCD_GUI+(code)+x)=bk;
			 	}
		     //{LCD_Point1(x,y,color);}
		     	x++;
		   	}
		   	y++;
		   	x-=8;
		}
		x+=8;
		y-=12;
		k+=1;
	}
}	

//====================LCD module low level IO ==================================

inline void write_cmd( u16 cmd)
{
  LCD_CMD = 0;  //cmd>>8;
  LCD_CMD = cmd&0xff;
}

inline void write_reg(u16 data)
{
  LCD_DATA = (data>>8);	
  LCD_DATA = data&0xff;	
}

inline void write_data(u8 data)
{
  LCD_DATA = data;	
}


u16 read_reg(void)
{
  u16 t;
  t=(LCD_DATA&0xff)<<8;
  t|=(LCD_DATA&0xff);
  return t;
}

//set GRAM window and AC address
void ssd1297_rect_set(int xs, int ys, int xe,int ye)
{
#ifdef LCD_VERTICAL
    write_cmd(0x45);				/* Horizontal RAM Start ADDR */
    write_reg(xs);
    write_cmd(0x46);				/* Horizontal RAM End ADDR */
    write_reg(xe);
  
    write_cmd(0x44);				/* Vertical Start,End ADDR */
    write_reg(((ye)<<8)|(ys));
  
    write_cmd(0x4F);				/* GRAM Vertical/Horizontal ADDR Set(AD0~AD7) */
    write_reg(xs);
    write_cmd(0x4E);				/* GRAM Vertical/Horizontal ADDR Set(AD8~AD16) */
    write_reg(ys);
#else
    write_cmd(0x45);				/* Horizontal RAM Start ADDR */
    write_reg(ys);
    write_cmd(0x46);				/* Horizontal RAM End ADDR */
    write_reg(ye);
  
    write_cmd(0x44);				/* Vertical Start,End ADDR */
    write_reg(((xe)<<8)|(xs));
  
    write_cmd(0x4F);				/* GRAM Vertical/Horizontal ADDR Set(AD0~AD7) */
    write_reg(ys);
    write_cmd(0x4E);				/* GRAM Vertical/Horizontal ADDR Set(AD8~AD16) */
    write_reg(xs);
#endif    
  
    write_cmd(0x22);				/* Write Data to GRAM */

}

//low level init of controller
volatile u16 t16;
void ssd1297_init(void)
{
  write_cmd(0xE5);  //
  write_reg(0x8000);  //
  write_cmd(0x0028);  //选择寄存器r28h
  write_reg(0x0006);  //
  write_cmd(0x0007);  //
  write_reg(0x0021);  //
  write_cmd(0x0000);  //
  write_reg(0x0001);  //
  DelayNS(50); 
  
  write_cmd(0x0007);  //
  write_reg(0x0023);  //
  write_cmd(0x0010);  //
  write_reg(0x0000);  //推出sleep模式
  DelayNS(50); 

  write_cmd(0x0007);  //
  write_reg(0x0033);  //

  write_cmd(0x0001);  //
#ifdef LCD_VERTICAL  
  // x*y=240*320 : x rev dir, y normal dir.  d14=RL=0, d9=TB=0 , d11=BGR=1: R is the LSB
  write_reg(0x293f);//0x213f);  //Driver Output Control: RGB order, scan order
#else
  // x*y=240*320: normal direction  
  write_reg(0x693f);//0x613f);//0x6b3f);//0x2b23);  //Driver Output Control: RGB order, scan order
#endif

  write_cmd(0x0002);  //
  write_reg(0x0600);  //

  write_cmd(0x0011);  //Entry Mode: interface, color mode, data address changing mode
  //d5d4: ID1ID0, d3:AM. AM=0: horizontal
#ifdef LCD_VERTICAL
  write_reg(0x4038);//0x4030);  //vertical
#else
  write_reg(0x4030);//0x4030);  // horizontal
#endif  
  
  write_cmd(0x0003);  //
  write_reg(0xaa0e);//0xa6a8);  //
  write_cmd(0x000f);  //
  write_reg(0x0000);  //
  write_cmd(0x000e);  //
  write_reg(0x3200);//0x3100);  //
  write_cmd(0x001e);  //
  write_reg(0x009f);  //

  write_cmd(0x000c);  //
  write_reg(0x0005);  //
  write_cmd(0x000d);  //
  write_reg(0x0008);//0x0005);  //

  write_cmd(0x000b);  //
  write_reg(0x5308);  //

  write_cmd(0x0025);  //
  write_reg(0xe000);  //
  write_cmd(0x003f);  //
  write_reg(0xbb84);  //
//  write_cmd(0x0027);  //
//  write_reg(0x0567);  //
//  write_cmd(0x0020);  //
//  write_reg(0x316c);  //

  write_cmd(0x3a);  //
  write_reg(0x1000);  //
  write_cmd(0x3b);  //
  write_reg(0x0008);  //
  write_cmd(0x30);  //
  write_reg(0x0007);  //
  write_cmd(0x31);  //
  write_reg(0x0003);  //
  write_cmd(0x32);  //
  write_reg(0x0300);  //
  write_cmd(0x33);  //
  write_reg(0x0101);  //
  write_cmd(0x34);  //
  write_reg(0x0707);  //
  write_cmd(0x35);  //
  write_reg(0x0002);  //
  write_cmd(0x36);  //
  write_reg(0x0503);  //
  write_cmd(0x37);  //
  write_reg(0x0202);  //
  write_cmd(0x3a);  //
  write_reg(0x1000);  //
  write_cmd(0x3b);  //
  write_reg(0x0008);  //

  write_cmd(0x44);  //
  write_reg(0xef00);  //
  write_cmd(0x45);  //
  write_reg(0x0000);  //
  write_cmd(0x46);  //
  write_reg(0x013f);  //
  write_cmd(0x4e);  //
  write_reg(0x0000);  //
  write_cmd(0x4f);  //
  write_reg(0x0000);  //
  write_cmd(0x48);  //
  write_reg(0x0000);  //
  write_cmd(0x49);  //
  write_reg(0x013f);  //
  write_cmd(0x4a);  //
  write_reg(0xef00);  //
  write_cmd(0x4b);  //
  write_reg(0x013f);  //
  
  /*
//---------------------------  
  write_cmd(0x0028);  //选择寄存器r28h
  write_reg(0x0006);  //
 
  write_cmd(0x0000);  //选择寄存器r00h
//  t16=read_reg();   //read controller ID, should be 0x9797
  
  write_reg(0x0001);  //启动OSC

  write_cmd(0x0010);  //
  write_reg(0x0000);  //推出sleep模式
  
  write_cmd(0x0007);  //
  write_reg(0x0033);  //display on


  write_cmd(0x0011);  //
  write_reg(0x4030);  //0x4d30);  //0x4d30: 262k color, ignore HSYNC etc, write RAM with RGB data
                      //use internal osc, RAM AC H V inc

  write_cmd(0x0001);  //
  write_reg(0x6b3f);  //output control: grey data bit order, RGB order, output lines
  
  write_cmd(0x000e);  //
  write_reg(0x3100);  //set VcomG driver voltage amplitue
  
  write_cmd(0x0003);  //
  write_reg(0xaaa8);  //power control
  
  write_cmd(0x000f);  //
  write_reg(0x0000);  //set scan line start pos
  
  write_cmd(0x001e);  //
  write_reg(0x00b2);  //set VcomH driver voltage amplitue
  
  write_cmd(0x000c);  //
  write_reg(0x0005);  //set VCIX2 output voltage to 6.1V
  
  write_cmd(0x000d);  //
  write_reg(0x0008);  //set VLCD63 : =Vref*2.165

  write_cmd(0x000b);  //
  write_reg(0x5308);  //set frame sycle, =default value

  write_cmd(0x0025);  //
  write_reg(0x8000);  //set frame frequency, =default 510KHz


  write_cmd(0x003f);  //??
  write_reg(0xbb84);  //set VcomG driver voltage amplitue
  
 
  //------- gama control
  write_cmd(0x0030);  //
  write_reg(0x0600);  //
  write_cmd(0x0031);  //
  write_reg(0x0707);  //
  write_cmd(0x0032);  //
  write_reg(0x0000);  //
  write_cmd(0x0033);  //
  write_reg(0x0000);  //
  write_cmd(0x0034);  //
  write_reg(0x0704);  //
  write_cmd(0x0035);  //
  write_reg(0x0007);  //
  write_cmd(0x0036);  //
  write_reg(0x0000);  //
  write_cmd(0x0037);  //
  write_reg(0x0000);  //
  write_cmd(0x003a);  //
  write_reg(0x0b00);  //
  write_cmd(0x003b);  //
  write_reg(0x0006);  //
  
  */
    
}
/*
void Dwrite_cmd( u16 cmd)
{
	LCD_CMD = cmd;	
	LCD_CMD = cmd;
}

void Dwrite_data(u16 data)
{
	LCD_DATA = data;	
	LCD_DATA = data;
}

u16 Dread_data(void)
{
	int temp;
	temp = LCD_DATA;
	temp =  LCD_DATA;
	return temp;
}

void power_off(void)
{
	Dwrite_cmd(0x0021);  //Display off
	Dwrite_data(0x0030);
	
	Dwrite_cmd(0x0021);  //HALT
	Dwrite_data(0x0070);
}
 
 */ 