//2013.1.14
//2013..2.21
//汉字库hzk12.c或hzk14.c利用连接器命令放在了片内FLASH的地址0x08047000位置
//为了避免每次下载代码都重复烧写字库，第一次使用lcdlib函数时，要将hzk14.c或
//hzk12.c包含到编译连接中，利用代码下载将字库烧写到指定位置，以后将字库文件排出到编译连接之外。
#include "stm32f10x.h"
#include "lcdlib.h"

#include "stdarg.h"
#include "stdio.h"
//#include "locale.h"

//#include "stdlib.h"

#define USE_FRAME_BUFFER

//读写LCD控制器的地址
#define ADR_LCD_CMD 0x60000010
#define ADR_LCD_DAT 0x60000011

#define LCD_CMD               (*(volatile u8 *)ADR_LCD_CMD)
#define LCD_DATA              (*(volatile u8 *)ADR_LCD_DAT)


//字模数组
extern const char data_hzk12_bin[];
extern const char font_SimSun14[];


#define MAX_X_1 (MAX_X-1)
#define MAX_Y_1 (MAX_Y-1)
#define MAXROWS (MAX_Y/16)            //字符行列数
#define ROW_HT	16

#ifdef LCD_FONT12
//12*12font
#define MAXCOLS (MAX_X/6)
#define FONTXSIZE	6
#define FONTYSIZE	12
#define ASCBYTES	24
#define DBITTEST	0x8   //00
#define BITTEST	0x02	
#define ASCSIZE	2304	//96*24	
#else
// 14*14 font
#define MAXCOLS (MAX_X/7)
#define FONTXSIZE	7
#define FONTYSIZE	14
#define ASCBYTES	14
#define DBITTEST	0x2
#define BITTEST	0x01
#define ASCSIZE	1344	//96*14
#endif

//---------- 图形库内部支持函数和常数定义 --------------------

#define LCD_CC_UNLOCK (0)    /* Default mode: Cache is transparent */
#define LCD_CC_LOCK   (1)    /* Cache is locked, no write operations */
#define LCD_CC_FLUSH  (2)    /* Flush cache, do not change mode */

typedef struct 
{
	u32 color;		//fore color
	u32 backcolor;		//background color
	u32 textbkmode;
	u32 autoreturn;		//if add return for longer strings
	u32 row,col;		//row=0-(240/FontXSize), col=0-19
	//u32 *lut;			//pointer to color LUT
	u32 *cache;			//pointer to graph buffer
	const char *pFont;

	u32	cache_x0, cache_y0;
	u32	cache_x1, cache_y1;
	u32     cache_lock_cnt;
	u32	cache_mode;
	
}GDC, *pGDC;
GDC dc;
static char lcdpch[256];


// --supporting functions --
void _LCD_HLine(int y, int x0, int x1, u32 color);
void _LCD_VLine(int x, int y0, int y1, u32 colorx);
void _LCD_Point(u32 x,u32 y,u32 color);
void _LCD_Rect_Fill(u32 x1,u32 y1,u32 x2,u32 y2,u32 color);
void _LCD_Rect_Copy(u32 x1,u32 y1,u32 x2,u32 y2,u32* pbuf);

u32 LCD_ControlCache(u32 mode);
int LCD_Cache2LCD(u32 x0,u32 y0,u32 x1,u32 y1);
int LCD_FlushCache(void);


//====local functions :LCD module low level IO ==================================
//internal 

// internal use
void _OutLine(int c0,int c, char *s);
void _Circle(u32 x0, u32 y0, u32 r, u32 color,u32 fill);
void _Ellipse(u32 x0, u32 y0, u32 x1, u32 y1, u32 color, u32 fill);
//void LCD_FillRect(int x0,int y0,int x1,int y1, u32 color);

//hardware access
inline void write_cmd(u16);
inline void write_reg(u16);
inline void write_data(u8);
inline u8 read_data(void);
inline u16 read_reg(void);

void ssd1297_init(void);
void ssd1297_rect_set(int xs, int ys, int xe,int ye);
void ssd1297_addr_set(int xs, int ys);
//


static void DelayNS(u32 dly)
{
	u32 i;
	for(;dly>0;dly--)
		for(i=0;i<50000;i++);
}

//=========================================================================
// LCD 底层函数
//=========================================================================
// -----------------------------------------------------------------
//	LCD显示初始化 
// -----------------------------------------------------------------
void LCD_Init(void)
{ 
	//int i,n;	
	dc.row=dc.col=0;
	dc.backcolor=LCD_BLACK;
	dc.textbkmode=1;
	dc.color=LCD_WHITE;
	dc.cache=(u32*)LCD_GUI;		//extern RAM at 0x62000000(need 76800Byte)
	//dc.lut=DefaultLut;//GLut;
	
	dc.cache_x0=MAX_X;
	dc.cache_y0=MAX_Y;
	dc.cache_x1=0;
	dc.cache_y1=0;
	dc.cache_mode=LCD_CC_UNLOCK;
	dc.cache_lock_cnt=0;
        
//#ifdef LCD_FONT12
//        dc.pFont=data_hzk12_bin;
//#else
//        dc.pFont=font_SimSun14;
//#endif        
        dc.pFont=(const char *)0x08047000;
        	
        //low level init
        ssd1297_init();

	gClearScreen(dc.backcolor);		//clear screen
	
	//setlocale(LC_ALL,"C");
	
}
// -----------------------------------------------------------------
//  用给定颜色清除LCD 
//  input : color = 32 bit color pattern to fill the screen
// -----------------------------------------------------------------
void LCD_ClearScr(u32 color)
{
    u32 i=MAX_Y*MAX_X;

#ifdef USE_FRAME_BUFFER
    volatile u32 *p=LCD_GUI;
    // fill the buffer with given color
    while(i--){
      *p++=color;
    }
#endif
    // set the rect area of GRAM to update  
    ssd1297_rect_set(0,0,MAX_X_1,MAX_Y_1);
    //write_cmd(0x22);
				
    i=MAX_Y*MAX_X;   //76800
    while(i--){
       write_data((u8)color);         //R
       write_data((u8)(color>>8));    //G
       write_data((u8)(color>>16));   //B
    }	

    dc.row=dc.col=0;        //cursor position

}  



//=========================================================================
// 2-D 图形绘制函数
//=========================================================================

// -----------------------------------------------------------------
// gTextColor : 设定文字输出的前景颜色
// input : 
//    color : 32-bit color pattern
// -----------------------------------------------------------------
void gTextColor(u32 color)
{
    dc.color=color;
}
// -----------------------------------------------------------------
// gTextBkColor : 设定文字输出的背景颜色
// input : 
//    color : 32-bit color pattern
// -----------------------------------------------------------------
void gTextBkColor(u32 color)
{
    dc.backcolor=color;
}
// -----------------------------------------------------------------
// 函数: void gTextBackFill(int fill) 
// 设置背景覆盖模式
// fill =1 : 覆盖模式，用字体背景色填充
// fill =0 : 叠加模式，不填充，保留原有内容
// -----------------------------------------------------------------
void gTextBackFill(u32 fill)
{
    dc.textbkmode=fill;
}
// -----------------------------------------------------------------
// gClearScreen : 用指定编号颜色填充LCD屏幕(和绘图缓冲区)
// input : 
//    color : 32-bit color pattern
// -----------------------------------------------------------------
void gClearScreen(u32 color)
{
	LCD_ClearScr(color);
}
// -----------------------------------------------------------------
// gPoint : 在指定像素位置画点 
// 	input :
//    x : 水平方向像素坐标，0-319(MAX_X)
//    y : 竖直方向像素坐标，0-239(MAX_Y)
//    color : 32-bit彩色值
// -----------------------------------------------------------------
void gPoint(u32 x,u32 y,u32 color)
{
       
#ifdef USE_FRAME_BUFFER
        //if(x>MAX_X_1) return;
        //if(y>MAX_Y_1) return;
	//write graph buffer 
	u32* p=dc.cache;
	*(p+(MAX_X*y)+x)=color;
#endif
        
  _LCD_Point(x,y,color);
  return;

 /* 
        if(x>=MAX_X) x=MAX_X-1;
	if(y>=MAX_Y) y=MAX_Y-1;
	if(dc.cache_mode==LCD_CC_LOCK){
		if(dc.cache_x0>x) dc.cache_x0=x;
		if(dc.cache_y0>y) dc.cache_y0=y;
		if(dc.cache_x1<x) dc.cache_x1=x;
		if(dc.cache_y1<y) dc.cache_y1=y;
		return;
	}

	LCD_Point(x,y,color);
*/	
}
	
