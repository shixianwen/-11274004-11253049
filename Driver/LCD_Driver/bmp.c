/*
File        : BMP.c
*/
#include "stm32f10x.h"

#include "lcdlib.h"

#include <stdlib.h>

//#include "GUI_Private.h"
//RGB565
//unsigned short LCD__aConvTable[256];
//#define LCD_Color2Index(r,g,b)	(((short)(r&0xf8)<<8)|((short)(g&0xfc)<<3)|((b&0xf8)>>3)) 
//RGB888
u32 LCD__aConvTable[256];
#define LCD_Color2Index(r,g,b)	(((u32)(r&0xff))|((u32)(g&0xff)<<8)|((u32)(b&0xff)<<16)) 
/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetStep
*/
static int _GetStep(int * pYSize, int * pY) {
    if (*pYSize > 0) {
        *pY = *pYSize - 1;
        return -1;
    } 
    else if (*pYSize < 0) {
        *pYSize = -*pYSize;
        *pY = 0;
        return 1;
    } 
    else {
        return 0;
    }
}
//--------------------------------------
void LCD_DrawBitmap(int x0, int y0, int xsize, int ysize, int xMul, int yMul,
                       int BitsPerPixel, int BytesPerLine,
                       const unsigned char * pPixel, const unsigned int * pTrans)
{
//        volatile u32  *p_buffer = (u32 *)LCD_GUI;       // 设置填充显示缓冲区的地址

	unsigned char  Data = 0;
	int x1, y1;
	/* Handle the optional Y-magnification */
	y1 = y0 + ysize - 1;
	x1 = x0 + xsize - 1;
/*  Handle BITMAP without magnification */
	if((xMul | yMul) == 1) {
    	int Diff;
	    /*  Clip y1 (bottom) */
    	Diff = y1 - MAX_Y-1;	//GUI_Context.ClipRect.y1;
    	if (Diff > 0) {
      		ysize -= Diff;
      		if (ysize <= 0) {
		    	return;
      		}
    	}
    	/*        Clip right side    */
    	Diff = x1 - MAX_X-1;	//GUI_Context.ClipRect.x1;
    	if (Diff > 0) {
      		xsize -= Diff;
    	}
    	Diff = 0;

    //LCDDEV_L0_DrawBitmap   (x0,y0, xsize, ysize, BitsPerPixel, BytesPerLine, pPixel, Diff, pTrans);
            {
                //--
                int x,y;
                int yi;
                int Shift = 8-BitsPerPixel;
                for (y=y0, yi=0; yi<ysize; yi++, y+= yMul, pPixel+=BytesPerLine) {
                        int BitsLeft =0;
                        int xi;
                        const unsigned char * pDataLine = pPixel;
                        for (x=x0, xi=0; xi<xsize; xi++, x+=xMul) {
                              unsigned char  Index;
                              if (!BitsLeft) {
                                  Data = *pDataLine++;
                                  BitsLeft =8;
                              }
                              Index = Data>>Shift;
                              Data    <<= BitsPerPixel;
                              BitsLeft -= BitsPerPixel;
                              
                              gPoint(x,y,*(pTrans+Index));
                              //*(p_buffer+x+(y*MAX_X)) = *(pTrans+Index);
                        }
                }	//for y
            
            }
		//--    
	} //if ((xMul | yMul) == 1)

	else{
	    /**** Handle BITMAP with magnification ***/
	    int x,y;
	    int yi;
	    int Shift = 8-BitsPerPixel;
	    for (y=y0, yi=0; yi<ysize; yi++, y+= yMul, pPixel+=BytesPerLine) {
                int yMax = y+yMul-1;
                int BitsLeft =0;
                int xi,LCD_COLORINDEX;
                const unsigned char *pDataLine = pPixel;
                for (x=x0, xi=0; xi<xsize; xi++, x+=xMul) {
                      int  Index;
                      if (!BitsLeft) {
                            Data = *pDataLine++;
                            BitsLeft =8;
                      }
                      Index = Data>>Shift;
                      Data    <<= BitsPerPixel;
                      BitsLeft -= BitsPerPixel;
                      LCD_COLORINDEX = *(pTrans+Index);
                      //LCD_FillRect(int x0,int y0,int x1,int y1, u16 color)
                      gRectFill(x,y, x+xMul-1, yMax, LCD_COLORINDEX);
                } //for x
            } //for y
  	}
}




/*********************************************************************
*
*       _DrawBitmap_Pal
*/

static int _DrawBitmap_Pal(const unsigned char * pData, int x0, int y0, int XSize, int YSize, int Bpp, int NumColors) 
{
      int i, y, Step;
      int BytesPerLine = 0;
      Step = _GetStep(&YSize, &y);
      if (!Step) {
          return 1;
      }
      for (i = 0; i < NumColors; i++) {
          unsigned char r, g, b;
          b = *(pData);
          g = *(pData + 1);
          r = *(pData + 2);
          pData += 4;
          LCD__aConvTable[i] = LCD_Color2Index(r,g,b);//((U32)b << 16) | (g << 8) | r);
      }
      switch (Bpp) {
          case 1:
              BytesPerLine = ((XSize + 31) >> 5) << 2;
              break;
          case 4:
              BytesPerLine = (((XSize << 2) + 31) >> 5) << 2;
              break;
          case 8:
              BytesPerLine = ((XSize +  3) >> 2) << 2;
              break;
      }
    //wen
    if((x0+XSize)>MAX_X){
        XSize=MAX_X-x0;
    }
    if(XSize<1) return 1;
    //
    
      for (; (y < YSize) && (y >= 0); y += Step) {
          if((y0+y)<(MAX_Y)){    //wen
              LCD_DrawBitmap(x0, y0 + y, XSize, 1, 1, 1, Bpp, XSize, pData, LCD__aConvTable);
          }
          pData += BytesPerLine;
      }
      return 0;
}

