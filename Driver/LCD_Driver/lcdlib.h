//2013.1.14
#include "stm32f10x.h"
////LCD ���
#ifndef _LCD_H
#define _LCD_H

#define LCD_VERTICAL  
//#define LCD_FONT12

//0-76800*4 in external sram as frame buffer
#define LCD_GUI               (volatile u32 *)0x68000000     //

//---------- �û�������Ե��õĳ����ͺ��� --------------------
//LCD��Ļ�ֱ��ʣ����ص���
#ifdef LCD_VERTICAL 
#define MAX_X                 320
#define MAX_Y                 240
#else
#define MAX_X                 240
#define MAX_Y                 320
#endif


// ���ó�����ɫ�궨�� (��ʽ: BBGGRR, )
#define   LCD_BLACK		0x000000      /* ��ɫ��    0,   0,   0 */
#define   LCD_NAVY		0x7F0000      /* ����ɫ��  0, 128,   0 */
#define   LCD_DGREEN	        0x007F00      /* ����ɫ��  0, 128,   0 */
#define   LCD_DCYAN		0x7F7F00      /* ����ɫ��  128, 128, 0 */
#define   LCD_MAROON	        0x00007F      /* ���ɫ��  0,   0,  128*/
#define   LCD_PURPLE	        0x7F007F      /* ��ɫ��  128,   0, 128 */
#define   LCD_OLIVE		0x007F7F      /* ����̣�  0, 128, 128 */
#define   LCD_LGRAY		0xC0C0C0      /* �Ұ�ɫ��192, 192, 192 */
#define   LCD_DGRAY		0x7F7F7F      /* ���ɫ��128, 128, 128 */
#define   LCD_BLUE		0xFF0000      /* ��ɫ��  255,  0,   0  */
#define   LCD_GREEN		0x00FF00      /* ��ɫ��    0, 255,  0 */
#define   LCD_CYAN	        0xFFFF00      /* ��ɫ��  255, 255,  0 */
#define   LCD_RED		0x0000FF      /* ��ɫ��    0,  0,  255 */
#define   LCD_MAGENTA	        0xFF00FF      /* Ʒ�죺  255,   0, 255 */
#define   LCD_YELLOW	        0x00FFFF      /* ��ɫ��    0, 255, 255 */
#define   LCD_WHITE		0xFFFFFF      /* ��ɫ��  255, 255, 255 */


//---- LCD �ײ㺯��
//LCD������ݽṹ�ĳ�ʼ��
void LCD_Init(void);

//---- �����ն˷�ʽ����ַ�������
int LCD_Printf(char* format,...);   //��ʽ���ַ������,������ʽͬC���Ե�printf()
void LCD_Goto(u32 row, u32 col);    //ָ����һ���ַ����λ��(����)

//---- ͼ�η�ʽ�ı����
//�趨�ı���ɫ  
void gTextColor(u32 color);           
//�趨�ı�������ɫ  
void gTextBkColor(u32 color);         
//�趨�ı��������ģʽ fill=1: ��䱳��ɫ   
void gTextBackFill(u32 fill);    
//����ǰָ������ɫ�Ը�����������Ϊ���Ͻǵ�λ�ÿ�ʼ����ַ���
void gTextOut(u32 x, u32 y,char *ch);                         
//����ǰָ������ɫ�Ը�����������Ϊ���Ͻǵ�λ�ÿ�ʼ�����ʽ���ַ���
void gFormatTextOut(u32 x, u32 y, char* format,...);          
//����ǰָ������ɫ,���������С�Ը�����������Ϊ���Ͻǵ�λ�ÿ�ʼ����ַ���
void gTextOut2(u32 x, u32 y,char *ch);                         
//����ǰָ������ɫ,���������С�Ը�����������Ϊ���Ͻǵ�λ�ÿ�ʼ�����ʽ���ַ���
void gFormatTextOut2(u32 x, u32 y, char* format,...);          

//---- 2-D ͼ�λ��ƺ���
//�ø�����ɫ���LCD�ͻ�ͼ������
void gClearScreen(u32 color);               
//��ָ������ɫ��ָ�����껭һ�����ص�
void gPoint(u32 x,u32 y,u32 color);                            
//��ָ������ɫ����������֮�仭һ��ֱ��
void gLine(u32 x0,u32 y0,u32 x1,u32 y1, u32 color);    
//��ָ������ɫ��np������֮�仭һ������
void gLineSegs(u32 const *points,u32 color,u32 np);    
//��ָ������ɫ����������֮�仭һ������
void gRect(int x0,int y0,int x1,int y1,u32 color);     
//��ָ������ɫ����������֮�仭һ��������
void gRectFill(int x0,int y0,int x1,int y1, u32 color);       
//��ָ������ɫ���������귶Χ�ڻ�һ����Բ
void gEllipse(u32 x0, u32 y0, u32 x1, u32 y1,u32 color);      
//��ָ������ɫ���������귶Χ�ڻ�һ��ʵ����Բ
void gEllipseFill(u32 x0, u32 y0, u32 x1, u32 y1, u32 color); 
//��ָ������ɫ�Ը���Բ������Ͱ뾶��һ��Բ
void gCircle(u32 x0, u32 y0, u32 r, u32 color);               
//��ָ������ɫ�Ը���Բ������Ͱ뾶��һ��ʵ��Բ
void gCircleFill(u32 x0, u32 y0, u32 r, u32 color);  


//�û������е�32λ��ɫ�������LCD��һ����������
void gRectCopy( u32 x1,u32 y1, u32 x2, u32 y2,u32 *pbuf);
//�û������е�rgb�ֽ��������LCD��һ����������
void gRectCopyRGB( u32 x1,u32 y1, u32 x2, u32 y2, u8 *pbuf);


//---- ��ʾBMPͼ��
//�ڸ�������Ϊ���Ͻǵľ���������ʾBMP��ʽͼ��
int gBMPDraw(int x0, int y0,const void * pBMP ); 
//�õ�����BMPͼ���X����ߴ�
int gBMPGetXSize(const void * pBMP) ;
//�õ�����BMPͼ���Y����ߴ�
int gBMPGetYSize(const void * pBMP) ;

//jpeg
//�õ�jpegͼ���x�ߴ�
int gJPEGGetXSize(const void * pFileData, int DataSize);
//�õ�jpegͼ���x�ߴ�
int gJPEGGetYSize(const void * pFileData, int DataSize);
//�ڸ�������Ϊ���Ͻǵľ���������ʾjpegͼ��
int gJPEGDraw(const void * pFileData, int DataSize, int x0, int y0);

#endif