// -----------------------------------------------------------------
// gLine : 在指定两个像素点之间画线 
// 	input :
//    x0, y0 : 第一个像素点水平和竖直方向坐标，
//    x1, y1 : 第二个像素点水平和竖直方向坐标，
//    color_index : 32-bit彩色值
// -----------------------------------------------------------------
void gLine(u32 x0, u32 y0, u32 x1, u32 y1, u32 color)
{  
    int dx_s, dy_s;	  // sign of delta
    int dx, dy;		  // deferences in x, y
    int dx_x2, dy_x2;	  // double deferences
    int di;		  // 

    dx = x1-x0;			
    dy = y1-y0;

    if(dx>0){  
        dx_s = 1;		
    }
    else{  
        if(dx<0){  
            dx_s = -1;				
        }
        else{  // dx==0，vertical
            _LCD_VLine(x0, y0, y1, color);
            return;
        }
    }

    if(dy>0){
        dy_s = 1;
    }
    else{  
        if(dy<0){  
            dy_s = -1;
        }
        else{  	// dy==0，horizontal
            _LCD_HLine( y0,x0, x1, color);
            return;
        }
    }

    //LCD_ControlCache(LCD_CC_LOCK);              //
    
    //take abs of dx,dy
    dx = dx_s * dx;
    dy = dy_s * dy;
    dx_x2 = dx*2;
    dy_x2 = dy*2;

    //Bresenham line algorithm
    if(dx>=dy){					// dx>=dy，along x axis 
        di = dy_x2 - dx;
        while(x0!=x1){
            gPoint(x0, y0, color);
            x0 += dx_s;
            if(di<0){
                  di += dy_x2;			// 
            }
            else{
                  di += dy_x2 - dx_x2;
                  y0 += dy_s;
            }
        }
        gPoint(x0, y0, color);	  // last point
    }
    else{   			  // dx<dy，along y axis
        di = dx_x2 - dy;
        while(y0!=y1){
            gPoint(x0, y0, color);
            y0 += dy_s;
            if(di<0){
                  di += dx_x2;
            }
            else{
                  di += dx_x2 - dy_x2;
                  x0 += dx_s;
            }
        }
        gPoint(x0, y0, color);	// last point
    } 

    //LCD_ControlCache(LCD_CC_UNLOCK);
    
}

// -----------------------------------------------------------------
// gLineSegs : 在多个像素点之间画折线 
// 	input :
//    points: 指向一系列像素坐标点数组的指针，该数组结构为
//       {x0,y0, x1,y1, ...}
//    color : 32-bit彩色值
//    np : 坐标点个数
// -----------------------------------------------------------------
void gLineSegs(u32 const *points,u32 color,u32 np)
{
      u32 x0,y0,x1,y1,j;
      
      if(0==np) return;
      if(1==np){
            x0=*points++;
            y0=*points;
            gPoint(x0,y0,color);
            return;
      } 

      //LCD_ControlCache(LCD_CC_LOCK);              //
      
      x0=*points++;
      y0=*points++;
      for(j=1;j<np;j++){
            x1=*points++;
            y1=*points++;
            gLine(x0,y0,x1,y1,color);
            x0=x1;
            y0=y1;
      }
      
      //LCD_ControlCache(LCD_CC_UNLOCK);
}
// -----------------------------------------------------------------
// gRect : 画矩形 
// 	input :
//    x0, y0 : 矩形左上角像素点水平和竖直方向坐标，
//    x1, y1 : 矩形右下角像素点水平和竖直方向坐标，
//    color_index : 32-bit彩色值
// -----------------------------------------------------------------
void gRect(int x0,int y0,int x1,int y1,u32 color)
{ 
  _LCD_HLine(y0,x0,x1,color);
  _LCD_HLine(y1,x0,x1,color);
  _LCD_VLine(x0,y0,y1,color);
  _LCD_VLine(x1,y0,y1,color);
}
// -----------------------------------------------------------------
// gRectFill : 用指定颜色号填充一个矩形区域
// input : 
//    color : 32-bit color pattern
//    x0,y0 : upper-left corner of the rect  
//    x1,y1 : lower-right corner of the rect 
// -----------------------------------------------------------------
void gRectFill(int x0,int y0,int x1,int y1, u32 color)
{
    int k,j ;

    if(x1<x0){
        k=x1; x1=x0; x0=k;
    }
    
    if(y1<y0){
        k=y1;y1=y0;y0=k;
    }

    if(x0>MAX_X_1) x0=MAX_X_1;
    if(x1>MAX_X_1) x1=MAX_X_1;
    if(y0>MAX_Y_1) x0=MAX_Y_1;
    if(y0>MAX_Y_1) x0=MAX_Y_1;
    
#ifdef USE_FRAME_BUFFER
    //fill buffer
    for (j=y0;j<y1;j++){
        for (k=x0;k<x1;k++){
            *(dc.cache+(MAX_X*j)+k)=color;
        }
    }
#endif

_LCD_Rect_Fill(x0,y0,x1,y1,color);

return;
/*
    if(dc.cache_mode==LCD_CC_LOCK){
        if(dc.cache_x0>x0) dc.cache_x0=x0;
        if(dc.cache_y0>y0) dc.cache_y0=y0;
        if(dc.cache_x1<x1) dc.cache_x1=x1;
        if(dc.cache_y1<y1) dc.cache_y1=y1;
        return;
    }

    //LCD_Cache2LCD(x0,y, x1, y);
    LCD_FillRect(x0,y0,x1,y1,color);  //fill cache and LCD
*/
}
// -----------------------------------------------------------------
//	LCD copy : copy bitmaps from buffer to a rect area of LCD display 
//  input : 
//    pbuf : pointer to image buffer， 0x00bbggrr
//    x1,y1 : upper-left position of the rect of LCD display 
//    x2,y2 : lower-right conner of the rect of LCD display
// -----------------------------------------------------------------
//copy a rect area of frame buffer into LCD GRAM
void gRectCopy(u32 x1,u32 y1, u32 x2, u32 y2,u32 *pbuf)
{
	int k ;
 
	if(x2<x1){
		k=x2; x2=x1; x1=k;
	}
	
	if(y2<y1){
		k=y2;y2=y1;y1=k;
	}
    
        _LCD_Rect_Copy(x1,y1,x2,y2,pbuf);
        
}  
//copy rgb data from a buffer into a rect of LCD GRAM
//input : x1,y1,x2,y2 gives a window in LCD GRAM
//        pbuf stores raw rgb data to fill LCD windows.
//        Three bytes r,g,b for one pixel
void gRectCopyRGB( u32 x1,u32 y1, u32 x2, u32 y2, u8 *pbuf)
{
	int k ;
 
	if(x2<x1){
		k=x2; x2=x1; x1=k;
	}
	
	if(y2<y1){
		k=y2;y2=y1;y1=k;
	}
        
        ssd1297_rect_set( x1, y1, x2, y2);
        
        k=(x2-x1+1)*(y2-y1+1)*3;
        
	//write rgb buffer to a rect of LCD GRAM
        while(k--){
             write_data(*pbuf++);
	}

}  

