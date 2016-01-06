/*

*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sys.h"
#include "eios.h"
#include "time.h"

//for ucos
#include "includes.h"

int __low_level_init(void)
{
  SystemInit();
  return 1;
}
//123
//--------------------------------------------
void UART_SendByte(char c);
void UART_SendStr(char *s);


#define STKSIZE 128
#define STACKSIZE 128

#define hang_low 5
#define lie_low 7

//#define hang_medium 10
//#define lie_medium 14

#define hang_high 10
#define lie_high 14
//#define hang 5
//#define lie 7

#define GAME_MAIN_PAGE 0
#define GAME_START 1
#define GAME_WIN 2
#define GAME_END 3
#define GAME_HELP 4
#define GAME_SET 5


volatile int quit=0;
//����֮��Ķ����Ƹ�ʽͼƬ
extern const char data_8_bmp[];
extern const char data_4_bmp[];
extern const char data_smile_bmp[];
extern const char data_cry_bmp[];
extern const char data_cry1_bmp[];
extern const char data_smile1_bmp[];
void PBInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  // RCC���򿪰�������˿�BPIOB��ʱ���Լ� AFIOģ��ʱ�� !!  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO, ENABLE);

  // GPIO : ������ PB0����Ϊ���ָ������� input floating 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;//
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 
  // GPIO : ����PB0��Ϊ���ж��ߣ�����PA0,PB0,...PG0��ѡ��һ����ΪEXTI0�� 
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
  
  
  // EXTI : �����ж�EXTI0���ж�
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;            //ѡ��EXTI0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   //��Ϊ�ж϶������¼���Դ
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //�½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;             // �����ж�
  EXTI_Init(&EXTI_InitStructure);

  // NVIC:  ����EXTI0��IRQͨ���Ĳ���
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;            //ѡ��EXTI0ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0; //�������ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;        //�������ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //������ж�ͨ��

  NVIC_Init(&NVIC_InitStructure); 
 
}


///******************������***************///
OS_STK StackStart[STKSIZE];
void TaskStart(void *arg);

OS_STK Task_Clock_Stack[STACKSIZE];
void Task_Clock(void *Id);

OS_STK Task_Game_Start_Stack[STACKSIZE]; 		//task1 stack
void Task_Game_Start(void *Id);

//OS_STK Task_Game_Win_Stack[STACKSIZE]; 		//task2 stack

OS_EVENT *QSem;                //��Ϣ����ָ��
void     *QMsgTbl[50];         //��Ϣָ������

//void Game_Start();
void Game_Win();
void Game_Start_Draw_Bg();
void Draw_Main_Page();
//int delay; 
char key;

int game_state;

int game_pause;

int sound;

int level;

int hang,lie;

int timer;

int select;

int step;

int src_x,src_y,src_w,src_h,box_w,box_h;

int map[hang_high][lie_high];

int select_j,select_i;



void Show_Author(){	
  gClearScreen(LCD_BLUE);
  gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(90,50,"ͨ��1110��");
     gTextOut2(40,100,"��ΰ");
    gTextOut2(160,100,"11274004");
    gTextOut2(40,150,"��ʿ��");
    gTextOut2(160,150,"11253049");
 

}

void Draw_Main_Page_Bg(){
  
    gClearScreen(LCD_BLUE);
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_RED);
    gTextOut2(70,40,"��  ��  ��  ��");
    select=0;
    Draw_Main_Page();
}

void Draw_Main_Page(){
  
  if(select==0){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(100,80,"��ʼ��Ϸ");
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(100,80,"��ʼ��Ϸ");
  }
  
  if(select==1){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(100,120,"������Ϸ");
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(100,120,"������Ϸ");
  }
  
  if(select==2){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(100,160,"��Ϸ����");
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(100,160,"��Ϸ����");
  }
  
  if(select==3){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(100,200,"��Ϸ����");
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(100,200,"��Ϸ����");
  }
  
   
}

void Draw_Help(){
  gClearScreen(LCD_BLUE);
  gTextBkColor(LCD_BLUE);
  gTextColor(LCD_YELLOW);
  gTextOut(30,60,"�����з�����Ц����");
  gTextOut(30,120,"����˵����");
  gTextOut(30,140,"2 8 4 6����������");
  gTextOut(30,160,"5��ȷ��");
  gTextOut(30,180,"7������");
}

