#ifndef __LCD_CONFIG_H__
#define __LCD_CONFIG_H__

typedef	unsigned char LCDBYTE;			//�ض����й�LCD��,Ҳ��LCD���е�����λ��
typedef unsigned char DOTBYTE;			//�ض����й�LCD��,Ҳ��LCD�ĵ�������
typedef signed short  DISWORD;			//�ض�����LCD_Dis.c�ļ���һЩ�м䴦������ݵĿ��

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   BOOL;

#define LCD_X_MAX			128-1			//��Ļ��X���������
#define LCD_Y_MAX			64-1			//��Ļ��Y���������
			
#define LCD_XY_Switch		0				//��ʾʱX���Y�ɽ���
#define LCD_X_Rev			0				//��ʾʱX�ᷴת
#define LCD_Y_Rev			0				//��ʾʱY�ᷴת

#if LCD_XY_Switch == 0
	#define Dis_X_MAX		LCD_X_MAX
	#define Dis_Y_MAX		LCD_Y_MAX	
#endif

#if LCD_XY_Switch == 1
	#define Dis_X_MAX		LCD_Y_MAX
	#define Dis_Y_MAX		LCD_X_MAX	
#endif

#define LCD_INITIAL_COLOR	0x00			//����LCD����ʼ��ʱ�ı���ɫ

#define LCD_DIS_CIRCLE		1				//�����Ƿ���Ҫ����Բ�εĹ���

//========================================================================
// ����: void Pos_Switch(unsigned int * x,unsigned int * y)
// ����: �����������任Ϊʵ��LCD��������ϵ���Ա��ڿ��ٻ�Բ���Լ�����
// ����: x  X������     y  Y������      
// ����: �� 
// ��ע: �����Լ�֮ǰ������x��y����ϵ�����û���ģ�������ʵ��LCD��������ϵ
//		 �������ṩ�ɽ�������任�Ľӿ�
// �汾:
//      2006/10/15      First version
//========================================================================
//========================================================================
// ����: void Writ_Dot(int x,int y,unsigned int Color)
// ����: �����x,yΪ���������
// ����: x  X������     y  Y������      Color  ������ɫ 
// ����: ��
// ��ע: �����Լ�֮ǰ������x��y����ϵ�����û���ģ�������ʵ��LCD��������ϵ
//		 �������ṩ�ɽ�������任�Ľӿ�
// �汾:
//      2006/10/15      First version
//========================================================================
//========================================================================
// ����: unsigned int Get_Dot(int x,int y)
// ����: ��ȡx,yΪ���������
// ����: x  X������     y  Y������      
// ����: Color  ������ɫ 
// ��ע: �����Լ�֮ǰ������x��y����ϵ�����û���ģ�������ʵ��LCD��������ϵ
//		 �������ṩ�ɽ�������任�Ľӿ�
// �汾:
//      2006/10/15      First version
//========================================================================
//========================================================================
// ����: void Set_Dot_Addr(int x,int y)
// ����: ���õ�ǰ��Ҫ���������ص�ַ
// ����: x  X������     y  Y������      
// ����: �� 
// ��ע: �����Լ�֮ǰ������x��y����ϵ�����û���ģ�������ʵ��LCD��������ϵ
//		 �������ṩ�ɽ�������任�Ľӿ�
// �汾:
//      2006/10/15      First version
//========================================================================
#if LCD_XY_Switch==0&&LCD_X_Rev==0&&LCD_Y_Rev==0
	#define Pos_Switch(x,y)	
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(x,y,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(x,y)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(x,y)	
#endif
#if LCD_XY_Switch==0&&LCD_X_Rev==1&&LCD_Y_Rev==0
	#define Pos_Switch(x,y)			x = Dis_X_MAX-x
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(Dis_X_MAX-x,y,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(Dis_X_MAX-x,y)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(Dis_X_MAX-x,y)		
#endif
#if LCD_XY_Switch==0&&LCD_X_Rev==0&&LCD_Y_Rev==1
	#define Pos_Switch(x,y)			y = Dis_Y_MAX-y	
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(x,Dis_Y_MAX-y,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(x,Dis_Y_MAX-y)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(x,Dis_Y_MAX-y)		
#endif
#if LCD_XY_Switch==0&&LCD_X_Rev==1&&LCD_Y_Rev==1
	#define Pos_Switch(x,y)			x = Dis_X_MAX-x;\
									y = Dos_Y_MAX-y	
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(Dis_X_MAX-x,Dis_Y_MAX-y,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(Dis_X_MAX-x,Dis_Y_MAX-y)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(Dis_X_MAX-x,Dis_Y_MAX-y)	
#endif

#if LCD_XY_Switch==1&&LCD_X_Rev==0&&LCD_Y_Rev==0
	#define Pos_Switch(x,y)			x = x+y;\
									y = x-y;\
									x = x-y;
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(y,x,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(y,x)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(y,x)		
#endif
#if LCD_XY_Switch==1&&LCD_X_Rev==1&&LCD_Y_Rev==0
	#define Pos_Switch(x,y)			x = Dis_X_MAX-x;\
									x = x+y;\
									y = x-y;\
									x = x-y;
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(y,Dis_X_MAX-x,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(y,Dis_X_MAX-x)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(y,Dis_X_MAX-x)		
#endif
#if LCD_XY_Switch==1&&LCD_X_Rev==0&&LCD_Y_Rev==1
	#define Pos_Switch(x,y)			y = Dis_Y_MAX-y;\
									x = x+y;\
									y = x-y;\
									x = x-y;
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(Dis_Y_MAX-y,x,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(Dis_Y_MAX-y,x)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(Dis_Y_MAX-y,x)		
#endif
#if LCD_XY_Switch==1&&LCD_X_Rev==1&&LCD_Y_Rev==1
	#define Pos_Switch(x,y)			x = Dis_X_MAX-x;\
									y = Dis_Y_MAX-y;\
									x = x+y;\
									y = x-y;\
									x = x-y;
	#define Writ_Dot(x,y,c)			Write_Dot_LCD(Dis_Y_MAX-y,Dis_X_MAX-x,c)
	#define Get_Dot(x,y)			Get_Dot_LCD(Dis_Y_MAX-y,Dis_X_MAX-x)
	#define Set_Dot_Addr(x,y)		Set_Dot_Addr_LCD(Dis_Y_MAX-y,Dis_X_MAX-x)		
#endif
//
#define Match_xy(s,e)	if(s>e){s = s+e;e = s-e;s = s-e;}

#endif