// -----------------------------------------------------------------
// gTextOut : 图形方式文本输出，超出屏幕显示范围的部分被截掉
//    ch    : 文本字符串指针
//    x, y  : 文本显示位置左上角象素坐标
// -----------------------------------------------------------------
void gTextOut(u32 x, u32 y, char *ch )
{
    u8 *pFont;
    int code,m,j,k;
    int x0=x,sx,sy; 
    u8 bm;
    
    if(x>=MAX_X || y>=MAX_Y) return;

    sy=MAX_Y-y;
    if(sy>FONTYSIZE) sy=FONTYSIZE;
    k=0;
    while(ch[k]!=0){
        if(ch[k]>0xa0){	//hanzi
            code=((ch[k]-0xa1)*94)+(ch[k+1]-0xa1);
            k++;
            pFont=(u8*)(dc.pFont+ASCSIZE+(code*FONTYSIZE*2));
            for(m=0;m<sy;m++){
                bm=*pFont++;
                code=MAX_X*y;
                for(j=0x80,sx=0;j>0&&x<MAX_X;j=j>>1,sx++,x++){
#ifdef USE_FRAME_BUFFER
                    if((bm&j)!=0){
                        *(LCD_GUI+(code)+x)=dc.color;
                    }
                    else if(dc.textbkmode){
                        *(LCD_GUI+(code)+x)=dc.backcolor;
                    }
#else
                    if((bm&j)!=0){
                        gPoint(x,y,dc.color);
                    }
                    else if(dc.textbkmode){
                        gPoint(x,y,dc.backcolor);
                    }
#endif
                }
                bm=*pFont++;
                for(j=0x80;j>DBITTEST&&x<MAX_X;j=j>>1,sx++,x++){
#ifdef USE_FRAME_BUFFER
                    if((bm&j)!=0){
                        *(LCD_GUI+(code)+x)=dc.color;
                    }
                    else if(dc.textbkmode){
                        *(LCD_GUI+(code)+x)=dc.backcolor;
                    }
#else
                    if((bm&j)!=0){
                        gPoint(x,y,dc.color);
                    }
                    else if(dc.textbkmode){
                        gPoint(x,y,dc.backcolor);
                    }
#endif
                }
                y++;
                x-=sx;	//12
                if(y>MAX_Y_1) break;  //??
            }
            x+=sx;	//12;
        }
        else{
            code=ch[k]-0x20;
            if(code<0 || code >95) code=0;
            pFont=(u8*)(dc.pFont+(code*ASCBYTES));
            for(m=0;m<sy;m++){
                bm=*pFont++;
#ifdef LCD_FONT12                                
                pFont++;
#endif
                code=MAX_X*y;
                for(j=0x80,sx=0;j>BITTEST&&x<MAX_X;j=j>>1,sx++,x++){ 
#ifdef USE_FRAME_BUFFER
                    if(((bm)&j)!=0){
                        *(LCD_GUI+(code)+x)=dc.color;
                    }	
                    else if(dc.textbkmode){
                        *(LCD_GUI+(code)+x)=dc.backcolor;
                    }
#else
                    if((bm&j)!=0){
                        gPoint(x,y,dc.color);
                    }
                    else if(dc.textbkmode){
                        gPoint(x,y,dc.backcolor);
                    }
#endif
                }
                y++;
                x-=sx;	//6;
                if(y>MAX_Y_1) break;  //??
            }
            x+=sx;	//6;
        }
        y-=sy;	//12;
        k+=1;
        if(x>=MAX_X) break;
    }

#ifdef USE_FRAME_BUFFER
    _LCD_Rect_Copy( x0,y,x-1,y+sy-1,(u32*)LCD_GUI);
#endif
}	
// -----------------------------------------------------------------
// gTextOut2 : 用两倍字体大小图形方式文本输出，超出屏幕显示范围的部分被截掉
//    ch    : 文本字符串指针
//    x, y  : 文本显示位置左上角象素坐标
// -----------------------------------------------------------------
void gTextOut2(u32 x, u32 y, char *ch )
{
    u8 *pFont;
    int code,m,j,k;
    int x0=x,sx,sy; 
    u8 bm;
    
    if(x>=MAX_X || y>=MAX_Y) return;

    sy=MAX_Y-y;
    if(sy>(FONTYSIZE*2)) sy=FONTYSIZE*2;  //!!
    k=0;
    while(ch[k]!=0){
        if(ch[k]>0xa0){	//hanzi
            code=((ch[k]-0xa1)*94)+(ch[k+1]-0xa1);
            k++;
            pFont=(u8*)(dc.pFont+ASCSIZE+(code*FONTYSIZE*2));
            for(m=0;m<sy;m+=2){
                bm=*pFont++;
                code=MAX_X*y;
                for(j=0x80,sx=0;j>0&&x<MAX_X;j=j>>1,sx+=2,x+=2){
                    if((bm&j)!=0){
                        gPoint(x,y,dc.color);
                        gPoint(x+1,y,dc.color);
                        gPoint(x,y+1,dc.color);
                        gPoint(x+1,y+1,dc.color);
                    }
                    else if(dc.textbkmode){
                        gPoint(x,y,dc.backcolor);
                        gPoint(x+1,y,dc.backcolor);
                        gPoint(x,y+1,dc.backcolor);
                        gPoint(x+1,y+1,dc.backcolor);
                    }
                }
                bm=*pFont++;
                for(j=0x80;j>DBITTEST&&x<MAX_X;j=j>>1,sx+=2,x+=2){
                    if((bm&j)!=0){
                        gPoint(x,y,dc.color);
                        gPoint(x+1,y,dc.color);
                        gPoint(x,y+1,dc.color);
                        gPoint(x+1,y+1,dc.color);
                    }
                    else if(dc.textbkmode){
                        gPoint(x,y,dc.backcolor);
                        gPoint(x+1,y,dc.backcolor);
                        gPoint(x,y+1,dc.backcolor);
                        gPoint(x+1,y+1,dc.backcolor);
                    }
                }
                y+=2;
                x-=sx;	//12
                if(y>MAX_Y_1) break;  //??
            }
            x+=sx;	//12;
        }
        else{
            code=ch[k]-0x20;
            if(code<0 || code >95) code=0;
            pFont=(u8*)(dc.pFont+(code*ASCBYTES));
            for(m=0;m<sy;m+=2){
                bm=*pFont++;
#ifdef LCD_FONT12                                
                pFont++;
#endif
                code=MAX_X*y;
                for(j=0x80,sx=0;j>BITTEST&&x<MAX_X;j=j>>1,sx+=2,x+=2){ 
                    if((bm&j)!=0){
                        gPoint(x,y,dc.color);
                        gPoint(x+1,y,dc.color);
                        gPoint(x,y+1,dc.color);
                        gPoint(x+1,y+1,dc.color);
                    }
                    else if(dc.textbkmode){
                        gPoint(x,y,dc.backcolor);
                        gPoint(x+1,y,dc.backcolor);
                        gPoint(x,y+1,dc.backcolor);
                        gPoint(x+1,y+1,dc.backcolor);
                    }
                }
                y+=2;
                x-=sx;	//6;
                if(y>MAX_Y_1) break;  //??
            }
            x+=sx;	//6;
        }
        y-=sy;	//12;
        k+=1;
        if(x>=MAX_X) break;
    }

}	
// -----------------------------------------------------------------
// gFormatTextOut : 图形方式格式化文本输出，超出屏幕显示范围的部分被截掉
//    x, y  : 文本显示位置左上角象素坐标
//    format,...    : 可变数量的输入变量，格式同C语言库函数printf()
// -----------------------------------------------------------------
void gFormatTextOut(u32 x, u32 y, char* format,...)
{
    int n;
    va_list ap;
    va_start(ap, format);
    n = vsprintf(lcdpch,format,ap);

    if(!n) return;
    gTextOut(x,y,lcdpch);
    va_end(ap);
}
	
// -----------------------------------------------------------------
// gFormatTextOut2 : 用两倍字体大小图形方式格式化文本输出，超出屏幕显示范围的部分被截掉
//    x, y  : 文本显示位置左上角象素坐标
//    format,...    : 可变数量的输入变量，格式同C语言库函数printf()
// -----------------------------------------------------------------
void gFormatTextOut2(u32 x, u32 y, char* format,...)
{
    int n;
    va_list ap;
    va_start(ap, format);
    n = vsprintf(lcdpch,format,ap);

    if(!n) return;
    gTextOut2(x,y,lcdpch);
    va_end(ap);
}
	

