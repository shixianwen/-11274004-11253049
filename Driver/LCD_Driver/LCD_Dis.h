#include "LCD_Config.h"
#include "LCD_PortConfig.h"
#include "LCD_Driver_User.h"  //


//===========================================================
//�û��ɵ��õĺ�����ʽ
/* 
LCD api����/�����˵��

1 void LCD_Init(void);

��ʼ����ʾģ��


2 void LCD_ColorSet(unsigned char color);

���û�ͼɫ��: 
�������: color=1 , 0


3 void LCD_FontSet(unsigned char Font_NUM);

ascii�ַ���������������С����
���������
font =0 : 6x10Ӣ��С����
     =1 : 8x16Ӣ�Ĵ�����


4 unsigned char LCD_FontColor(unsigned char Color);

����������ɫ
���������Color=0,1


5 void LCD_FontBackFill(int fill);

�����ַ���ʾ�ĸ��ǻ����ģʽ
�������  fill=1: �ñ���ɫ����ַ�����, fill=0: �����


6 LCD_PutChar(unsigned char x,unsigned char y,char asc)

��ָ��λ��д��һ����׼�ַ�
�������: x : �ַ����Ͻ�����ˮƽ����(0-127��0Ϊ����࣬127Ϊ���Ҳ�)
          y : �ַ����Ͻ����ش�ֱ����(0-63��0Ϊ���ϲ࣬63Ϊ������)
          asc :  Ҫ��ʾ���ַ���asc��  


7 LCD_PutString(unsigned char x,unsigned char y,char *s)

��x��yΪ��ʼ���괦д��һ����׼�ַ�
�������: x : �ַ����Ͻ�����ˮƽ����(0-127��0Ϊ����࣬127Ϊ���Ҳ�)
          y : �ַ����Ͻ����ش�ֱ����(0-63��0Ϊ���ϲ࣬63Ϊ������)
          s :  Ҫ��ʾ���ַ���

8 void LCD_TextOut(u32 x, u32 y, char *ch);

���12x12�����ֵĺ���
������� x,y : ��ʼ�ַ������Ͻ�����
         ch  : �ַ���


9 LCD_Point(unsigned char x, unsigned char y)

��ָ��λ���ϻ���һ��ǰ��ɫ�ĵ㡣
�������: x : ͼ�ε������ˮƽ����(0-127��0Ϊ����࣬127Ϊ���Ҳ�)
          y : ͼ�ε�����ش�ֱ����(0-63��0Ϊ���ϲ࣬63Ϊ������)
  �����ɫ�����һ�ζԺ���LCD_ColorSet()�ĵ��þ���


10 LCD_Line(unsigned char x1, unsigned char y1��unsigned char x2, unsigned char y2)

������֮�仭һ��ֱ�ߣ���ɫ�����һ�ζԺ���LCD_ColorSet()�ĵ��þ�����
�������: x1,y1 : ֱ�ߵ��������(x1 =0-127��y1 = 0-63)
          x2,y2 : ֱ�ߵ��յ�����(x2 =0-127��y2 = 0-63)


11 LCD_Rect(unsigned char x1, unsigned char y1��unsigned char x2, unsigned char y2,unsigned char mode)

��һ�����Σ�ѡ��������䣬��ɫ�����һ�ζԺ���LCD_ColorSet()�ĵ��þ�����
�������: x1,y1 : ���ε����Ͻ�����(x1 =0-127��y1 = 0-63)
          x2,y2 : ���ε����½�����(x2 =0-127��y2 = 0-63)
          mode : =0 :����䣬���ľ���
                 =1 :��䣬ʵ�ľ���

12 LCD_Circle(unsigned char x,unsigned char y,unsigned char r,unsigned char mode)

��һ��Բ�Σ�ѡ��������䣬��ɫ�����һ�ζԺ���LCD_ColorSet()�ĵ��þ�����
�������: x,y : Բ�ε�Բ������(x1 =0-127��y1 = 0-63)
          r   : Բ�ε�ֱ���������ص�Ϊ��λ
          mode : =0 :����䣬����Բ
                 =1 :��䣬ʵ��Բ

13 LCD_ClrScreen(unsigned char mode)

���ƶ�����ɫ�����Ļ
������� mode = 0 : ��Ĭ�ϵı���ɫ(0)
         mode = 1 : ��ǰ���趨��ǰ��ɫ(0��1)

*/

//===========================================================================
//���¶����ͼapi�������������û����õ�����

//---- ��ֱ�ߺ�����x1��y1Ϊ��ʼ�㣬x2��y2Ϊ������
// void LCD_Line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2) 
#define LCD_Line(x1,y1,x2,y2)  Line(x1,y1,x2,y2)

//---- ���û�ͼǰ����ɫ��color=0,1
// void LCD_ColorSet(uint8_t color)
#define LCD_ColorSet(color) SetPaintMode(0,color)

//---- �����ص�����(x,y)Ϊ���Ͻǵľ���λ�ã����һ��5x10��С���ַ�a��
// void LCD_PutChar(uint8_t x, uint8_t y, uint8_t a)
#define LCD_PutChar(x,y,a) PutCharPutChar(x,y,a) 

