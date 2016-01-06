//========================================================================
// �ļ���: LCD_Dis.c
// ��  ��: Xinqiang Zhang(email: Xinqiang@Mzdesign.com.cn)
//			www.Mzdesign.com.cn
// ��  ��: 2007/02/24
// ��  ��: ͨ�ð�LCD�û��ӿڲ����
//
// ��  ��: ��ο�������ʾ����Ӳ������˵���������DataSheet,���Դ�����д����
//			��ӿڳ���;������MCS51ϵ��MCU
// ��  ��:
//      2006/10/18      First version    Mz Design
//		2007/01/13		V1.1  ȥ��������ϵ�任�Ĺ��ܣ����ٶȽ���һ�����Ż�
//								��ʹ���ļ��Ĵ������ͨ���ڸ���LCD
//		2007/02/24		V1.2  ΪС��Դ��51��Ƭ�������������򣬼��ٶ�RAM��ռ
//								��
//		2008/06/12		V2.0  �ü���ԭ�б��õ�������任�Ĺ���,����������
//								����ͳһ���ض���(��LCD_Config.h����),�޸�
//								PutChar����
//								�����ı���ʾ�ĸ�����ʾ����
//========================================================================
#include "LCD_Driver_User.h"		//LCD�ײ���������ͷ�ļ�
#include "LCD_Config.h"			//LCD�������ļ��������������Ƿ��õȵĶ���


//extern unsigned int Asii8[];				//6X8��ASII�ַ���
extern const unsigned char Asii0610[];			//6X10��ASII�ַ���
extern const unsigned char Asii0816[];			//8X16��ASII�ַ���

DOTBYTE X_Witch=5;					//�ַ�д��ʱ�Ŀ��
DOTBYTE Y_Witch=10;					//�ַ�д��ʱ�ĸ߶�
DOTBYTE Font_Wrod=10;					//�����ÿ����ģռ�ö��ٸ��洢��Ԫ��
const unsigned char *Char_TAB= Asii0610;		//�ֿ�ָ��
//unsigned char Plot_Mode;				//��ͼģʽ
LCDBYTE BMP_Color=0xff;
LCDBYTE Char_Color=1;

BYTE	Font_type=1;					//��ʶ�ַ�����
BYTE	Font_Cover=0;					//�ַ���ʾ��ģʽ
LCDBYTE	Char_BackColor=0;				//���ø���ģʽʱ,�ַ��ı���ɫ 


//��ֱ�ߺ�����s_x��s_yΪ��ʼ�㣬e_x��e_yΪ������
void Line(LCDBYTE s_x,LCDBYTE s_y,LCDBYTE e_x,LCDBYTE e_y);
//��ͼģʽ����ǰ��ͼ��ɫ����
void SetPaintMode(BYTE Mode,LCDBYTE Color);
//��׼�ַ����ã��������ͺ�ASCII��Ĵ�С�ߴ�
void LCD_FontSet(DOTBYTE Font_NUM);
//������ɫ������,������ǰ����ɫ
LCDBYTE LCD_FontColor(LCDBYTE Color);
//�����ַ���ʾ��ģʽ,�����ñ���ɫ
//void FontMode(BYTE Mode,LCDBYTE FontBackColor);
//��x��y��������д��һ����׼�ַ�
void PutChar(DOTBYTE x,DOTBYTE y,char a);
//��x��y������Ϊ��ʼд��һ����׼�ַ���
void PutString(DOTBYTE x,DOTBYTE y,char *p);
//��x��y������Ϊ���ģ�����һ����
void PutPixel(DOTBYTE x,DOTBYTE y);
//��x��y������Ϊ���ģ�����һ��Բ�߻�ʵ��Բ
void Circle(DOTBYTE x,DOTBYTE y,DOTBYTE r,BYTE mode);
//����һ����left��top��right��bottomΪ�����Խǵľ��ο����ʵ�ľ���
void Rectangle(DOTBYTE left, DOTBYTE top, DOTBYTE right, DOTBYTE bottom, BYTE Mode);
//����������ִ��ȫ��Ļ��������ǰ��ɫ
void ClrScreen(BYTE Mode);
//========================================================================
// ����: void FontSet(int Font_NUM)
// ����: �ı���������
// ����: Font_NUM ����ѡ��,�������������ֿ�Ϊ׼
//		 Color  �ı���ɫ,���������Դ��ֿ�  
// ����: ��
// ��ע: 
// �汾:
//      2006/10/15      First version
//========================================================================
void LCD_FontSet(DOTBYTE Font_NUM)
{
    switch(Font_NUM)
    {
        case 1: 
            Font_Wrod = 16;	//ASII�ַ�A
            X_Witch = 8;
            Y_Witch = 16;
            Font_type = 1;
            Char_TAB = Asii0816;
            break;
        case 0: 
            Font_Wrod = 10;	//ASII�ַ�B
            X_Witch = 5;
            Y_Witch = 10;
            Font_type = 1;
            Char_TAB = Asii0610;
            break;	
    }
}
//========================================================================
// ����: void LCD_FontColor(unsigned char color)
// ����: �ı���������
// ����: Font_NUM ����ѡ��,�������������ֿ�Ϊ׼
//		 Color  �ı���ɫ,���������Դ��ֿ�  
// ����: ԭ������ɫ
//========================================================================