// -----------------------------------------------------------------
// gEllipse : 画椭圆 
// 	input :
//    x0, y0 : 椭圆外切矩形左上角像素点水平和竖直方向坐标，
//    x1, y1 : 椭圆外切矩形右下角像素点水平和竖直方向坐标，
//    color : 32-bit彩色值
// -----------------------------------------------------------------
void gEllipse(u32 x0, u32 y0,u32 x1,u32 y1,u32 color)
{  
	_Ellipse(x0,y0,x1,y1,color,0);
}

// -----------------------------------------------------------------
// gEllipseFill : 画填充的椭圆 
// 	input :
//    x0, y0 : 椭圆外切矩形左上角像素点水平和竖直方向坐标，
//    x1, y1 : 椭圆外切矩形右下角像素点水平和竖直方向坐标，
//    color : 32-bit彩色值
// -----------------------------------------------------------------
void gEllipseFill(u32 x0, u32 y0, u32 x1, u32 y1, u32 color)
{
	_Ellipse(x0,y0,x1,y1,color,1);
}

// -----------------------------------------------------------------
// gCircle : 画圆 
// 	input :
//    x0, y0 : 圆心点水平和竖直方向像素坐标，
//    r :      圆半径
//    color : 32-bit彩色值
// -----------------------------------------------------------------
void gCircle(u32 x0, u32 y0, u32 r, u32 color)
{  
    _Circle(x0,y0,r,color,0);
}

// -----------------------------------------------------------------
// gCircleFill : 画填充圆 
// 	input :
//    x0, y0 : 圆心点水平和竖直方向像素坐标，
//    r :      圆半径
//    color : 32-bit彩色值
// -----------------------------------------------------------------
void gCircleFill(u32 x0, u32 y0, u32 r, u32 color)
{
	_Circle(x0,y0,r,color,1);
}
/*
// -----------------------------------------------------------------
// 设定使用默认的调色板
//  out : 默认调色板数据指针
// -----------------------------------------------------------------
u16* LCD_SetDefPalette(void)
{
//	int i,n;
	u16* oldlut=dc.lut;
	
	dc.lut=DefaultLut;//GLut;	
	return oldlut;
}
// -----------------------------------------------------------------
// 设定使用用户自定义调色板
//  in  : lut, 要使用的新调色板数据指针, 指向256元素的u16数组 
//  out : 旧的调色板数据指针
// -----------------------------------------------------------------
u16* LCD_SetPalette(u16* lut)
{
	u16 *oldlut=dc.lut;
	dc.lut=lut;
	return oldlut;
}
*/
// -----------------------------------------------------------------
// 根据给定RGB值形成一个16位彩色
//    in  : r, g, b : 0-255
//    out : 32-bit color : BBBBBBxxGGGGGGxxRRRRRRxx
// -----------------------------------------------------------------
u32 RGB2Int(u8 r,u8 g,u8 b)
{
//RGB565 : RRRrrGGGgggBBbb
//	r=r>>3;	//5bit
//	g=g>>2;	//6bit
//	b=b>>3;	//5bit
//	return (u16)((b)+(g<<5)+(r<<11)); b
  return (b<<16)|(g<<8)|r;
}




// -----------------------------------------------------------------
//  以下为图形库内部调用的函数
// -----------------------------------------------------------------



// -----------------------------------------------------------------
//	LCD_Point : draw a point on LCD
//  input : 
//    x : horizontal pixel pos, 0-239 (no checking)
//    y : vertical pixel pos, 0-319   (no checking)
//    color_index : color index to a color LUT 
// -----------------------------------------------------------------
void _LCD_Point(u32 x,u32 y,u32 color)
{   
    ssd1297_addr_set( x, y);
    write_data((u8)color);
    write_data((u8)(color>>8));
    write_data((u8)(color>>16));
  
}
// -----------------------------------------------------------------
//	LCD_Cache2LCD : copy an image block from grawing cache to LCD 
//  input : 
//    x0,y0 : upper-left corner of the block  (no checking!)
//    x1,y1 : lower-right corner of the block (no checking!)
// -----------------------------------------------------------------
int LCD_Cache2LCD(u32 x0,u32 y0,u32 x1,u32 y1)
{
    u32 n,j,k;//,p1;
    u32 p1;

    //修改窗口	
    ssd1297_rect_set( x0, y0, x1, y1);

    //fill buffer
    n=y1*MAX_X;
    for (j=(y0*MAX_X);j<=n;j+=MAX_X){
        for (k=x0;k<=x1;k++){
            p1=*(LCD_GUI+(j)+k);
            write_data((u8)p1);
            write_data((u8)(p1>>8));
            write_data((u8)(p1>>16));
        }
    }

    return 1;
}  
// -----------------------------------------------------------------
//	LCD_ControlCache : drawing cache control 
//  input : 
//    mode : cache mode as follows.
// decleared in LCD.h :
// #define LCD_CC_UNLOCK (0)    // Default mode: Cache is transparent 
// #define LCD_CC_LOCK   (1)    // Cache is locked, no write operations 
// #define LCD_CC_FLUSH  (2)    // Flush cache, do not change mode 
// -----------------------------------------------------------------
#ifdef USE_FRAME_BUFFER

u32 LCD_ControlCache(u32 mode)
{
	if(mode==LCD_CC_LOCK){
		if(dc.cache_mode==LCD_CC_UNLOCK){
			dc.cache_x0=MAX_X;
			dc.cache_y0=MAX_Y;
			dc.cache_x1=0;
			dc.cache_y1=0;
		}
		dc.cache_lock_cnt++;
		dc.cache_mode=LCD_CC_LOCK;
	}
	if(mode==LCD_CC_UNLOCK){
		if(dc.cache_mode==LCD_CC_UNLOCK){
			return LCD_CC_UNLOCK;
		}
		if(!(--dc.cache_lock_cnt)){
			LCD_FlushCache();
			dc.cache_mode=LCD_CC_UNLOCK;
			dc.cache_x0=MAX_X;
			dc.cache_y0=MAX_Y;
			dc.cache_x1=0;
			dc.cache_y1=0;
		}
	}
	if(mode==LCD_CC_FLUSH){
		LCD_FlushCache();
		dc.cache_x0=MAX_X;
		dc.cache_y0=MAX_Y;
		dc.cache_x1=0;
		dc.cache_y1=0;
	}
	return dc.cache_mode;
}
// -----------------------------------------------------------------
// LCD_FlushCache: flushing drawing cache to LCD
// and reset the invalid region
// -----------------------------------------------------------------
int LCD_FlushCache(void)
{
	u32 xs,xe,ys,ye,k;//,p1;
	int n,j ;
	u32 p1;
 
	if(dc.cache_x0>dc.cache_x1){
              return 0;
	}
	if(dc.cache_y0>dc.cache_y1){
              return 0;
	}
	xs=dc.cache_x0;xe=dc.cache_x1;
	ys=dc.cache_y0;ye=dc.cache_y1;
		 
	//修改窗口	
        ssd1297_rect_set( xs, ys, xe, ye);

	//fill buffer
	n=dc.cache_y1*MAX_X;
	for (j=(dc.cache_y0*MAX_X);j<=n;j+=MAX_X){
            for (k=dc.cache_x0;k<=dc.cache_x1;k++){
                p1=*(LCD_GUI+(j)+k);
                write_data((u8)p1);
                write_data((u8)(p1>>8));
                write_data((u8)(p1>>16));
            }
	}
    
	return 1;

}  	
#endif

