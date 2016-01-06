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
//处理之后的二进制格式图片
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

  // RCC：打开按键输入端口BPIOB的时钟以及 AFIO模块时钟 !!  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO, ENABLE);

  // GPIO : 将引脚 PB0配置为数字浮空输入 input floating 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;//
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 
  // GPIO : 设置PB0作为外中断线（即从PA0,PB0,...PG0中选择一个作为EXTI0） 
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
  
  
  // EXTI : 打开外中断EXTI0的中断
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;            //选择EXTI0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   //作为中断而不是事件来源
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;             // 允许中断
  EXTI_Init(&EXTI_InitStructure);

  // NVIC:  配置EXTI0在IRQ通道的参数
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;            //选择EXTI0通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0; //分组优先级 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;        //组内优先级 0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //打开这个中断通道

  NVIC_Init(&NVIC_InitStructure); 
 
}


///******************任务定义***************///
OS_STK StackStart[STKSIZE];
void TaskStart(void *arg);

OS_STK Task_Clock_Stack[STACKSIZE];
void Task_Clock(void *Id);

OS_STK Task_Game_Start_Stack[STACKSIZE]; 		//task1 stack
void Task_Game_Start(void *Id);

//OS_STK Task_Game_Win_Stack[STACKSIZE]; 		//task2 stack

OS_EVENT *QSem;                //消息队列指针
void     *QMsgTbl[50];         //消息指针数组

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
    gTextOut2(90,50,"通信1110班");
     gTextOut2(40,100,"曹伟");
    gTextOut2(160,100,"11274004");
    gTextOut2(40,150,"温士贤");
    gTextOut2(160,150,"11253049");
 

}

void Draw_Main_Page_Bg(){
  
    gClearScreen(LCD_BLUE);
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_RED);
    gTextOut2(70,40,"智  力  方  块");
    select=0;
    Draw_Main_Page();
}

void Draw_Main_Page(){
  
  if(select==0){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(100,80,"开始游戏");
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(100,80,"开始游戏");
  }
  
  if(select==1){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(100,120,"继续游戏");
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(100,120,"继续游戏");
  }
  
  if(select==2){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(100,160,"游戏帮助");
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(100,160,"游戏帮助");
  }
  
  if(select==3){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(100,200,"游戏设置");
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(100,200,"游戏设置");
  }
  
   
}

void Draw_Help(){
  gClearScreen(LCD_BLUE);
  gTextBkColor(LCD_BLUE);
  gTextColor(LCD_YELLOW);
  gTextOut(30,60,"把所有方块变成笑脸。");
  gTextOut(30,120,"操作说明：");
  gTextOut(30,140,"2 8 4 6：上下左右");
  gTextOut(30,160,"5：确定");
  gTextOut(30,180,"7：返回");
}

void Draw_Set(){
  if(select==0){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    if(sound==0)
      gTextOut2(100,80,"声音：关");
    else if(sound==1)
      gTextOut2(100,80,"声音：开");
   
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    if(sound==0)
      gTextOut2(100,80,"声音：关");
    else if(sound==1)
      gTextOut2(100,80,"声音：开");
  }
  
  if(select==1){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    
    
    if(level==0)
      gTextOut2(100,120,"难度：低");
    
    else if(level==1)
      gTextOut2(100,120,"难度：高");
    
   
  }
  else{
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    
    if(level==0)
      gTextOut2(100,120,"难度：低");
   
    else if(level==1)
      gTextOut2(100,120,"难度：高");
    
  }
}

void Draw_Set_Bg(){
  gClearScreen(LCD_BLUE);
  gTextBkColor(LCD_BLUE);
  gTextColor(LCD_YELLOW);
  gTextOut2(100,40,"游戏设置");
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
    game_state=GAME_MAIN_PAGE;  //返回主菜单
    Draw_Main_Page_Bg();      //画主菜单背景
    
  }
}