void Draw_Set(){
  if(select==0){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    if(sound==0)
      gTextOut2(100,80,"��������");
    else if(sound==1)
      gTextOut2(100,80,"��������");
   
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    if(sound==0)
      gTextOut2(100,80,"��������");
    else if(sound==1)
      gTextOut2(100,80,"��������");
  }
  
  if(select==1){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    
    
    if(level==0)
      gTextOut2(100,120,"�Ѷȣ���");
    
    else if(level==1)
      gTextOut2(100,120,"�Ѷȣ���");
    
   
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    
    if(level==0)
      gTextOut2(100,120,"�Ѷȣ���");
   
    else if(level==1)
      gTextOut2(100,120,"�Ѷȣ���");
    
  }
}

void Draw_Set_Bg(){
  gClearScreen(LCD_BLUE);
  gTextBkColor(LCD_BLUE);
  gTextColor(LCD_YELLOW);
  gTextOut2(100,40,"��Ϸ����");
  select=0;
  Draw_Set();
}



void Draw_Help_Select(char key){
  if(key=='7'){
       game_state=GAME_MAIN_PAGE;
       Draw_Main_Page_Bg();
  }
}


void Draw_Set_Select(char key){
   if(key=='2'){
    select=(select+1)%2;
    Draw_Set();
  }
  else if(key=='8'){
    select=(select+1)%2;
    Draw_Set();
  }
  else if(key=='5'){
    if(select==0){
      sound=1-sound;
    }
    else if(select==1){
      level=(level+1)%2;
    }
    
    Draw_Set();
  }
  else if(key=='7'){
    game_state=GAME_MAIN_PAGE;  //�������˵�
    Draw_Main_Page_Bg();      //�����˵�����
    
  }
}

void Draw_Start_Back(){
  gClearScreen(LCD_BLUE);
  
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut(0,0,"��Ϸ��ʼ");
    
    gFormatTextOut(80,0,"��ʱ:%d��",timer);
    
  gRect(src_x,src_y,src_x+src_w,src_y+src_h,LCD_RED);         
  
}

void Draw_Box(){

  int i,j;
  
  for(i=0;i<hang;i++)
    for(j=0;j<lie;j++){
      
     if(level==1) 
      {
       if(map[i][j]==0)
        gBMPDraw(src_x+box_w*j,src_y+box_h*i,(const void *)data_smile_bmp);
      
       else
         gBMPDraw(src_x+box_w*j,src_y+box_h*i,(const void *)data_cry_bmp);
       
      
      
       if(i==select_i&&j==select_j){
        gEllipseFill(src_x+box_w*j+1 +box_w/8,src_y+box_h*i+1 +box_h/8,
              src_x+box_w*(j+1)-1 -box_w/8,src_y+box_h*(i+1)-1 -box_h/8,
              LCD_BLACK);
           }
      
       
      }
      else if(level==0)
       {
        if(map[i][j]==0)
          gBMPDraw(src_x+box_w*j,src_y+box_h*i,(const void *)data_smile1_bmp);
      
       else
         gBMPDraw(src_x+box_w*j,src_y+box_h*i,(const void *)data_cry1_bmp);
      
      
      
       if(i==select_i&&j==select_j){
        gEllipseFill(src_x+box_w*j+1 +box_w/8,src_y+box_h*i+1 +box_h/8,
              src_x+box_w*(j+1)-1 -box_w/8,src_y+box_h*(i+1)-1 -box_h/8,
              LCD_BLACK);
           } 
       }
    }
}
//ά�������ά����ͨ�� Draw_Box()��ˢ����Ļ
void Set_Map(int select_i,int select_j){
  if(key=='5')
   {
    map[select_i][select_j]=1-map[select_i][select_j];
    
    if(select_i>0)
      map[select_i-1][select_j]=1-map[select_i-1][select_j];
    
    if(select_j>0)
      map[select_i][select_j-1]=1-map[select_i][select_j-1];
    
    if(select_i<hang-1)
      map[select_i+1][select_j]=1-map[select_i+1][select_j];
    
    if(select_j<lie-1)
      map[select_i][select_j+1]=1-map[select_i][select_j+1];
   }
  else if(key=='1')
  {map[select_i][select_j]=1-map[select_i][select_j];}
}