// -----------------------------------------------------------------
// _Ellipse : supporting functions : draw an ellipse w or w/o fill
// -----------------------------------------------------------------
void _Ellipse(u32 x0, u32 y0, u32 x1, u32 y1, u32 color, u32 fill)
{  
    int  wx0, wy0, wx1, wy1, wx2, wy2, wx3, wy3;
    int xx, yy, di;			// control
    int cx, cy;				// center
    int rx, ry;				// radius
    int  rxx, ryy;			// squire radius
    int  rxx2, ryy2;		// double squire radius
    u32 t;  //cache_x0,cache_y0,cache_x1,cache_y1,

    if( (x0==x1) || (y0==y1) ) return;

    //LCD_ControlCache(LCD_CC_LOCK);

    if(x0>x1){
        t=x0;x0=x1;x1=t;
    }
    if(y0>y1){
        t=y0;y0=y1;y1=t;
    }
   	//
        /*
	cache_x0=x0;
	cache_x1=x1;
	if(cache_x1>=MAX_X) cache_x1=MAX_X-1;
	cache_y0=y0;
	cache_y1=y1;
	if(cache_y1>=MAX_Y) cache_y1=MAX_Y-1;

	if(dc.cache_mode==LCD_CC_LOCK){
		if(dc.cache_x0>cache_x0) dc.cache_x0=cache_x0;
		if(dc.cache_x1<cache_x1) dc.cache_x1=cache_x1;
		if(dc.cache_y0>cache_y0) dc.cache_y0=cache_y0;
		if(dc.cache_y1<cache_y1) dc.cache_y1=cache_y1;
	}
        */
   
    cx = (x0 + x1) >> 1;		//center		
    cy = (y0 + y1) >> 1;

    //radius
    if(x0 > x1){
            rx = (x0 - x1) >> 1;
    }
    else{  
            rx = (x1 - x0) >> 1;
    }
    if(y0 > y1){  
            ry = (y0 - y1) >> 1;
    }
    else{  
            ry = (y1 - y0) >> 1;
    }
    // squire radius
    rxx = rx * rx;
    ryy = ry * ry;
    //double
    rxx2 = rxx<<1;
    ryy2 = ryy<<1;

    xx = 0;
    yy = ry;
    di = ryy2 + rxx - rxx2*ry ;	
    
    // intersects on y axis
    wx0 = wx1 = wx2 = wx3 = cx;
    wy0 = wy1 = cy + ry;
    wy2 = wy3 = cy - ry;

    // draw the intersects on y axis
    gPoint(wx0, wy0, color);
    gPoint(wx2, wy2, color);
	
    while( (ryy*xx) < (rxx*yy) ){  
        if(di<0){  
            di+= ryy2*(2*xx+3);
        }
        else{
            di += ryy2*(2*xx+3) + 4*rxx - 4*rxx*yy;
            yy--;
            wy0--; wy1--; wy2++; wy3++;				 
        }
        xx++;	
        wx0++; wx1--; wx2++; wx3--;
        
        gPoint(wx0, wy0, color);
        gPoint(wx1, wy1, color);
        gPoint(wx2, wy2, color);
        gPoint(wx3, wy3, color);
  
        if(fill){	// fill 
            if(di>=0){
                _LCD_HLine( wy0,wx0, wx1, color);
                _LCD_HLine( wy2,wx2, wx3, color);
            }
        }
            //
    } //end while
  
    di = rxx2*(yy-1)*(yy-1) + ryy2*xx*xx + ryy + ryy2*xx - rxx2*ryy;
    while(yy>=0){ 
        if(di<0){
            di+= rxx2*3 + 4*ryy*xx + 4*ryy - 2*rxx2*yy;
            xx ++;						
            wx0++; wx1--; wx2++; wx3--;  
        }
        else{
            di += rxx2*3 - 2*rxx2*yy;	 	 		     			 
        }
        yy--;
        wy0--; wy1--; wy2++; wy3++;	
        
        gPoint(wx0, wy0, color);
        gPoint(wx1, wy1, color);
        gPoint(wx2, wy2, color);
        gPoint(wx3, wy3, color);
  
        if(fill){	// fill
            _LCD_HLine(wy0,wx0, wx1, color);
            _LCD_HLine(wy2,wx2, wx3, color); 
        }//
    } //end while

    //LCD_ControlCache(LCD_CC_UNLOCK);

//	if(dc.cache_mode!=LCD_CC_LOCK){
//		LCD_Cache2LCD(cache_x0,cache_y0,cache_x1,cache_y1);
//	}
	    
}
// -----------------------------------------------------------------
// _Circle : supporting functions : draw a circle w or w/o fill
// -----------------------------------------------------------------
void _Circle(u32 x0, u32 y0, u32 r, u32 color,u32 fill)
{  
    int wx0, wy0, wx1,wy1, wx2,wy2,wx3, wy3;	
    int wx4, wy4, wx5,wy5, wx6,wy6, wx7,wy7;	
    int fx0, fx1, fy0;			// fill
    int xx, yy, di;				// control
    //int cache_x0,cache_y0,cache_x1,cache_y1;

    if(0==r) return;
        
        
    //LCD_ControlCache(LCD_CC_LOCK);
   
    // draw 4 special points
    wx0 = wx1 = x0;
    wy0 = wy1 = y0 + r;
    if(wy0<MAX_Y){
        gPoint(wx0, wy0, color);		// 90 degree 
    }
    
    wx2 = wx3 = x0;
    wy2 = wy3 = y0 - r;
    if(wy2>=0){
        gPoint(wx2, wy2, color);		// 270 degree
    }
    
    wx4 = wx6 = x0 + r;
    wy4 = wy6 = y0;
    if(wx4<MAX_X){ 
        gPoint(wx4, wy4, color);		// 0 degree
        fx1 = wx4;
    }
    else{
        fx1 = MAX_X-1;
    }
    if(fill){
        fy0 = y0;				// fill start
        fx0 = x0 - r;				// fill end
        if(fx0<0) fx0 = 0;
        _LCD_HLine(fy0, fx0, fx1, color);
    }
    
    wx5 = wx7 = x0 - r;
    wy5 = wy7 = y0;
    if(wx5>=0){  
            gPoint(wx5, wy5, color);		// 180 degree
    }
      
      //
      /*
      cache_x0=(wx5<0)?0:wx5;
      cache_x1=(wx4>=MAX_X)?(MAX_X-1):wx4;
      cache_y0=(wy2<0)?0:wy2;
      cache_y1=(wy0>=MAX_Y)?(MAX_Y-1):wy0;

      if(dc.cache_mode==LCD_CC_LOCK){
              if(dc.cache_x0>cache_x0) dc.cache_x0=cache_x0;
              if(dc.cache_x1<cache_x1) dc.cache_x1=cache_x1;
              if(dc.cache_y0>cache_y0) dc.cache_y0=cache_y0;
              if(dc.cache_y1<cache_y1) dc.cache_y1=cache_y1;
      }
      */
        
    if(1==r){
		//if(dc.cache_mode!=LCD_CC_LOCK){
		//	LCD_Cache2LCD(cache_x0,cache_y0,cache_x1,cache_y1);
		//}
       //LCD_ControlCache(LCD_CC_UNLOCK);
       return;
    }
   
   
    //Bresenham algorithm
    di = 3 - 2*r;							
    xx = 0;
    yy = r;
    while(xx<yy){  
        if(di<0){
            di += 4*xx + 6;
        }
        else{
            di += 4*(xx - yy) + 10;
            yy--;	  
            wy0--; 
            wy1--;
            wy2++;
            wy3++;
            wx4--;
            wx5++;
            wx6--;
            wx7++;		 
        }
        xx++;   
        wx0++;
        wx1--;
        wx2++;
        wx3--;
        wy4++;
        wy5++;
        wy6--;
        wy7--;
		
        if((wx0<MAX_X)&&(wy0>=0)){
            gPoint(wx0, wy0, color);
        }	    
        if((wx1>=0)&&(wy1>=0)){	
            gPoint(wx1, wy1, color);
        }
  
        if(fill){	//H line filling , lower half
            if(wx1>=0){
                  fx0 = wx1;	// fill start fx0
                  fy0 = wy1;	// fill start fy0
                  if(fy0>=MAX_Y) fy0 = MAX_Y-1;
                  if(fy0<0) fy0 = 0; 
                  fx1 = x0*2 - wx1;	// fill end point				
                  if(fx1>=MAX_X) fx1 = MAX_X-1;
                  _LCD_HLine(fy0, fx0, fx1, color);
            }
      	}
	  
        if((wx2<MAX_X)&&(wy2<MAX_Y)){	
            gPoint(wx2, wy2, color);   
        }
          
        if((wx3>=0)&&(wy3<MAX_Y)){
            gPoint(wx3, wy3, color);
        }
        
        if(fill){	// V line fill	  
            if(wx3>=0){
                fx0 = wx3;		// start p fx0
                fy0 = wy3;		// start p fy0
                if(fy0>=MAX_Y) fy0 = MAX_Y-1;
                if(fy0<0) fy0 = 0;
                fx1 = x0*2 - wx3;	//end p fx1				
                if(fx1>=MAX_X) fx1 = MAX_X-1;
                _LCD_HLine(fy0, fx0, fx1, color);
            }
      	}//

        if((wx4<MAX_X)&&(wy4>=0)){
            gPoint(wx4, wy4, color);
        }
        if((wx5>=0)&&(wy5>=0)){
            gPoint(wx5, wy5, color);
        }
  
        if(fill){	// v line fill, upper half
            if(wx5>=0){
                fx0 = wx5;		// start p fx0
                fy0 = wy5;		// start p fy0
                if(fy0>=MAX_Y) fy0 = MAX_Y-1;
                if(fy0<0) fy0 = 0;
                fx1 = x0*2 - wx5;	//end p fx1				
                if(fx1>=MAX_X) fx1 = MAX_X-1;
                _LCD_HLine(fy0, fx0, fx1, color);
            }
      	}//
      	
        if((wx6<MAX_X)&&(wy6<MAX_Y)){
            gPoint(wx6, wy6, color);
        }
  
        if((wx7>=0)&&(wy7<MAX_Y)){
            gPoint(wx7, wy7, color);
        }
  
        if(fill){	// v line fill
            if(wx7>=0){
                fx0 = wx7;		//start p fx0
                fy0 = wy7;		//start p fy0
                if(fy0>=MAX_Y) fy0 = MAX_Y-1;
                if(fy0<0) fy0 = 0;
                fx1 = x0*2 - wx7;	//end p fx1			
                if(fx1>=MAX_X) fx1 = MAX_X-1;
                _LCD_HLine(fy0, fx0, fx1, color);
            }
        }//
	  
    } //end while(xx<yy)

    //LCD_ControlCache(LCD_CC_UNLOCK);
//	if(dc.cache_mode!=LCD_CC_LOCK){
//		LCD_Cache2LCD(cache_x0,cache_y0,cache_x1,cache_y1);
//	}
}
// -----------------------------------------------------------------
// _HLine : supporting functions : draw horizontal line 
// -----------------------------------------------------------------
void _LCD_HLine(int y, int x0, int x1, u32 color)
{  
      int i;
      volatile u32 *p;

      if(x0>x1){
          i=x0;x0=x1;x1=i;
      }
      i=x1-x0+1;

#ifdef USE_FRAME_BUFFER
      p=dc.cache+(MAX_X*y)+x0;

      //write to buffer
      while(i--){
          *p++=color;
      }
#endif
      
_LCD_Rect_Fill(x0,y,x1,y,color);
return;
      
//      if(dc.cache_mode==LCD_CC_LOCK){
//            if(dc.cache_x0>x0) dc.cache_x0=x0;
//            if(dc.cache_y0>y) dc.cache_y0=y;
//            if(dc.cache_x1<x1) dc.cache_x1=x1;
//            if(dc.cache_y1<y) dc.cache_y1=y;
//            return;
//      }
//
//      LCD_Cache2LCD(x0,y, x1, y);
}
// -----------------------------------------------------------------
// _XorHLine : supporting functions : draw horizontal line using XOR mode
// -----------------------------------------------------------------
/*
void _XorHLine(int y, int x0, int x1,u32 max_colors)
{  
    u32 PixelIndex;
    int i;
    u32 *p;
//	u8 p1=(u8)color_index;
    
    if(x0>x1){
        i=x0;x0=x1;x1=i;
    }
    i=x1-x0;
    
    p=dc.cache+(MAX_X*y)+x0;

    //write to buffer 
    do{
        PixelIndex=*p;
        *p++=(u8)(max_colors - PixelIndex - 1);
    }while(i--);
    
    if(dc.cache_mode==LCD_CC_LOCK){
        if(dc.cache_x0>x0) dc.cache_x0=x0;
        if(dc.cache_y0>y) dc.cache_y0=y;
        if(dc.cache_x1<x1) dc.cache_x1=x1;
        if(dc.cache_y1<y) dc.cache_y1=y;
        return;
    }

    LCD_Cache2LCD(x0,y, x1, y);
}
*/
// -----------------------------------------------------------------
// _VLine : supporting functions : draw vertical line
// -----------------------------------------------------------------
void _LCD_VLine(int x, int y0, int y1, u32 color)
{  
    
    int i;

    if(y0>y1){
        i=y0;y0=y1;y1=i;
    }

#ifdef USE_FRAME_BUFFER
    {
    volatile u32 *p;

    i=y1-y0+1;

    p=dc.cache+(MAX_X*y0)+x;

    //write to buffer 
    while(i--){
        *p=color;
        p+=MAX_X;
    }
    }
#endif
    
_LCD_Rect_Fill(x,y0,x,y1,color);

return;
    
//    if(dc.cache_mode==LCD_CC_LOCK){
//        if(dc.cache_x0>x) dc.cache_x0=x;
//        if(dc.cache_y0>y0) dc.cache_y0=y0;
//        if(dc.cache_x1<x) dc.cache_x1=x;
//        if(dc.cache_y1<y1) dc.cache_y1=y1;
//        return;
//    }
//
//    LCD_Cache2LCD(x,y0, x, y1);
}

