//========================================================================
// putchar.c
//========================================================================
extern const unsigned char Asii0816[];			//8X16的ASII字符库

//于x、y的坐标上写入一个标准字符
void PutChar(int x, int y, unsigned char a);
//于x、y的坐标为起始写入一串标准字符串
void PutString(int x, int y,unsigned char *p);
//在x、y的坐标上用颜色c绘制一个点
void PutPixel(int x, int y, int c);
// 设置文字颜色和背景模式
void SetTextMode(int mode,int color, int bkcolor);

//绘制一个以left、top和right、bottom为两个对角的矩形框或者实心矩形
//void Rectangle(DOTBYTE left, DOTBYTE top, DOTBYTE right, DOTBYTE bottom, BYTE Mode);

//绘制文字参数
int  text_mode = 1;         //背景模式，=1: 使用背景颜色，=0: 使用图像背景
int  text_color =0xffff;   // 文字颜色，默认白色？
int  text_bkcolor=0;      // 文字背景颜色，默认黑色



//========================================================================
// 函数: void PutChar(unsigned char x,unsigned char y,char a)  
// 描述: 写入一个标准字符
// 参数: x,y : 分别是文字左上角x轴坐标和y(行)坐标
//       a   : 要显示字符的ASCII码  
// 返回: 无
//========================================================================
void PutChar(int x,int y, unsigned char a)       
{
    int i,j;		//数据暂存
    const unsigned char *p_font=Asii0816+(a-32)*16; //字模首地址
    unsigned char bmp;
    
    //a font has 8 pixels in width and 16 pixels in height. 
    for(i=0;i<16;i++){  
      bmp=*(p_font+i);
      for(j=0;j<8;j++){
        if(0x80&bmp)
          PutPixel(y+i,x+j,text_color);      //文字前景颜色
        else{
          if(text_mode) //需要用背景颜色覆盖图像
            PutPixel(y+i,x+j,text_bkcolor);    //文字背景颜色，去掉这个语句，可保留原来背景图像
        }
        bmp<<=1;
      }
    }
}

//========================================================================
// 函数: void PutString(unsigned char x,unsigned char y,char *p)
// 描述: 在x、y为起始坐标处写入一串标准字符
// 参数: x  X轴坐标     y  Y轴坐标
//	 p  要显示的字符串  
// 返回: 无
//========================================================================
void PutString(int x,int y, unsigned char *p)
{
    while(*p!=0)
    {
        PutChar(x,y,*p);
        x += 16;  
        p++;
    }
}
//========================================================================
// 设置文字颜色和背景模式
// text_mode 背景模式，=1: 使用背景颜色覆盖图像，=0: 使用图像背景
// text_color : 文字颜色
// text_bkcolor : 背景颜色
//========================================================================
void SetTextMode(int mode,int color, int bkcolor)
{
    text_mode = mode;
    text_color =color;   // ????
    text_bkcolor=bkcolor;
}
//========================================================================
// 函数: void PutPixel(int x,int y, int c)
// 描述: 在x、y点上绘制一个指定颜色c的点
// 参数: x  X轴坐标     y  Y轴坐标
// 返回: 无
//========================================================================
void PutPixel(int x, int y, int c)
{
  //y : row
  //x : column, or pixels in a row
  //在图像的Y, U, V缓冲区适当位置写入适当的数值
  //可以用此函数在一个专门的缓冲区里预先写好文字，然后在实时图像处理时，把这个缓冲区
  //复制到要显示的图像部位
  
}


