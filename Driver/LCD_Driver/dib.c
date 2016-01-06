//dib.c
//#include "71x_lib.h"
#include "stm32f10x.h"
#include "lcdlib.h"

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef long LONG;

typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;

typedef __packed struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef __packed struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];	//实际上尺寸为调色板入口个数乘以RGBQUAD
} BITMAPINFO,*PBITMAPINFO;

typedef struct tagBITMAPINFOPT {
    BITMAPINFOHEADER    *pbmiHeader;
    char                *pbmiColors;	//实际上尺寸为调色板入口个数乘以RGBQUAD
} BITMAPINFOPT;

//extern GDC dc;

//=================================================================
BITMAPINFOPT DibInfo;	
BITMAPFILEHEADER *pbmfileheader;
BITMAPINFOHEADER *pbmheader;

u16 BmpLut[256];
char* DibBits;

int GetBitsPerPixel()
{ 
	return DibInfo.pbmiHeader->biBitCount;
}

DWORD BytesPerLine()
{
	DWORD bytes_per_line;
	//fillup byte
	bytes_per_line=(DibInfo.pbmiHeader->biWidth*GetBitsPerPixel()+7)/8;
	//quad-byte alignment
	bytes_per_line=(bytes_per_line+3)/4;
	return bytes_per_line*4;
}

// -----------------------------------------------------------------
// DrawDIB : 显示DIB(BMP)图像, 图像为调色板彩色方式，颜色数<=256
//    bmpbuf : 图像数据缓冲区指针
//    x0, y0 : 图像显示位置左上角象素坐标
// -----------------------------------------------------------------
int DrawDIB(char *bmpbuf,int x0,int y0)
{
	long size;
	int PaletteSize=0;
	int i,n;//,cbHeaderSize;
	u16 *plut;
	int w,mask,x,y,m;
	u8 *pbuf, *pRGB;

	pbmfileheader=(BITMAPFILEHEADER *)bmpbuf;
	if(pbmfileheader->bfType!=0x4d42) return FALSE; 
	size=sizeof(BITMAPFILEHEADER);
	pbmheader=(BITMAPINFOHEADER*)(bmpbuf+size);

	switch(pbmheader->biBitCount)
	{
	case 1:
		PaletteSize=2;
		break;
	case 4:
		PaletteSize=16;
		break;
	case 8:
		PaletteSize=256;
		break;
	}

	//cbHeaderSize=sizeof(BITMAPINFOHEADER)+PaletteSize*sizeof(RGBQUAD);
	//DibInfo=(BITMAPINFO*)new char[cbHeaderSize];

	DibInfo.pbmiHeader=pbmheader;
	DibInfo.pbmiColors=(char*)pbmheader+sizeof(BITMAPINFOHEADER);

//	size=BytesPerLine()*DibInfo.pbmiHeader->biHeight;
	y=DibInfo.pbmiHeader->biHeight;
	if(y>(MAX_Y-1-y0)) y=MAX_Y-1-y0;
	size=BytesPerLine()*y;
	DibBits=bmpbuf+pbmfileheader->bfOffBits;	//(void*) new char[size];

	//make LUT
	pRGB=(u8*)(DibInfo.pbmiColors);
	//RGB565 : RRRrrGGGgggBBbbb
	for(i=0;i<PaletteSize;i++){
		n=(u32)((*pRGB++)>>3);		//blue 
		n|=((u32)(*pRGB++)<<3)&0x07e0;	//green
		n|=((u32)(*pRGB++)<<8)&0xf800;	//red
		pRGB++;				//skip reserved byte
		BmpLut[i]=(u16)n;
	}
	plut=LCD_SetPalette(BmpLut);
	//copy image bits to LCDbuffer, convert to 8bit pixel
	pbuf=(dc.cache+x0+(240*y0));
	n=GetBitsPerPixel();
	mask=0xff;
	mask=mask>>(8-n);
	w=DibInfo.pbmiHeader->biWidth;
	if(w>(MAX_X-1-x0)) w=MAX_X-1-x0;
	for(y=size-BytesPerLine();y>=0;y-=BytesPerLine()){
		x=0;
		i=w;
		while(i){
			m=8-n;
			do{
				*pbuf++=(*(DibBits+y+x)>>m)&mask;
				m-=n;
				i--;
			}while(m>=0&&i>0);
			x++;
		}
		pbuf-=w;
		pbuf+=240;
	}
	//
	LCD_Copy(pbuf,x0,y0,x0+w-1,y0+DibInfo.pbmiHeader->biHeight-1);
	LCD_SetPalette(plut);
	
	
	return TRUE;
}