// -----------------------------------------------------------------
// _XorVLine : supporting functions : draw vertical line using XOR mode
// -----------------------------------------------------------------
/*
void _XorVLine(int x, int y0, int y1, u32  max_colors)
{  
    int i;
    u32 *p;
    u32 PixelIndex;
    
    
    if(y0>y1){
        i=y0;y0=y1;y1=i;
    }
    i=y1-y0;

    p=dc.cache+MAX_X*y1;

    //write to buffer 
    do{
        PixelIndex=*p;
        *p=(u8)(max_colors - PixelIndex - 1);
        p+=MAX_X;
    }while(i--);

    if(dc.cache_mode==LCD_CC_LOCK){
        if(dc.cache_x0>x) dc.cache_x0=x;
        if(dc.cache_y0>y0) dc.cache_y0=y0;
        if(dc.cache_x1<x) dc.cache_x1=x;
        if(dc.cache_y1<y1) dc.cache_y1=y1;
        return;
    }

    LCD_Cache2LCD(x,y0, x, y1);
}

*/
// -----------------------------------------------------------------
// 默认调色板
// -----------------------------------------------------------------
/*
u16 DefaultLut[256]={
0x0000,0xf800,0x07e0,0x001f,0xffe0,0x07ff,0xf81f,0xffff,0x18e3,0x39e7,0x5aeb,0x7bef,0x9cf3,0xbdf7,0xdefb,0xffff,
0x18a2,0x3965,0x5a28,0x5965,0x7aeb,0x79e7,0x9bae,0x9a69,0x9924,0xbc71,0xbaeb,0xb965,0xdd34,0xdb6d,0xd9a6,0xd800,
0xfdf7,0xfbef,0xf9e7,0xf800,0x18c2,0x39a5,0x5a88,0x5a25,0x7b6b,0x7ae7,0x9c4e,0x9ba9,0x9b04,0xbd31,0xbc6b,0xbba5,
0xde14,0xdd2d,0xdc46,0xdb60,0xfef7,0xfdef,0xfce7,0xfbe0,0x18e2,0x39e5,0x5ae8,0x5ae5,0x7beb,0x7be7,0x9cee,0x9ce9,
0x9ce4,0xbdf1,0xbdeb,0xbde5,0xdef4,0xdeed,0xdee6,0xdee0,0xfff7,0xffef,0xffe7,0xffe0,0x18e2,0x31e5,0x52e8,0x42e5,
0x6beb,0x5be7,0x8cee,0x74e9,0x64e4,0xa5f1,0x8deb,0x75e5,0xc6f4,0xa6ed,0x8ee6,0x6ee0,0xdff7,0xbfef,0x9fe7,0x7fe0,
0x10e2,0x29e5,0x42e8,0x2ae5,0x5beb,0x3be7,0x74ee,0x4ce9,0x24e4,0x8df1,0x5deb,0x2de5,0xa6f4,0x6eed,0x36e6,0x06e0,
0xbff7,0x7fef,0x3fe7,0x07e0,0x10e3,0x29e6,0x42ea,0x2ae8,0x5bed,0x3beb,0x74f1,0x4cee,0x24ec,0x8df4,0x5df1,0x2dee,
0xa6f8,0x6ef4,0x36f1,0x06ed,0xbffb,0x7ff7,0x3ff3,0x07ef,0x10e3,0x29e7,0x42eb,0x2aeb,0x5bef,0x3bef,0x74f3,0x4cf3,
0x24f3,0x8df7,0x5df7,0x2df7,0xa6fb,0x6efb,0x36fb,0x06fb,0xbfff,0x7fff,0x3fff,0x07ff,0x10c3,0x29a7,0x428b,0x2a2b,
0x5b6f,0x3aef,0x7453,0x4bb3,0x2313,0x8d37,0x5c77,0x2bb7,0xa61b,0x6d3b,0x345b,0x037b,0xbeff,0x7dff,0x3cff,0x03ff,
0x10a3,0x2967,0x422b,0x296b,0x5aef,0x39ef,0x73b3,0x4a73,0x2133,0x8c77,0x5af7,0x2977,0xa53b,0x6b7b,0x31bb,0x001b,
0xbdff,0x7bff,0x39ff,0x001f,0x18a3,0x3167,0x522b,0x416b,0x6aef,0x59ef,0x8bb3,0x7273,0x6133,0xa477,0x8af7,0x7177,
0xc53b,0xa37b,0x89bb,0x681b,0xddff,0xbbff,0x99ff,0x781f,0x18a3,0x3967,0x5a2b,0x596b,0x7aef,0x79ef,0x9bb3,0x9a73,
0x9933,0xbc77,0xbaf7,0xb977,0xdd3b,0xdb7b,0xd9bb,0xd81b,0xfdff,0xfbff,0xf9ff,0xf81f,0x18a3,0x3966,0x5a2a,0x5968,
0x7aed,0x79eb,0x9bb1,0x9a6e,0x992c,0xbc74,0xbaf1,0xb96e,0xdd38,0xdb74,0xd9b1,0xd80d,0xfdfb,0xfbf7,0xf9f3,0xf80f
};
*/
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