//       _DrawBitmap_24bpp
static int _DrawBitmap_24bpp(const unsigned char * pData, int x0, int y0, int XSize, int YSize) 
{
//    volatile u32  *p_buffer = (u32 *)LCD_GUI;       // 设置填充显示缓冲区的地址
    int x, y, BytesPerLine, Step;
    Step = _GetStep(&YSize, &y);
    if (!Step) {
        return 1;
    }
    //wen
    if((x0+XSize)>MAX_X){
        XSize=MAX_X-x0;
    }
    if(XSize<1) return 1;
    
//    if((y0+YSize)>MAX_Y){
//        YSize=MAX_Y-y0;
//    }
    //
    BytesPerLine = ((24 * XSize + 31) >> 5) << 2;
    for (; (y < YSize) && (y >= 0); y += Step) {
        if((y0+y)<(MAX_Y)){    //wen
            for (x = 0; x < XSize; x++) {
                const  unsigned char * pColor = pData + 3 * x;
                 unsigned char r, g, b;
                b = *(pColor);
                g = *(pColor + 1);
                r = *(pColor + 2);
        //      LCD_SetPixelIndex(x0 + x, y0 + y, LCD_Color2Index(((U32)b << 16) | (g << 8) | r));
                gPoint(x0+x,y0+y,r|(g<<8)|(b<<16));  //LCD_Color2Index(r,g,b));
                //*(p_buffer+x0 + x	+( y0 + y)*MAX_X)=LCD_Color2Index(r,g,b); 
            }
        }
        pData += BytesPerLine;
    }
    return 0;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_BMP_GetXSize
*/

unsigned short GUI__Read16(const unsigned char ** ppData) {
    const unsigned char * pData;
    unsigned short  Value;
    pData = *ppData;
    Value = *pData;
    Value |= (unsigned short)(*(pData + 1) << 8);
    pData += 2;
    *ppData = pData;
    return Value;
}
unsigned int GUI__Read32(const  unsigned char ** ppData) {
    const  unsigned char * pData;
    unsigned int  Value;
    pData = *ppData;
    Value = *pData;
    Value |= (     *(pData + 1) << 8);
    Value |= ((unsigned int)*(pData + 2) << 16);
    Value |= ((unsigned int)*(pData + 3) << 24);
    pData += 4;
    *ppData = pData;
    return Value;
}

int gBMPGetXSize(const void * pBMP) 
{
    const unsigned char  * pSrc = (const unsigned char  *)pBMP;
    if (!pBMP) {
        return 0;
    }
    pSrc += 18;                   /* skip rest of BITMAPFILEHEADER */
    return GUI__Read32(&pSrc);
}

/*********************************************************************
*
*       GUI_BMP_GetYSize
*/
int gBMPGetYSize(const void * pBMP) 
{
    const unsigned char  * pSrc = (const unsigned char  *)pBMP;
    if (!pBMP) {
        return 0;
    }
    pSrc += 22;
    return labs((int)GUI__Read32(&pSrc));
}

/*********************************************************************
*
*       GUI_BMP_Draw
*/
int gBMPDraw(int x0, int y0, const void * pBMP) 
{
    int Ret = 0;
    signed long Width, Height;
    unsigned short BitCount, Type;
    unsigned int ClrUsed, Compression;
    int NumColors;
    const unsigned char * pSrc = (const unsigned char *)pBMP;
    Type        = GUI__Read16(&pSrc); /* get type from BITMAPFILEHEADER */
    pSrc += 12;                   /* skip rest of BITMAPFILEHEADER */
    /* get values from BITMAPINFOHEADER */
    pSrc += 4;
    Width       = GUI__Read32(&pSrc);
    Height      = GUI__Read32(&pSrc);
    pSrc += 2;
    BitCount    = GUI__Read16(&pSrc);
    Compression = GUI__Read32(&pSrc);
    pSrc += 12;
    ClrUsed     = GUI__Read32(&pSrc);
    pSrc += 4;
    /* calculate number of colors */
    switch (BitCount) {
        case 0:   return 1; /* biBitCount = 0 (JPEG format) not supported. Please convert image ! */
        case 1:   NumColors = 2;   break;
        case 4:   NumColors = 16;  break;
        case 8:   NumColors = 256; break;
        case 24:  NumColors = 0;   break;
        default:
            return 1; /* biBitCount should be 1, 4, 8 or 24 */
    }
    if (NumColors && ClrUsed) {
        NumColors = ClrUsed;
    }
    /* check validity of bmp */
    if ((NumColors > 256) ||
          (Type != 0x4d42)                 || /* 'BM' */
          (Compression)                    || /* only uncompressed bitmaps */
          (Width  > 1024)                  ||
          (Height > 1024)) {
        return 1;
    }
    /* Show bitmap */
    switch (BitCount) {
      case 1:
      case 4:
      case 8:
          Ret = _DrawBitmap_Pal(pSrc, x0, y0, Width, Height, BitCount, NumColors);
          break;
      case 24:
          Ret = _DrawBitmap_24bpp(pSrc, x0, y0, Width, Height);
          break;
    }
    return Ret;
}

/*************************** End of file ****************************/