LCDBYTE LCD_FontColor(LCDBYTE Color)
{
    LCDBYTE c=Char_Color;
    Char_Color = Color;
    Char_BackColor = (Color==0)?1:0;
    return c;
}
//========================================================================
// ����: void FontBackFill(int fill) 
// ���ñ�������ģʽ
// fill =1 : ����ģʽ�������屳��ɫ���
// fill =0 : ����ģʽ������䣬����ԭ������
// ��ע: �����ַ���ʾ��Ч
void LCD_FontBackFill(int fill)
{
  Font_Cover = fill;
}
//========================================================================
// ����: void PutChar(unsigned char x,unsigned char y,char a)  
// ����: д��һ����׼�ַ�
// ����: x  X������     y  Y������
//		 a  Ҫ��ʾ���ַ����ֿ��е�ƫ����  
// ����: ��
// ��ע: ASCII�ַ���ֱ������ASCII�뼴��
// �汾:
//      2006/10/15      First version
//		2007/01/11		V1.1
//		2008/06/12		V2.0
//========================================================================
void PutChar(DOTBYTE x,DOTBYTE y,char a)       
{
	DOTBYTE i,j;		//�����ݴ�
	const unsigned char *p_data;
	LCDBYTE Temp;
	BYTE Index = 0;
	if(Font_type==1)
		p_data = Char_TAB + (a-32)*Font_Wrod;
	else
		p_data = Char_TAB + a*Font_Wrod;	//Ҫд�ַ����׵�ַ
	j = 0;
	while((j ++) < Y_Witch)
	{
		if(y > Dis_Y_MAX) break;
		i = 0;
		while(i < X_Witch)
		{
			if((i&0x07)==0)
			{
//				Temp = *(p_data + (Index>>1));
//				if((Index&0x01)==0)Temp = Temp>>8; 
				Temp = *(p_data+Index);
				Index++;
			}
			if((Temp & 0x80) > 0) Writ_Dot/*Write_Dot_LCD*/((x+i),y,Char_Color);
			else if(Font_Cover) Writ_Dot/*Write_Dot_LCD*/((x+i),y,Char_BackColor);
			Temp = Temp << 1;
			if((x+i) >= Dis_X_MAX) 
			{
				Index += (X_Witch-i)>>3;
				break;
			}
			i++;
		}
		y ++;
	}
}