void init(){
  
  if(level==0){   //�����Ѷ�
    hang=hang_low;
    lie=lie_low;
  }
 
  else if(level==1){
    hang=hang_high;
    lie=lie_high;
  }
    
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    
    timer=0;
    
    step=0;
    
    OSTaskCreate(Task_Clock, (void*)0, Task_Clock_Stack + (STACKSIZE - 1), 5);//��ʱ����
  
  src_x=20;
  src_y=20;
  src_w=280;
  src_h=200;
  
  box_w=src_h/hang;
  box_h=src_w/lie;
  
  
  select_j=0;
  select_i=0;
  
  
  int i,j;
  for(i=0;i<hang;i++)
    for(j=0;j<lie;j++){
      map[i][j]=1;
    }
  
  
  int rand_time=200;     //���ģ���˵Ĳ������ظ�200�Σ��ѷ������
  while(rand_time-->0){
    i=rand()%hang;
    j=rand()%lie;
    
    Set_Map(i,j);     //�ı��ͼ
    
  }
  
  
}


void show_Step(){	//�����������ʾstep
  int i,n=step,a[6];
  for(i=0;i<6;i++){
    a[i]=n%10;
    LEDDisp(i,a[i]);
    n/=10;
    
  }
  
}


void Game_Start_Onpress(char key){
  char key1;
  if(key=='2')
    select_i=(select_i+hang-1)%hang;  //����
  else if(key=='8')
    select_i=(select_i+1)%hang;       //
  else if(key=='4')
    select_j=(select_j+lie-1)%lie;    //
  else if(key=='6')
    select_j=(select_j+1)%lie;        //
  else if(key=='5'){
    Set_Map(select_i,select_j); //�ı��ά�������
    step++;
    if(step<21)                   //���ƴ��ص������
    {
      if(step%10==0&&step%20!=0)//ÿ10����20�����в�ͬ��ʽ������
       {
      Beep(1);
      OSTimeDly(OS_TICKS_PER_SEC*0.5);
      Beep(0);
       } 
      if(step%20==0)
       {
      LEDLight(0,1);
      OSTimeDly(OS_TICKS_PER_SEC*0.05);
      LEDLight(0,0);
      LEDLight(1,1);
     OSTimeDly(OS_TICKS_PER_SEC*0.05);
      LEDLight(1,0);
            LEDLight(2,1);
      OSTimeDly(OS_TICKS_PER_SEC*0.05);
      LEDLight(2,0);
            LEDLight(3,1);
      OSTimeDly(OS_TICKS_PER_SEC*0.05);
      LEDLight(3,0);
            LEDLight(4,1);
      OSTimeDly(OS_TICKS_PER_SEC*0.05);
      LEDLight(4,0);
            LEDLight(5,1);
      OSTimeDly(OS_TICKS_PER_SEC*0.05);
      LEDLight(5,0);
       }
    }
    else
      {
      gClearScreen(LCD_BLUE);
      gTextOut2(100,100,"YOU LOSE");
      OSTimeDly(OS_TICKS_PER_SEC*2);
      
      }
    show_Step();                //�������ʾstep
  }
  else if(key=='1'){              //���װ���
    Set_Map(select_i,select_j);}
  
  if (step<21)
  {
      Draw_Box();     //�ػ淽��
  
     if(key=='7')
     //{ game_state=GAME_MAIN_PAGE;  //�������ʵ�
         // game_pause=1;             //��Ϸվͣ��ʹ���˵��ġ�������Ϸ����Ч
          //Draw_Main_Page_Bg();      //�����˵�����
     //}
      {
        gClearScreen(LCD_BLUE);
        gTextOut2(100,90,"�����밴A");    //�˻����˵�ʱ�Ƿ񱣴�
        gTextOut2(90,120,"�������밴B");
        OSTimeDly(OS_TICKS_PER_SEC*2);       //2S֮������A/B��ѡ��ͨ��GetKey()��ȡ����key1
        key1=GetKey();
        if(key1=='A')
          {
          game_state=GAME_MAIN_PAGE;  //�������ʵ�
          game_pause=1;             //��Ϸվͣ��ʹ���˵��ġ�������Ϸ����Ч
          Draw_Main_Page_Bg();      //�����˵�����
          }
         else if(key1=='B')
         {
          game_state=GAME_MAIN_PAGE;  //�������ʵ�
          game_pause=0;             //��Ϸվͣ��ʹ���˵��ġ�������Ϸ����Ч
          Draw_Main_Page_Bg();      //�����˵�����
         }
      }
  }
  else 
      {
      game_state=GAME_MAIN_PAGE;  
                 
      Draw_Main_Page_Bg();     
      }
}

