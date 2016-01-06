//========================================================================
// �ļ���: LCD_Driver_User.c
// ��  ��: Xinqiang Zhang(email: Xinqiang@Mzdesign.com.cn)
//			www.Mzdesign.com.cn
// ��  ��: 2008/06/11
// ��  ��: �ײ�ӿ�������չ����
//
// ��  ��: ��ο�������ʾ����Ӳ������˵���������DataSheet,���Դ�����д��
//			��ӿ����������Լ�LCD�ĳ�ʼ������
// ��  ��:
//      2007/02/25      First version    Mz Design
//		2008/06/11		V2.0			 Mzdesign
// ע  ��: 
//			�������������кܶ��Ѿ������ε����ӳ����û��ɸ����Լ���Ӧ��
//			ȡ�����ϲ���һ�����޸ģ�������ܵĻ���
//			���ļ���LCD�������й�,ΪMzDesignͨ�ð�LCD������V2.0�汾,�ں���
//			�����Ǳ�׼�ĺ���,ÿ��LCD��Ӧ��ͨ�ð�LCD�������������һ����,ֻ
//			�ǻ���Բ�ͬ��LCDģ�����һЩ����,�û����Բο����ǽ���V1.0�汾
//			��������<LCD�����Կ�ԭ��>.
// ��  ��: ����Ϊ���MzDesign��MzL02 128X64����LCD��������ֲ��ȥ,������MCS-51
//			ϵ��MCU
//========================================================================
#include "LCD_Config.h"
#include "LCD_PortConfig.h"
//#include <stdio.h>
LCDBYTE LCD_DataRead(void);
void LCD_RegWrite(LCDBYTE Command);
void LCD_DataWrite(LCDBYTE Dat);
void Write_Dot_LCD(LCDBYTE x,LCDBYTE y,LCDBYTE Color);
//LCDBYTE Get_Dot_LCD(WORD x,WORD y);
//void Clear_Dot_LCD(WORD x,WORD y);
void Set_Dot_Addr_LCD(LCDBYTE x,LCDBYTE y);
void LCD_Fill(LCDBYTE a);
//void LCD_Fill_s(WORD Number,LCDBYTE Color);
LCDBYTE LCD_ReadStatus(void);
LCDBYTE LCD_TestStatus(LCDBYTE bitMatch);
void TimeDelay(unsigned int Time);
void LCD_Init(void);
LCDBYTE LCDDATA[128][9];
//========================================================================
// ����: void LCD_DataWrite(unsigned int Data)
// ����: дһ���ֽڵ���ʾ������LCD�е���ʾ����RAM����
// ����: Data д������� 
// ����: ��
// ��ע: Mz ͨ�ð�LCD�������� ��׼�Ӻ���
// �汾:
//      2007/01/09      First version
//========================================================================
void LCD_DataWrite(LCDBYTE Dat)
{
    LCD_Data_BUS = Dat;
}
//========================================================================
// ����: LCDBYTE LCD_DataRead(void)
// ����: ��LCD�е���ʾ����RAM���ж�һ���ֽڵ���ʾ����
// ����: ��  
// ����: ���������ݣ�
// ��ע: Mz ͨ�ð�LCD�������� ��׼�Ӻ��� 
// �汾:
//      2007/01/09      First version
// ע��: 
//========================================================================
LCDBYTE LCD_DataRead(void)
{
//    LCDBYTE Read_Data;
//    Read_Data = LCD_Data_BUS;
    return LCD_Data_BUS;
}	
//========================================================================
// ����: void LCD_RegWrite(LCDBYTE Command)
// ����: дһ���ֽڵ�������LCD�еĿ��ƼĴ�������
// ����: Command		д������ݣ��Ͱ�λ��Ч��byte�� 
// ����: ��
// ��ע: Mz ͨ�ð�LCD�������� ��׼�Ӻ���
// �汾:
//      2007/01/09      First version
//========================================================================
void LCD_RegWrite(LCDBYTE Command)
{
    LCD_CMD_BUS = Command;
}
//========================================================================
// ����: LCDBYTE LCD_ReadStatus(void)
// ����: ��ȡLCDģ���ָ��״̬λ
// ����: ��
// ����: ��ȡ����״̬λ
// ��ע: Mz ͨ�ð�LCD�������� ��׼�Ӻ���
// �汾:
//      2007/01/09      First version
//========================================================================
LCDBYTE LCD_ReadStatus(void)
{
	return LCD_CMD_BUS;
}
//========================================================================
// ����: void LCD_TestStatus(LCDBYTE bitMatch)
// ����: ����LCDģ���ָ��״̬λ
// ����: bitMatch
// ����: �������Ч�򷵻�1  �糬ʱ�򷵻�0
// ��ע: Mz ͨ�ð�LCD�������� ��׼�Ӻ���
// �汾:
//      2007/01/09      First version
//========================================================================
LCDBYTE LCD_TestStatus(LCDBYTE bitMatch)
{
	bitMatch = bitMatch;
	return 1;			//return 1  as test status bit ok
}
//========================================================================
// ����: void Write_Dot_LCD(int x,int y,unsigned int i)
// ����: ��LCD����ʵ����ϵ�ϵ�X��Y��������ɫΪi�ĵ�
// ����: x 		X������
//	 y 		Y������
//	 i 		Ҫ���ĵ����ɫ 
// ����: ��
// ��ע: Mz ͨ�ð�LCD�������� ��׼�Ӻ���
// �汾:
//      2006/10/15      First version
//		2007/01/09		V1.2 
//========================================================================
void Write_Dot_LCD(DOTBYTE x,DOTBYTE y,LCDBYTE i)
{
	DOTBYTE x_low,x_hight;		//�����е�ַ�ĸߵ�λָ��
	DOTBYTE Dot_Mask_Buf=0x01;
	DOTBYTE y_Page;			//���ڴ��Ҫ�����λ�����ڵ�byte����λ��
        
        if(x>127) x=127;
        if(y>63) y=63;
        
	x = x+1;
	x_low = (x&0x0f);		//��λ�е�ַ���õĵ�λָ��
	x_hight = ((x>>4)&0x0f)+0x10;	//��λ�е�ַ���õĸ�λָ��
//	y_Page = y&0x07;		//ȡ����λ
        
        Dot_Mask_Buf = 1<<(y&0x07);     //wen
//	switch(y&0x07)
//	{
//		case 0: Dot_Mask_Buf = 0x01;break;
//		case 1: Dot_Mask_Buf = 0x02;break;
//		case 2: Dot_Mask_Buf = 0x04;break;
//		case 3: Dot_Mask_Buf = 0x08;break;
//		case 4: Dot_Mask_Buf = 0x10;break;
//		case 5: Dot_Mask_Buf = 0x20;break;
//		case 6: Dot_Mask_Buf = 0x40;break;
//		case 7: Dot_Mask_Buf = 0x80;break;
//	}
	y_Page = (y>>3)+0xb0;		//Get the page of the byte
	LCD_RegWrite(y_Page);
	LCD_RegWrite(x_low);
	LCD_RegWrite(x_hight);
//	LCD_RegWrite(0xe0);		//����Ϊ��д��ģʽ
//	y_Page = LCD_DataRead();	//
//	y_Page = LCD_DataRead();	//
//      y_Page = LCD_DataRead();
        y_Page = LCDDATA[x][(y>>3)];//+0xb0]; //wen
	if(i) y_Page |= Dot_Mask_Buf;
	else y_Page &= ~Dot_Mask_Buf;
        //LCDDATA[x][(y>>3)+0xb0]=y_Page;
        LCDDATA[x][(y>>3)]=y_Page;            //wen
	LCD_DataWrite(y_Page);	//
        //LCD_DataWrite(Dot_Mask_Buf);	
//	LCD_RegWrite(0xee);		//��д��ģʽ����
}
/*
//========================================================================
// ����: void Clear_Dot_LCD(WORD x,WORD y)
// ����: �����LCD����ʵ����ϵ�ϵ�X��Y��
// ����: x 		X������
//		 y 		Y������
// ����: ��
// ��ע: �ݲ�ʹ�øú���
// �汾:
//      2006/10/15      First version
//========================================================================
void Clear_Dot_LCD(WORD x,WORD y)
{
	//�ɸ����Լ�����Ҫ��д����
	x = y;						//�����壬��Ϊ����ϵͳ����ʾ����
}
//========================================================================
// ����: unsigned int Get_Dot_LCD(int x,int y)
// ����: ��ȡ��LCD����ʵ����ϵ�ϵ�X��Y���ϵĵ�ǰ���ɫ����
// ����: x 		X������
//		 y 		Y������
// ����: �õ����ɫ
// ��ע: �ݲ�ʹ�øú���
// �汾:
//      2006/10/15      First version
//========================================================================
LCDBYTE Get_Dot_LCD(DOTBYTE x,DOTBYTE y)
{
	LCDBYTE Read_Data;
	//�ɸ����Լ�����Ҫ��д����
	x = y;						//�����壬��Ϊ����ϵͳ����ʾ����
	return Read_Data;
}	   */