inline u8 read_data(void)
{
  return LCD_DATA;	
}

u16 read_reg(void)
{
  u16 t;
  t=(LCD_DATA&0xff)<<8;
  t|=(LCD_DATA&0xff);
  return t;
}


void _LCD_Rect_Fill(u32 x1,u32 y1,u32 x2,u32 y2,u32 color)
{
    u32 i;
    ssd1297_rect_set(x1,y1,x2,y2);
    i=(x2-x1+1)*(y2-y1+1);
    while(i--){
        write_data((u8)color);         //R
        write_data((u8)(color>>8));    //G
        write_data((u8)(color>>16));   //B
    }
    ssd1297_rect_set(0,0,MAX_X_1,MAX_Y_1);
}

// -----------------------------------------------------------------
//	LCD copy : copy bitmaps from buffer to a rect area of LCD display 
//  input : 
//    pbuf : pointer to image buffer
//    x0,y0 : upper-left position of the rect of LCD display 
//    x1,y1 : lower-right conner of the rect of LCD display
// -----------------------------------------------------------------
//copy a rect area of frame buffer into LCD GRAM
void _LCD_Rect_Copy( u32 x1,u32 y1, u32 x2, u32 y2, u32 *pbuf)
{
	int n,j,k ;
	int p1;
 
	if(x2<x1){
		k=x2; x2=x1; x1=k;
	}
	
	if(y2<y1){
		k=y2;y2=y1;y1=k;
	}
        
        ssd1297_rect_set( x1, y1, x2, y2);
        
	//fill buffer
	n=y2*MAX_X;
	for (j=(y1*MAX_X);j<=n;j+=MAX_X){
            for (k=x1;k<=x2;k++){
                p1=*(pbuf+(j)+k);
                //p1=*(pbuf+(j)+k);
                write_data((u8)p1);
                write_data((u8)(p1>>8));
                write_data((u8)(p1>>16));
            }
	}

}  


//set GRAM window and AC address
void ssd1297_rect_set(int xs, int ys, int xe,int ye)
{
#ifdef LCD_VERTICAL
    write_cmd(0x45);				// Horizontal RAM Start ADDR 
    write_reg(xs);
    write_cmd(0x46);				// Horizontal RAM End ADDR 
    write_reg(xe);
  
    write_cmd(0x44);				// Vertical Start,End ADDR 
    write_reg(((ye)<<8)|(ys));
  
    write_cmd(0x4F);				// GRAM Vertical/Horizontal ADDR Set(AD0~AD7) 
    write_reg(xs);
    write_cmd(0x4E);				// GRAM Vertical/Horizontal ADDR Set(AD8~AD16) 
    write_reg(ys);
#else
    write_cmd(0x45);				// Horizontal RAM Start ADDR 
    write_reg(ys);
    write_cmd(0x46);				// Horizontal RAM End ADDR 
    write_reg(ye);
  
    write_cmd(0x44);				// Vertical Start,End ADDR 
    write_reg(((xe)<<8)|(xs));
  
    write_cmd(0x4F);				// GRAM Vertical/Horizontal ADDR Set(AD0~AD7) 
    write_reg(ys);
    write_cmd(0x4E);				// GRAM Vertical/Horizontal ADDR Set(AD8~AD16) 
    write_reg(xs);
#endif    
  
    write_cmd(0x22);				// Write Data to GRAM 

}
//
void ssd1297_addr_set(int xs, int ys)
{
#ifdef LCD_VERTICAL
    write_cmd(0x4F);				// GRAM Vertical/Horizontal ADDR Set(AD0~AD7) 
    write_reg(xs);
    write_cmd(0x4E);				// GRAM Vertical/Horizontal ADDR Set(AD8~AD16) 
    write_reg(ys);
#else
    write_cmd(0x4F);				// GRAM Vertical/Horizontal ADDR Set(AD0~AD7) 
    write_reg(ys);
    write_cmd(0x4E);				// GRAM Vertical/Horizontal ADDR Set(AD8~AD16) 
    write_reg(xs);
#endif    
    write_cmd(0x22);				// Write Data to GRAM 
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
     
}