int check(){	//����Ƿ񴳹سɹ�
  int i,j;
  
  for(i=0;i<hang;i++)
    for(j=0;j<lie;j++){
      if(map[i][j]==1){
        return 0;
      }
    }
  
  return 1;
}

void Task_Clock(void *Id)	//��ʱ����
{
  
  while(game_state==GAME_START||game_pause==1){ //��Ϸ��ʼʱ����
    
    if(game_state==GAME_START){  //��Ϸ��ʼʱ����ص����˵�������ʱ
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gFormatTextOut(80,0,"��ʱ:%d��",timer);
    }
    
    OSTimeDly(OS_TICKS_PER_SEC);
  
    if(game_state==GAME_START){
    timer++;
    }
  
  }
  
  OSTaskDel (OS_PRIO_SELF);     //��Ϸ����ɾ������
  OSTimeDly(100);
  
}


void Draw_Win_Select(){
  if(select==0){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(120,120,"���¿�ʼ");
    
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(120,160,"������Ϸ");
  }
  else if(select==1){
    
    
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(120,160,"������Ϸ");
    
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(120,120,"���¿�ʼ");
    
  }
}

void Draw_Win(){
  gClearScreen(LCD_BLUE);
      int i,n;                            //���ض���
    u32 sx=0,sy=0;//
    u32 oldx=0,oldy=0;
    double f1;
    int mm=0;
    u32 rndx,rndy,r,g,b; 
  for(mm=0;mm<2048;mm++){
        srand(mm);
        rndx=rand()>>23;
        srand(mm*rndx);
        rndy=rand()>>23;
        
        if((mm&0xff)==0x00){
                oldx=sx;
                oldy=sy;
                gClearScreen(LCD_BLACK);
        }
        sx+=rndx;
        sy+=rndy;
        sx=sx%MAX_X;
        sy=sy%MAX_Y;
        gLine(oldx,oldy,sx,sy,rand()&0xffffff);

    }
  gTextOut2(50,40,"���سɹ���");
  gFormatTextOut2(50,80,"��ʱ%d��,��ת%d��",timer,step);
  
  select=0;
  
  Draw_Win_Select();
}



void Draw_Win_Set_Select(char key){
  if(key=='2'||key=='8'){
    select=1-select;
    Draw_Win_Select();
  }
  else if(key=='5'){
    if(select==0){
      
      
      //���¿�ʼ��Ϸ
      game_state=GAME_START;	
      game_pause=0;
      init();
      Draw_Start_Back();
      OSTaskCreate(Task_Clock, (void*)0, Task_Clock_Stack + (STACKSIZE - 1), 5);
      

      Draw_Box();
      
    }
    else if(select==1){
      gClearScreen(LCD_BLACK);
    }
  }
  
  
}



void Draw_Main_Page_Set_Select(char key){
  
  if(key=='2'){
    select=(select+3)%4;
    Draw_Main_Page();
  }
  else if(key=='8'){
    select=(select+1)%4;
    Draw_Main_Page();
  }
  else if(key=='5'){
    if(select==0){
      game_state=GAME_START;
      game_pause=0;     //���ص����ʵ�ʱ������������=1���Ϳ���ѡ��"������Ϸ"
      init();         //��ʼ����Ϸ����
      Draw_Start_Back();  //������
      Draw_Box();     //������ 
    }
    else if(select==1){
      if(game_pause==1){
        game_state=GAME_START;		//ͬ�ϣ���û��init()��ʼ��
        game_pause=0;
        Draw_Start_Back();
        Draw_Box();
        
      }
    }
    else if(select==2){
      game_state=GAME_HELP;
      Draw_Help();
    }
    else if(select==3){
      game_state=GAME_SET;
      Draw_Set_Bg();
    }
  }
  
    
}