//========================================================================
// ����: void PutString(unsigned char x,unsigned char y,char *p)
// ����: ��x��yΪ��ʼ���괦д��һ����׼�ַ�
// ����: x  X������     y  Y������
//	 p  Ҫ��ʾ���ַ���  
// ����: ��
// ��ע: ���������Դ���ASCII�ַ�����ʾ
// �汾:
//      2006/10/15      First version
//========================================================================
void PutString(DOTBYTE x,DOTBYTE y,char *p)
{
	while(*p!=0)
	{
		PutChar(x,y,*p);
		x += X_Witch;
		if((x + X_Witch) > Dis_X_MAX)
		{
			x = 0;
			if((Dis_Y_MAX - y) < Y_Witch) break;
			else y += Y_Witch;
		}
		p++;
	}
}
//========================================================================
// ����: void SetPaintMode(int Mode,unsigned int Color)
// ����: ��ͼģʽ����
// ����: Mode ��ͼģʽ    Color  ���ص����ɫ,�൱��ǰ��ɫ  
// ����: ��
// ��ע: Mode��Ч
// �汾:
//      2006/10/15      First version
//========================================================================
void SetPaintMode(BYTE Mode,LCDBYTE Color)
{
	Mode = Mode;//Plot_Mode = Mode;		//������Ϊ�˱���������������һ���ԣ���ͼģʽ�ڸð�������δ��
        //BMP_Color = Color;
        //wen
        if(Color) 
          BMP_Color =0xff;
        else
          BMP_Color=0;
}
//========================================================================
// ����: void PutPixel(int x,int y)
// ����: ��x��y���ϻ���һ��ǰ��ɫ�ĵ�
// ����: x  X������     y  Y������
// ����: ��
// ��ע: ʹ��ǰ��ɫ
// �汾:
//      2006/10/15      First version
//========================================================================
void PutPixel(DOTBYTE x,DOTBYTE y)
{
	Writ_Dot/*Write_Dot_LCD*/(x,y,BMP_Color);
}
//========================================================================
// ����: void Line(unsigned char s_x,unsigned char s_y,unsigned char e_x,unsigned char e_y)
// ����: ��s_x��s_yΪ��ʼ���꣬e_x��e_yΪ�����������һ��ֱ��
// ����: x  X������     y  Y������
// ����: ��
// ��ע: ʹ��ǰ��ɫ
// �汾:
//      2006/10/15      First version
//========================================================================
void Line(LCDBYTE s_x,LCDBYTE s_y,LCDBYTE e_x,LCDBYTE e_y)
{  
	signed short Offset_x,Offset_y,Offset_k = 0;
	signed short Err_d = 1;
	if(s_y>e_y)
	{
		Offset_x = s_x;
		s_x = e_x;
		e_x = Offset_x;
		Offset_x = s_y;
		s_y = e_y;
		e_y = Offset_x;
	} 
	Offset_x = e_x-s_x;
	Offset_y = e_y-s_y;
	Writ_Dot(s_x,s_y,BMP_Color);
    if(Offset_x<0)
	{
		Offset_x = s_x-e_x;
       // Err_d = s_x;
       // s_x = e_x;
       // e_x = Err_d;
		Err_d = -1;
	}
    if(Offset_x==0)
    {
        while(s_y<e_y)
        {
            s_y++;
            if(s_y>Dis_Y_MAX) return;
            Writ_Dot/*Write_Dot_LCD*/(s_x,s_y,BMP_Color);
        }
        return;
    }
    else if(Offset_y==0)
    {
        while(s_x!=e_x)
        {
            s_x+=Err_d;
            if(s_x>Dis_X_MAX) return;
            Writ_Dot/*Write_Dot_LCD*/(s_x,s_y,BMP_Color);
        }
        return;
    }
	if(Offset_x>Offset_y)
	{
		Offset_k += Offset_y;
		while(s_x!=e_x)
		{
			if(Offset_k>0)
			{
				s_y+=1;
				Offset_k += (Offset_y-Offset_x);
			}
			else Offset_k += Offset_y; 
			s_x+=Err_d;
			if(s_x>Dis_X_MAX||s_y>Dis_Y_MAX) break;
			Writ_Dot/*Write_Dot_LCD*/(s_x,s_y,BMP_Color);
		}	
	}
	else
	{
		Offset_k += Offset_x;
		while(s_y!=e_y)
		{
			if(Offset_k>0)
			{
				s_x+=Err_d;
				Offset_k += (Offset_x-Offset_y);
			}
			else Offset_k += Offset_x;
			s_y+=1;
			if(s_x>Dis_X_MAX||s_y>Dis_Y_MAX) break;
			Writ_Dot/*Write_Dot_LCD*/(s_x,s_y,BMP_Color);
		}
	}    
}
#if LCD_DIS_CIRCLE==1
//========================================================================
// ����: void W_Red_Dot(unsigned char x,unsigned char y,char a,char b,unsigned char mode)
// ����: ����Բ�ĸ��������еĵ����
// ����: 
// ����: ��
// ��ע: �ú������û����ɼ���ʹ��ǰ��ɫ
// �汾:
//      2006/10/15      First version
//========================================================================
void W_Red_Dot(DOTBYTE x,DOTBYTE y,DISWORD a,DISWORD b,BYTE mode)
{
    if(mode > 0)
    {
       Line(x+a,y+b,x-a,y+b);
       Line(x+a,y-b,x-a,y-b);
    }
    else
    {
       PutPixel(x+a,y+b);
       PutPixel(x-a,y+b);
       PutPixel(x+a,y-b);
       PutPixel(x-a,y-b);
    }
}
//========================================================================
// ����: void W_Red_Err(int *a,int *b,int *r)
// ����: ��Բ������
// ����: 
// ����: ��
// ��ע: �ú������û����ɼ�
// �汾:
//      2006/10/16      First version
//========================================================================
void W_Red_Err(DISWORD *a,DISWORD *b,DOTBYTE *r)
{
	DISWORD R_Error;
	WORD uiTemp;
	R_Error = (*a+1)*(*a+1);
	uiTemp = (*b)*(*b);
	R_Error += uiTemp;
	uiTemp = (*r)*(*r);
	R_Error -= uiTemp;
	if(R_Error>=0)
	{
		R_Error = R_Error-*b;
		if(R_Error>=0) *b = *b-1;
	}
	*a = *a+1;
}
//========================================================================
// ����: void Circle(unsigned char x,unsigned char y,unsigned char r,unsigned char mode)
// ����: ��x,yΪԲ��RΪ�뾶��һ��Բ(mode = 0) or Բ��(mode = 1)
// ����: 
// ����: ��
// ��ע: ��Բ����ִ�н��������MCU�п��Ź����������幷�Ĳ���
// �汾:
//      2006/10/16      First version
//========================================================================
void Circle(DOTBYTE x,DOTBYTE y,DOTBYTE r,BYTE mode)
{
	DISWORD arx1=0,ary1,arx2,ary2=0;
//	Pos_Switch(&x,&y);						//����任
//	x += 4;
	ary1=r;
	arx2=r;
	while(1)
	{
        W_Red_Dot(x,y,arx1,ary1,mode);
        W_Red_Err(&arx1,&ary1,&r);
		if(arx1 == arx2)
		{
			W_Red_Dot(x,y,arx1,ary1,mode);
			break; 
		}
		W_Red_Dot(x,y,arx2,ary2,mode);
		W_Red_Err(&ary2,&arx2,&r);
		if(arx1 == arx2) 
		{
			W_Red_Dot(x,y,arx2,ary2,mode);
			break;
		}
	}
}	
#endif
//========================================================================
// ����: void Rectangle(DOTBYTE left, DOTBYTE top, DOTBYTE right, 
//						DOTBYTE bottom, BYTE Mode)
// ����: ��x,yΪԲ��RΪ�뾶��һ��Բ(mode = 0) or Բ��(mode = 1)
// ����: left - ���ε����ϽǺ����꣬��Χ0��118
//		 top - ���ε����Ͻ������꣬��Χ0��50
//		 right - ���ε����½Ǻ����꣬��Χ1��119
//		 bottom - ���ε����½������꣬��Χ1��51
//		 Mode - ����ģʽ��������������ֵ֮һ��
//				0:	���ο򣨿��ľ��Σ�
//				1:	�����棨ʵ�ľ��Σ�
// ����: ��
// ��ע: ��Բ����ִ�н��������MCU�п��Ź����������幷�Ĳ���
// �汾:
//      2005/05/21      First version
//========================================================================
void Rectangle(DOTBYTE left, DOTBYTE top, DOTBYTE right, DOTBYTE bottom, BYTE Mode)
{
	DOTBYTE uiTemp;
	
	if(Mode==0)
	{
		Line(left,top,left,bottom);
		Line(left,top,right,top);
		Line(right,bottom,left,bottom);
		Line(right,bottom,right,top);
	}
	else
	{
//		Pos_Switch(&left,&top);						// ����任
//		Pos_Switch(&right,&bottom);					//����任
		if(left>right)
		{
			uiTemp = left;
			left = right;
			right = uiTemp;
		}
		if(top>bottom)
		{
			uiTemp = top;
			top = bottom;
			bottom = uiTemp;
		}
		for(uiTemp=top;uiTemp<=bottom;uiTemp++)
		{
			Line(left,uiTemp,right,uiTemp);
		}
	}
}
//========================================================================
// ����: void ClrScreen(BYTE Mode)
// ����: ����������ִ��ȫ��Ļ��������ǰ��ɫ
// ����: Mode   0:ȫ������
//		1:ȫ�����ǰ��ɫ
// ����: ��
// ��ע: 
// �汾:
//      2007/01/11      First version
//========================================================================
//������������⣬����֮������ͼ������ ???
void ClrScreen(BYTE Mode)
{
	if(Mode==0)
		LCD_Fill(LCD_INITIAL_COLOR);
	else
		LCD_Fill(BMP_Color);
}

