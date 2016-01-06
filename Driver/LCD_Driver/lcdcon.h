////LCD ���
#ifndef _LCD_H
#define _LCD_H

//Ԥ����ĳ����ı���ɫ���������õ���ɫ��Χ��0-0xffffff
//����ָ����Щ��ɫΪǰ���򱳾�ɫ
#define LCD_BLACK 	0x000000                  //��
#define LCD_RED		0x0000ff                  //��
#define LCD_GREEN 	0x00ff00                  //��
#define LCD_BLUE	0xff0000                  //��
#define LCD_YELLOW	(LCD_RED+LCD_GREEN)       //��
#define LCD_MAGENTA	(LCD_RED+LCD_BLUE)        //��
#define LCD_CYAN	(LCD_BLUE+LCD_GREEN)      //��
#define	LCD_WHITE	0xffffff                  //��

//setup

//LCD������ݽṹ�ĳ�ʼ��
void LCD_Init(void);

//��ָ������ɫ�����Ļ
void LCD_ClearScr(u32 color);

//�趨���ֵ���ɫ
void LCD_SetColor(u32 color);

//���ñ�����ɫ
void LCD_SetBkColor(u32 color);

//text functions

//�ƶ���ָ��������λ�ã���һ���ַ��������λ�ÿ�ʼ��ʾ
//�� row=0-19,  �� col=0-29
void LCD_Goto(u32 row, u32 col);	//row=0..19; col=0..29

//��ʽ�����ַ��������������ʽͬC���Ե�printf()
int LCD_Printf(char* format,...);


#endif