// -----------------------------------------------------------------
// _TextOut: internal function supporting LCD_Printf
// 	   drawing a text string ch, at pixel pos x,y
// -----------------------------------------------------------------
void _TextOut(u8 *ch,int x, int y)
{
    u8 *pFont;
    int code,m,j,k;
    u8 bm;
//    int dx,dy,xs,ys,xe,ye;
    
//    dx=dy=0;

    k=0;
    while(ch[k]!=0){
        if(ch[k]>0xa0){	//hanzi
            code=((ch[k]-0xa1)*94)+(ch[k+1]-0xa1);
            k++;
            pFont=(u8*)(dc.pFont+ASCSIZE+(code*FONTYSIZE*2));
            for(m=0;m<FONTYSIZE;m++){
                bm=*pFont++;
                code=MAX_X*y;
                //code=(FONTXSIZE*2)*dy;
                for(j=0x80;j>0;j=j>>1){
                    //if(((*pFont)&j)!=0){
                    if((bm&j)!=0){
                        gPoint(x,y,dc.color);
                        //*(pBuf+(code)+x)=dc.color;
                    }
                    else if(dc.textbkmode){
//                    else{
                        gPoint(x,y,dc.backcolor);
                        //*(pBuf+(code)+x)=dc.backcolor;
                    }
                    x++;
                }
                bm=*pFont++;
                for(j=0x80;j>DBITTEST;j=j>>1){
                    //if(((*pFont)&j)!=0){
                    if((bm&j)!=0){
                        gPoint(x,y,dc.color);
                    }
                    else if(dc.textbkmode){
//                    else{
                        gPoint(x,y,dc.backcolor);
                    }
                    x++;
                }
                y++;
                x-=(2*FONTXSIZE);	//12
            }
            x+=(2*FONTXSIZE);
        }
        else{     //ascii
            code=ch[k]-0x20;
            if(code<0 || code >95) code=0;
            pFont=(u8*)(dc.pFont+(code*ASCBYTES));
            for(m=0;m<FONTYSIZE;m++){
                bm=*pFont++;
#ifdef LCD_FONT12                                
                pFont++;
#endif
                for(j=0x80;j>BITTEST;j=j>>1){ 
                    if(((bm)&j)!=0){
                        gPoint(x,y,dc.color);
                    }	
                    else if(dc.textbkmode){
//                    else{
                        gPoint(x,y,dc.backcolor);
                    }
                    x++;
                }
                y++;
                x-=FONTXSIZE;
            }
            x+=FONTXSIZE;
        }
        y-=FONTYSIZE;
        k+=1;
    }
        
}	
// -----------------------------------------------------------------
// _OutLine: internal function supporting LCD_Printf
// 	   drawing a text row, starting from column c0 to c1
// input : 
//    c0, c1 : column position
//    d: pointer to string
// -----------------------------------------------------------------
void _OutLine(int c0,int c, char *s)
{
#ifdef USE_FRAME_BUFFER
      int j,k;
      int blk=ROW_HT*MAX_X;
      volatile u32  *p= (u32 *)LCD_GUI;
      
      if((dc.row)>(MAXROWS-1)){	        // at the last row
          //-- first update the buffer
          //scroll up the screen by one row
          for(k=0;k<(MAX_X*(MAX_Y-ROW_HT));k+=blk){
                  for(j=0;j<blk;j++){
                          p[k+j]=p[k+j+blk];
                  }
                  //LCD_BLT_DMA((int)&p[k+blk],(int)&p[k],blk);
          }
          //clear the last row
          for(j=0;j<blk;j++){
                  p[k+j]=dc.backcolor;
          }
          //LCD_FILL_DMA(dc.backcolor,(int)&p[k],blk);
          //-- then copy the buffer to LCD ram
          dc.row--;
          _LCD_Rect_Copy( 0,0,MAX_X_1,MAX_Y_1,(u32*)LCD_GUI);

      }
#else
      //for no buffer mode
      if((dc.row)>(MAXROWS-1)){	        // at the last row
        LCD_ClearScr(dc.backcolor);     // just clear the screen 
        dc.row=0;                       // and print from the first row
      }

#endif
      
      s[c-c0]=0; 
//	_TextOut1((u8*)d,c0*FONTXSIZE,dc.row<<4,dc.color);
      _TextOut((u8*)s,c0*FONTXSIZE,dc.row<<4);
      //LCD_Copy((u8*)LCD_GUI, 0,dc.row<<4,239,(dc.row<<4)+15);
      if(c>(MAXCOLS-1))	//29) 
              if(dc.row<(MAXROWS))
                      dc.row++;	

}

// -----------------------------------------------------------------
//   LCD_Printf :   文字终端方式的格式化字符串输出函数
//      in  : format,... : 可变数量的输入变量，格式同C语言库函数printf()
//      out : 输出的字符串长度
//   note :
//   1  字符串显示颜色和背景颜色由 gSetColor() 和 gSetBkColor()指定
//   2  字符串按文字终端方式显示，每个字符的位置固定在行列网格上，超过屏幕
//      宽度的字符回卷到下一行开头。当到达屏幕的最下面一行时，屏幕内容会自
//      动上滚。屏幕可显示15行，53列半角字符。
//   3  新的字符串显示行列位置紧跟着上一个字符串最后一个字符之后。可以利用
//      LCD_GotoXY()函数设定新的字符串输出行列位置。
// -----------------------------------------------------------------
int LCD_Printf(char* format,...)
{
	int m, k, n;
	int c0,c;
	char d[MAXCOLS+1];//d[30];
	
	va_list ap;
	va_start(ap, format);
	n= vsprintf(lcdpch,format,ap);
	
	if(n==0) return 0;
	
	c=c0=dc.col;
	k=0;m=0;
	while(lcdpch[k]!=0){
		if(lcdpch[k]==0x09){	        //tab
			d[m++]=0x20;
			c++;
			if((c+4)>(MAXCOLS-1)){	//44){
				while(c++<MAXCOLS)	//30) 
					d[m++]=0x20;
				_OutLine(c0,c,d);
				c0=c=0;
				dc.col=0;
				m=0;
			}
			else{
				while((c&0x03)!=0){
					d[m++]=0x20;
					c++;
				}
			}
			k++;
		}
		else if(lcdpch[k]==0x0a){	//LF
			_OutLine(c0,c,d);
			c0=c=0;
			dc.col=0;
			m=0;
			dc.row++;
			k++;
		}
		else if(lcdpch[k]==0x0d){	//CR
			_OutLine(c0,c,d);
			c0=c=0;
			dc.col=0;
			m=0;
			//dc.row++;
			k++;
		}
		else if(lcdpch[k]>0x7f){	//hanzi
			if(c>=(MAXCOLS-1)){
				d[m]=' ';		//pad a space
				_OutLine(c0,++c,d);
				c0=c=0;
				dc.col=0;
				m=0;
			}
			else{
				c++;
				d[m++]=lcdpch[k++];
				d[m++]=lcdpch[k++];
				if(c++>=(MAXCOLS-1)){
					_OutLine(c0,c,d);
					c0=c=0;
					dc.col=0;
					m=0;
				}
			}
		}
		else{				// ascii
			d[m++]=lcdpch[k];
			if(c++>=(MAXCOLS-1)){	//29){
				_OutLine(c0,c,d);
				c0=c=0;
				dc.col=0;
				m=0;
			}
			k++;
		}
	}
	if(c>c0){
		_OutLine(c0,c,d);
		dc.col=c;
	} 
	
	va_end(ap);

	return n;

}
// -----------------------------------------------------------------
//	LCD_Goto : 指定下一次字符输出位置(行列)
//  input : 
//    col : 列号，水平字符位置, 0-33
//    row : 行号，垂直字符位置, 0-19 
// -----------------------------------------------------------------
void LCD_Goto(u32 row, u32 col)
{
      dc.row=row;
      dc.col=col;
      
      if(dc.row>(MAXROWS-1)) dc.row=(MAXROWS-1);
      if(dc.col>(MAXCOLS-1)) dc.col=(MAXCOLS-1);	//29
} 

/*
typedef struct{
char name[32];
int imgoffset;
int (*proc)();
}MENUITEM;

typedef struct{
int count;
char title[64];
char *imgptr;
MENUITEM item[64]; 
}MENU;
int doSomething(void)
{
	return 0;
}
int ImgIndex[6]=
{
0,
10102,
10198,
10102,
9726,
10102
};
#define MENU_IMAGE	0x60000000
MENU sport={6,"Game ticket",(char *)MENU_IMAGE,
"game 1",0,doSomething,
"game 2",10102,doSomething,
"game 3",10198,doSomething,
"game 4",10102,doSomething,
"game 5",9726,doSomething,
"game 6",10102,doSomething
};
int InitMenu(void)
{
	return 0;	
}  
*/