void Draw_Start_Back(){
  gClearScreen(LCD_BLUE);
  
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut(0,0,"游戏开始");
    
    gFormatTextOut(80,0,"用时:%d秒",timer);
    
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
//维护这个二维数组通过 Draw_Box()来刷新屏幕
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
  
  if(level==0){   //设置难度
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
    
    OSTaskCreate(Task_Clock, (void*)0, Task_Clock_Stack + (STACKSIZE - 1), 5);//计时任务
  
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
  
  
  int rand_time=200;     //随机模拟人的操作，重复200次，把方块打乱
  while(rand_time-->0){
    i=rand()%hang;
    j=rand()%lie;
    
    Set_Map(i,j);     //改变地图
    
  }
  
  
}


void show_Step(){	//在数码管上显示step
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
    select_i=(select_i+hang-1)%hang;  //上移
  else if(key=='8')
    select_i=(select_i+1)%hang;       //
  else if(key=='4')
    select_j=(select_j+lie-1)%lie;    //
  else if(key=='6')
    select_j=(select_j+1)%lie;        //
  else if(key=='5'){
    Set_Map(select_i,select_j); //改变二维数组参数
    step++;
    if(step<21)                   //控制闯关的最大步数
    {
      if(step%10==0&&step%20!=0)//每10步，20步进行不同方式的提醒
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
    show_Step();                //数码管显示step
  }
  else if(key=='1'){              //作弊按键
    Set_Map(select_i,select_j);}
  
  if (step<21)
  {
      Draw_Box();     //重绘方块
  
     if(key=='7')
     //{ game_state=GAME_MAIN_PAGE;  //返回主彩单
         // game_pause=1;             //游戏站停，使主菜单的“继续游戏”有效
          //Draw_Main_Page_Bg();      //画主菜单背景
     //}
      {
        gClearScreen(LCD_BLUE);
        gTextOut2(100,90,"保存请按A");    //退回主菜单时是否保存
        gTextOut2(90,120,"不保存请按B");
        OSTimeDly(OS_TICKS_PER_SEC*2);       //2S之内作出A/B的选择通过GetKey()读取付给key1
        key1=GetKey();
        if(key1=='A')
          {
          game_state=GAME_MAIN_PAGE;  //返回主彩单
          game_pause=1;             //游戏站停，使主菜单的“继续游戏”有效
          Draw_Main_Page_Bg();      //画主菜单背景
          }
         else if(key1=='B')
         {
          game_state=GAME_MAIN_PAGE;  //返回主彩单
          game_pause=0;             //游戏站停，使主菜单的“继续游戏”有效
          Draw_Main_Page_Bg();      //画主菜单背景
         }
      }
  }
  else 
      {
      game_state=GAME_MAIN_PAGE;  
                 
      Draw_Main_Page_Bg();     
      }
}

int check(){	//检查是否闯关成功
  int i,j;
  
  for(i=0;i<hang;i++)
    for(j=0;j<lie;j++){
      if(map[i][j]==1){
        return 0;
      }
    }
  
  return 1;
}

void Task_Clock(void *Id)	//计时任务
{
  
  while(game_state==GAME_START||game_pause==1){ //游戏开始时启用
    
    if(game_state==GAME_START){  //游戏开始时如果回到主菜单，不计时
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gFormatTextOut(80,0,"用时:%d秒",timer);
    }
    
    OSTimeDly(OS_TICKS_PER_SEC);
  
    if(game_state==GAME_START){
    timer++;
    }
  
  }
  
  OSTaskDel (OS_PRIO_SELF);     //游戏结束删除任务
  OSTimeDly(100);
  
}


void Draw_Win_Select(){
  if(select==0){
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(120,120,"重新开始");
    
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(120,160,"结束游戏");
  }
  else if(select==1){
    
    
    gTextBkColor(LCD_YELLOW);
    gTextColor(LCD_BLUE);
    gTextOut2(120,160,"结束游戏");
    
    gTextBkColor(LCD_BLUE);
    gTextColor(LCD_YELLOW);
    gTextOut2(120,120,"重新开始");
    
  }
}

void Draw_Win(){
  gClearScreen(LCD_BLUE);
      int i,n;                            //过关动画
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
  gTextOut2(50,40,"闯关成功！");
  gFormatTextOut2(50,80,"用时%d秒,翻转%d次",timer,step);
  
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
      
      
      //重新开始游戏
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
      game_pause=0;     //当回到主彩单时，如果这个变量=1，就可以选择"继续游戏"
      init();         //初始化游戏数据
      Draw_Start_Back();  //画背景
      Draw_Box();     //画表情 
    }
    else if(select==1){
      if(game_pause==1){
        game_state=GAME_START;		//同上，但没有init()初始化
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



void Task_Game_Start(void *Id){          //游戏任务
    int i,n;                            //开机动画
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
  gTextOut2(50,6,"北 京 交 通 大 学");
  OSTimeDly(OS_TICKS_PER_SEC*1.5);
  gClearScreen(LCD_BLUE);
  gTextBkColor(LCD_BLUE);
  gTextColor(LCD_RED);
  gBMPDraw(130,35,(const void *)data_8_bmp);
  gTextOut2(100,120,"欢迎挑战！");
  gTextOut2(20,200,"All Rights Reserved！");
  //gBMPDraw(0,0,(const void *)data_4_bmp);
  OSTimeDly(OS_TICKS_PER_SEC*5);

    while(1)
    {

      game_state=GAME_MAIN_PAGE;
       game_pause=0; 
       sound=1;	//默认设置声音开
      level=0;	//默认难度为低
      Draw_Main_Page_Bg();
     while(quit==0){            //重新启动功能，设置一个中断，当按下PB0响应中断，把quit变为1，
        if(gKeyPressed){        //在外部中断EXTI4中设定的按键标志
        gKeyPressed=0;        
        key=GetKey();         //经过I2C总线读取按键码
        if(sound==1){     //按键响
        Beep(1);
        OSTimeDly(OS_TICKS_PER_SEC*0.02);
        Beep(0);
        }
        if(game_state==GAME_MAIN_PAGE){
        Draw_Main_Page_Set_Select(key);
        }
        else if(game_state==GAME_START){
           Game_Start_Onpress(key);
        
        
          if(check()==1){       //检查方块
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
     gTextOut2(100,100,"重新启动");
     OSTimeDly(OS_TICKS_PER_SEC*2);
     gClearScreen(LCD_BLUE);
     quit = 0;
    } 
}




//TaskStart:  读取消息队列，按照固定速率发送按键字符
void TaskStart(void *arg)
{
    u8 err;
    char rxmsg;

    /* 系统时钟/总线，GPIO，按键， LED，LCD的初始化 */
    System_Init();    

    //设定systick用来产生100HZ定时中断
    Tmr_TickInit ();  //setup systick
    
    PBInit();

    //创建消息队列
    QSem = OSQCreate(&QMsgTbl[0], 50);		
    //清空队列
    OSQFlush(QSem);
    
    //srand((int)time(NULL));

    OSTaskCreate(Task_Game_Start, (void*)0, Task_Game_Start_Stack + (STACKSIZE - 1), 6);
    
    
    //删除自己
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
  //关闭LED发光管显示，函数在bsp.c中
  //LEDLights(0x3f,0);	

  srand((int)time(NULL));
  // uCOS初始化
  OSInit();	
  //建立第一个应用任务
  OSTaskCreate(TaskStart, (void*)0, StackStart + (STKSIZE - 1), 4);	//任务优先级=4

  //启动uCOS调度
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