void Task_Game_Start(void *Id){          //��Ϸ����
    int i,n;                            //��������
    u32 sx=0,sy=0;//
    u32 oldx=0,oldy=0;
    double f1;
    int mm=0;
    u32 rndx,rndy,r,g,b;  
  for(mm=0;mm<1024;mm++){
        srand(mm);
        rndx=rand()>>23;
        srand(mm*rndx);
        rndy=rand()>>23;
        
        if((mm&0xff)==0x00){
                oldx=sx;
                oldy=sy;
                gClearScreen(LCD_BLACK);
        }
        sx+=rndx;
        sy+=rndy;
        sx=sx%MAX_X;
        sy=sy%MAX_Y;
        gLine(oldx,oldy,sx,sy,rand()&0xffffff);

    }
  Show_Author();
  gTextColor(LCD_RED);
  gTextOut2(50,6,"�� �� �� ͨ �� ѧ");
  OSTimeDly(OS_TICKS_PER_SEC*1.5);
  gClearScreen(LCD_BLUE);
  gTextBkColor(LCD_BLUE);
  gTextColor(LCD_RED);
  gBMPDraw(130,35,(const void *)data_8_bmp);
  gTextOut2(100,120,"��ӭ��ս��");
  gTextOut2(20,200,"All Rights Reserved��");
  //gBMPDraw(0,0,(const void *)data_4_bmp);
  OSTimeDly(OS_TICKS_PER_SEC*5);

    while(1)
    {

      game_state=GAME_MAIN_PAGE;
       game_pause=0; 
       sound=1;	//Ĭ������������
      level=0;	//Ĭ���Ѷ�Ϊ��
      Draw_Main_Page_Bg();
     while(quit==0){            //�����������ܣ�����һ���жϣ�������PB0��Ӧ�жϣ���quit��Ϊ1��
        if(gKeyPressed){        //���ⲿ�ж�EXTI4���趨�İ�����־
        gKeyPressed=0;        
        key=GetKey();         //����I2C���߶�ȡ������
        if(sound==1){     //������
        Beep(1);
        OSTimeDly(OS_TICKS_PER_SEC*0.02);
        Beep(0);
        }
        if(game_state==GAME_MAIN_PAGE){
        Draw_Main_Page_Set_Select(key);
        }
        else if(game_state==GAME_START){
           Game_Start_Onpress(key);
        
        
          if(check()==1){       //��鷽��
          game_state=GAME_WIN;
          Draw_Win();
          
          }
        }
        else if(game_state==GAME_WIN){
          Draw_Win_Set_Select(key);
        }
        else if(game_state==GAME_HELP){
          Draw_Help_Select(key);
        }
        else if(game_state==GAME_SET){
          Draw_Set_Select(key);
        }
       }
    }
    gClearScreen(LCD_BLUE);
    //gTextBkColor(LCD_BLUE);
     LEDDisp(0,0);
     gTextColor(LCD_RED);
     gTextOut2(100,100,"��������");
     OSTimeDly(OS_TICKS_PER_SEC*2);
     gClearScreen(LCD_BLUE);
     quit = 0;
    } 
}




//TaskStart:  ��ȡ��Ϣ���У����չ̶����ʷ��Ͱ����ַ�
void TaskStart(void *arg)
{
    u8 err;
    char rxmsg;

    /* ϵͳʱ��/���ߣ�GPIO�������� LED��LCD�ĳ�ʼ�� */
    System_Init();    

    //�趨systick��������100HZ��ʱ�ж�
    Tmr_TickInit ();  //setup systick
    
    PBInit();

    //������Ϣ����
    QSem = OSQCreate(&QMsgTbl[0], 50);		
    //��ն���
    OSQFlush(QSem);
    
    //srand((int)time(NULL));

    OSTaskCreate(Task_Game_Start, (void*)0, Task_Game_Start_Stack + (STACKSIZE - 1), 6);
    
    
    //ɾ���Լ�
    OSTaskDel (OS_PRIO_SELF);

    while(1)
    {
      OSTimeDly(100);
    }
    
    
}



/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
  //�ر�LED�������ʾ��������bsp.c��
  //LEDLights(0x3f,0);	

  srand((int)time(NULL));
  // uCOS��ʼ��
  OSInit();	
  //������һ��Ӧ������
  OSTaskCreate(TaskStart, (void*)0, StackStart + (STKSIZE - 1), 4);	//�������ȼ�=4

  //����uCOS����
  OSStart();  
  
  while (1);  
}



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
