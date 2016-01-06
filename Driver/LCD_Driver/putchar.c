//========================================================================
// putchar.c
//========================================================================
extern const unsigned char Asii0816[];			//8X16��ASII�ַ���

//��x��y��������д��һ����׼�ַ�
void PutChar(int x, int y, unsigned char a);
//��x��y������Ϊ��ʼд��һ����׼�ַ���
void PutString(int x, int y,unsigned char *p);
//��x��y������������ɫc����һ����
void PutPixel(int x, int y, int c);
// ����������ɫ�ͱ���ģʽ
void SetTextMode(int mode,int color, int bkcolor);

//����һ����left��top��right��bottomΪ�����Խǵľ��ο����ʵ�ľ���
//void Rectangle(DOTBYTE left, DOTBYTE top, DOTBYTE right, DOTBYTE bottom, BYTE Mode);

//�������ֲ���
int  text_mode = 1;         //����ģʽ��=1: ʹ�ñ�����ɫ��=0: ʹ��ͼ�񱳾�
int  text_color =0xffff;   // ������ɫ��Ĭ�ϰ�ɫ��
int  text_bkcolor=0;      // ���ֱ�����ɫ��Ĭ�Ϻ�ɫ



//========================================================================
// ����: void PutChar(unsigned char x,unsigned char y,char a)  
// ����: д��һ����׼�ַ�
// ����: x,y : �ֱ����������Ͻ�x�������y(��)����
//       a   : Ҫ��ʾ�ַ���ASCII��  
// ����: ��
//========================================================================
void PutChar(int x,int y, unsigned char a)       
{
    int i,j;		//�����ݴ�
    const unsigned char *p_font=Asii0816+(a-32)*16; //��ģ�׵�ַ
    unsigned char bmp;
    
    //a font has 8 pixels in width and 16 pixels in height. 
    for(i=0;i<16;i++){  
      bmp=*(p_font+i);
      for(j=0;j<8;j++){
        if(0x80&bmp)
          PutPixel(y+i,x+j,text_color);      //����ǰ����ɫ
        else{
          if(text_mode) //��Ҫ�ñ�����ɫ����ͼ��
            PutPixel(y+i,x+j,text_bkcolor);    //���ֱ�����ɫ��ȥ�������䣬�ɱ���ԭ������ͼ��
        }
        bmp<<=1;
      }
    }
}

//========================================================================
// ����: void PutString(unsigned char x,unsigned char y,char *p)
// ����: ��x��yΪ��ʼ���괦д��һ����׼�ַ�
// ����: x  X������     y  Y������
//	 p  Ҫ��ʾ���ַ���  
// ����: ��
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
// ����������ɫ�ͱ���ģʽ
// text_mode ����ģʽ��=1: ʹ�ñ�����ɫ����ͼ��=0: ʹ��ͼ�񱳾�
// text_color : ������ɫ
// text_bkcolor : ������ɫ
//========================================================================
void SetTextMode(int mode,int color, int bkcolor)
{
    text_mode = mode;
    text_color =color;   // ????
    text_bkcolor=bkcolor;
}
//========================================================================
// ����: void PutPixel(int x,int y, int c)
// ����: ��x��y���ϻ���һ��ָ����ɫc�ĵ�
// ����: x  X������     y  Y������
// ����: ��
//========================================================================
void PutPixel(int x, int y, int c)
{
  //y : row
  //x : column, or pixels in a row
  //��ͼ���Y, U, V�������ʵ�λ��д���ʵ�����ֵ
  //�����ô˺�����һ��ר�ŵĻ�������Ԥ��д�����֣�Ȼ����ʵʱͼ����ʱ�������������
  //���Ƶ�Ҫ��ʾ��ͼ��λ
  
}