//========================================================================
// ����: void Set_Dot_Addr_LCD(int x,int y)
// ����: ������LCD����ʵ����ϵ�ϵ�X��Y���Ӧ��RAM��ַ
// ����: x 		X������
//	 y 		Y������
// ����: ��
// ��ע: �����õ�ǰ������ַ��Ϊ�����������������׼��
//		 Mz ͨ�ð�LCD�������� ��׼�Ӻ���
// �汾:
//      2006/10/15      First version
//	2007/01/09	V1.2 
//========================================================================
//void Set_Dot_Addr_LCD(DOTBYTE x,DOTBYTE y)
//{
//	DOTBYTE x_low,x_hight;			//�����е�ַ�ĸߵ�λָ��
//	DOTBYTE y_Page;				//���ڴ��Ҫ�����λ�����ڵ�byte����λ��
//	x = x+4;
//	x_low = (x&0x0f);			//��λ�е�ַ���õĵ�λָ��
//	x_hight = ((x>>4)&0x0f)+0x10;		//��λ�е�ַ���õĸ�λָ��
//	y_Page = (y>>3)+0xb0;			//Get the page of the byte
//	LCD_RegWrite(y_Page);
//	LCD_RegWrite(x_low);
//	LCD_RegWrite(x_hight);
//}
/*
//========================================================================
// ����: void LCD_Fill_s(WORD Number,LCDBYTE Color)
// ����: ���������Colorɫ����Number����
// ����: Number ��������    Color  ���ص����ɫ  
// ����:
// ��ע: �ú������ܻ�ʹ�õ�
// �汾:
//      2006/10/15      First version
//		2007/01/09		V1.2 
//========================================================================
void LCD_Fill_s(WORD Number,LCDBYTE Color)
{
	while(Number!=0)
	{
		LCD_DataWrite(Color);						//
		Number--;
	}
} */
//========================================================================
// ����: void LCD_Fill(unsigned int Data)
// ����: ���������Data��������������
// ����: Data   Ҫ������ɫ����
// ����: ��
// ��ע: Mz ͨ�ð�LCD�������� ��׼�Ӻ���
// �汾:
//      2006/10/15      First version
//		2007/01/09		V1.2 
//========================================================================
void LCD_Fill(LCDBYTE Data)
{
	DOTBYTE i,j=0;
	DOTBYTE uiTemp;  
        uiTemp = Dis_Y_MAX;
	uiTemp = uiTemp>>3;
	for(i=0;i<=uiTemp;i++)			//��LCD������ʼ������ʾ����
	{
		LCD_RegWrite(0xb0+i);
		LCD_RegWrite(0x00 +1);
		LCD_RegWrite(0x10);
		for(j=0;j<=Dis_X_MAX;j++)
		{
			LCD_DataWrite(Data);
		}
	}
        
	LCD_RegWrite(0xe0);
        //--- wen
        for(int i=0;i<128;i++)
          for(int j=0;j<8;j++)
            LCDDATA[i][j]=Data;
}
//========================================================================
// ����: void LCD_PortInit(void)
// ����: ��LCD���ӵĶ˿ڳ�ʼ������
// ����: ��  
// ����: ��
// ��ע: Mz ͨ�ð�LCD�������� ��׼�Ӻ���
// �汾:
//      2007/01/09      First version
// ע��: 
//========================================================================
//void LCD_PortInit(void)
//{
//	LCD_A0 = 1;
//	LCD_EP = 1;
//	LCD_CS = 1;
//	LCD_WR = 1;
//	LCD_RE = 1;
//}
//========================================================================
// ����: void LCD_Init(void)
// ����: LCD��ʼ����������������LCD��ʼ����Ҫ���õ����Ĵ������������
//		 �û����˽⣬����鿴DataSheet���и����Ĵ���������
// ����: �� 
// ����: ��
// ��ע: Mz ͨ�ð�LCD�������� ��׼�Ӻ���
// �汾:
//      2006/10/15      First version
//		2007/01/09		V1.2 
//========================================================================
//��ʱ����

void TimeDelay(unsigned int Time)
{
	unsigned int i;
	while(Time > 0)
	{
		for(i = 0;i < 1600;i++) //1600
		{
			asm("nop");
		}
		Time --;
	}
}
void LCD_Init(void)
{
	//LCD������ʹ�õ��Ķ˿ڵĳ�ʼ��������б�Ҫ�Ļ���
	//LCD_PortInit();
	//LCD_RE = 0;
       
	TimeDelay(200);
	//LCD_RE = 1;
	//TimeDelay(20);
	//����LCD�ڲ��Ĵ���
        LCD_RegWrite(0xe2);
        TimeDelay(200);
	LCD_RegWrite(0xaf);
        TimeDelay(200);//LCD On
        LCD_RegWrite(0x2f);//�����ϵ����ģʽ;	
        TimeDelay(200);//LCD On 

	LCD_RegWrite(0x81);//��������ģʽ����ʾ���ȣ�
	LCD_RegWrite(0x0f);//��������0x0000~0x003f
	
	LCD_RegWrite(0x27);//V5�ڲ���ѹ���ڵ�������
	LCD_RegWrite(0xa2);//LCDƫѹ���ã�V3ʱѡ
	
	LCD_RegWrite(0xc8);//Com ɨ�跽ʽ����
	LCD_RegWrite(0xa0);//Segment����ѡ��
        LCD_RegWrite(0xa5);//ȫ���䰵ָ��
	LCD_RegWrite(0xa4);//ȫ������/�䰵ָ��
	LCD_RegWrite(0xa6);//��������ʾ����ָ��
	
	LCD_RegWrite(0xac);//�رվ�ָ̬ʾ��
	LCD_RegWrite(0x00);
	
	LCD_RegWrite(0x40+32);//������ʾ��ʼ�ж�ӦRAM
	LCD_RegWrite(0xe0);//ָ������
	LCD_Fill(LCD_INITIAL_COLOR);
}