// ---- �����ص�����(x,y)Ϊ���Ͻǵľ���λ�ÿ�ʼ�����һ���ַ���s��
// void LCD_PutString(uint8_t x, uint8_t y, uint8_t *s)
#define LCD_PutString(x,y,s) PutString(x,y,s)

// ---- �����ص�����(x,y)�����һ���㣬��ɫ��֮ǰLCD_ColorSet()�趨��
// void LCD_Point(uint8_t x, uint8_t y) 
#define LCD_Point(x,y) PutPixel(x,y)

// ---- ��һ�����Σ����Ͻ�����(x1,y1)�����½�����(x2,y2), ��ɫ��֮ǰLCD_ColorSet()�趨��
//      m=0: �ڲ�����䣬 m=1: �ڲ����
// void LCD_Rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t m)
#define LCD_Rect(x1,y1,x2,y2,m) Rectangle(x1,y1,x2,y2,m)

// ---- ��һ��԰��Բ������(x,y)���뾶r�� ��ɫ��֮ǰLCD_ColorSet()�趨��
//      m=0: �ڲ�����䣬 m=1: �ڲ����
// void LCD_Circle(uint8_t x, uint8_t y, uint8_t r, uint8_t m)
#define LCD_Circle(x,y,r,m) Circle(x,y,r,m)

// ---- ��ָ������ɫ�����Ļ : c =0,1 Ϊ��ɫ
// void LCD_ClrScreen(uint8_t c) 
#define LCD_ClrScreen(c) ClrScreen(c)


// ---- LCD ��ʼ������, ����LCD���ļĴ�����ʼ��
extern void LCD_Init(void);	//��LCD_Driver_User.c�ж���ĺ���

// ---- ��׼�ַ����ã��������ͺ�ASCII��Ĵ�С�ߴ磬0��1
extern void LCD_FontSet(unsigned char Font_NUM);

// ---- ����������ɫ0��1
extern unsigned char LCD_FontColor(unsigned char Color);

// ---- �����ַ���ʾ�ĸ��ǻ����ģʽ, fill=1: �ñ���ɫ����ַ�����, fill=0: �����
extern void LCD_FontBackFill(int fill);

// ---- ���12x12�����ֵĺ�����x,y : ��ʼ�ַ������Ͻ����꣬ch :�ַ���
extern void LCD_TextOut(u32 x, u32 y, char *ch);









//===================================================================
//������Щ�����ڲ�ʹ�ã��û��������
//��ֱ�ߺ�����s_x��s_yΪ��ʼ�㣬e_x��e_yΪ������
extern void Line(LCDBYTE s_x,LCDBYTE s_y,LCDBYTE e_x,LCDBYTE e_y);
//��ͼģʽ����ǰ��ͼ��ɫ����
extern void SetPaintMode(BYTE Mode,LCDBYTE Color);

//��x��y��������д��һ����׼�ַ�
extern void PutChar(DOTBYTE x,DOTBYTE y,char a);
//��x��y������Ϊ��ʼд��һ����׼�ַ���
extern void PutString(DOTBYTE x,DOTBYTE y,char *p);
//��x��y������Ϊ���ģ�����һ����
extern void PutPixel(DOTBYTE x,DOTBYTE y);
//��x��y������Ϊ���ģ�����һ��Բ�߻�ʵ��Բ
extern void Circle(DOTBYTE x,DOTBYTE y,DOTBYTE r,BYTE mode);
//����һ����left��top��right��bottomΪ�����Խǵľ��ο����ʵ�ľ���
extern void Rectangle(DOTBYTE left, DOTBYTE top, DOTBYTE right, DOTBYTE bottom, BYTE Mode);
//����������ִ��ȫ��Ļ��������ǰ��ɫ
extern void ClrScreen(BYTE Mode);

//���º����Լ�����������һ�㽨���û���Ҫ���ã������߼��û������б�д������ʾЧ���Լ�����Ӧ�ó���ʱʹ��
//

extern DOTBYTE X_Witch;						//�ַ�д��ʱ�Ŀ��
extern DOTBYTE Y_Witch;						//�ַ�д��ʱ�ĸ߶�
extern DOTBYTE Font_Wrod;					//�����ÿ����ģռ�ö��ٸ��洢��Ԫ��
extern const unsigned char *Char_TAB;				//�ֿ�ָ��
//unsigned char Plot_Mode;					//��ͼģʽ
extern LCDBYTE BMP_Color;
extern LCDBYTE Char_Color;

extern BYTE	Font_type;
extern BYTE	Font_Cover;								//
extern LCDBYTE	Char_BackColor;							//

//extern void Pos_Switch(unsigned int * x,unsigned int * y);
//extern void Writ_Dot(int x,int y,unsigned int Color);
//extern void Line_f(int s_x,int s_y,int e_x);
//extern unsigned int Get_Dot(int x,int y);
//extern void Clear_Dot(int x,int y);
//extern void Set_Dot_Addr(int x,int y);
