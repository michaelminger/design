#include  "systemInit.h"
#include  <uart.h>
#include  "uartGetPut.h"
#include  <timer.h>
#include  "lcd_driver.H"
#include  "KEY.H"
#include  "SoftI2C.h"
#include  <hw_types.h>
#include  <hw_memmap.h>
#include  <sysctl.h>
#include  <gpio.h>
#include  <stdio.h>
#include  "LM3S1138_PinMap.H"

// 将较长的标识符定义为较短的形式
#define  SysCtlPeriEnable       SysCtlPeripheralEnable
#define  SysCtlPeriDisable      SysCtlPeripheralDisable
#define  GPIOPinTypeIn          GPIOPinTypeGPIOInput
#define  GPIOPinTypeOut         GPIOPinTypeGPIOOutput
#define  GPIOPinTypeOD          GPIOPinTypeGPIOOutputOD

#define  KEY_PERIPH             SYSCTL_PERIPH_GPIOG
#define  KEY_PORT               GPIO_PORTG_BASE
#define  KEY_PIN                GPIO_PIN_5

#define  Infrared_S_E           SYSCTL_PERIPH_GPIOE
#define  Infrared_S_G           SYSCTL_PERIPH_GPIOG
#define  Infrared_PORT_E        GPIO_PORTE_BASE
#define  Infrared_PORT_G        GPIO_PORTG_BASE

#define  E0                     GPIOPinRead(Infrared_PORT_E, GPIO_PIN_0)
#define  E1                     GPIOPinRead(Infrared_PORT_E, GPIO_PIN_1)
#define  E2                     GPIOPinRead(Infrared_PORT_E, GPIO_PIN_2)
#define  E3                     GPIOPinRead(Infrared_PORT_E, GPIO_PIN_3)
#define  G2                     GPIOPinRead(Infrared_PORT_G, GPIO_PIN_2)
#define  G3                     GPIOPinRead(Infrared_PORT_G, GPIO_PIN_3)


////////////////////////////////////////////////////////////////
// 定义PID参数结构体
///////////////////////////////////////////////////////////////
//位置式PID
typedef struct PID { 
   double SetPoint;   // 设定目标 Desired Value 
   double Proportion; // 比例常数 Proportional Const 
   double Integral;   // 积分常数 Integral Const 
   double Derivative; // 微分常数 Derivative Const 
   double LastError;  // Error[-1]
   double PrevError;  //Error[-2]
   double SumError;   // Sums of Errors
} PID;

static PID vPID_X;                        //定义结构变量名
static PID *fptr_X = &vPID_X ;

static PID vPID_A;                        //定义结构变量名
static PID *fptr_A = &vPID_A ;

// 定义PID参数结构体
///////////////////////////////////////////////////////////////
int Pdata_X  = 80 , Idata_X = 0 , Ddata_X = 0 ;
int Pdata_A  = 10 , Idata_A = 0 , Ddata_A = 0 ;

void IncPIDInit(void)
{
  fptr_X->LastError = 0.0;      //Error[-1]
  fptr_X->PrevError = 0.0;      //Error[-2]
  fptr_X->Proportion = Pdata_X; //比例常数 Proportional Const
  fptr_X->Integral =   Idata_X; //积分常数Integral Const
  fptr_X->Derivative = Ddata_X; //微分常数 Derivative Const
  fptr_X->SetPoint =0.0;        //目标是30
  
  fptr_A->LastError = 0.0;      //Error[-1]
  fptr_A->PrevError = 0.0;      //Error[-2]
  fptr_A->Proportion = Pdata_A; //比例常数 Proportional Const
  fptr_A->Integral =   Idata_A; //积分常数Integral Const
  fptr_A->Derivative = Ddata_A; //微分常数 Derivative Const
  fptr_A->SetPoint =0.0;        //目标是30
}


//  防止JTAG失效
void  JTAG_Wait(void){
  SysCtlPeriEnable(KEY_PERIPH);                               //  使能KEY所在的GPIO端口
  GPIOPinTypeIn(KEY_PORT , KEY_PIN);                          //  设置KEY所在管脚为输入
  if ( GPIOPinRead(KEY_PORT , KEY_PIN)  ==  0x00 ){           //  如果复位时按下KEY，则进入
    while(1);                                               //  死循环，以等待JTAG连接
  }
  SysCtlPeriDisable(KEY_PERIPH);                              //  禁止KEY所在的GPIO端口
}


//  定义全局的系统时钟变量
unsigned long  TheSysClock  =  12000000UL;
char cBuf[30] , dBuf[30] , eBuf[30] ;
int Ti1 , Ti2 , Ti3 , Ti4 , Ti5 ;
int N ; 
unsigned char Re_buf[11],counter=0;
unsigned char ucStra[6],ucStrw[6],ucStrAngle[6];
float Value[9];
int T1 , T2 ,T3;
int T_F , T_B ; 
float Error_Y , Error_A ;
double  Error , Error_a , Error_x ;
int Time ;

void Infrar_Init(void);
void Moto_Init(void);
double PIDCalc( PID *pp, double NextPoint );
float PIDCal( PID *pp, float ThisError );
void Fun_1 ( void );
void Fun_2 ( void );

void Front( int speed );
void Back(  int speed  );
void Stay(  void  ) ;
void Right_F( int speed  ) ;
void Left_F(  int speed  ) ;
void Right_B( int speed  ) ;
void Left_B(  int speed  ) ;
void Delay(unsigned long x);

//  系统初始化
void  SystemInit(void){
  SysCtlClockSet(SYSCTL_USE_PLL |                             //  系统时钟设置，采用PLL
                 SYSCTL_OSC_MAIN |                            //  主振荡器
                   SYSCTL_XTAL_6MHZ |                           //  外接6MHz晶振
                     SYSCTL_SYSDIV_4);                            //  分频结果为20MHz
  
  TheSysClock  =  SysCtlClockGet();                           //  获取系统时钟，单位：Hz
  
  KEY_Init(KEY1 | KEY2);                                      //  KEY初始化
}


void Time_Init( void ) {
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER0);                 //  使能Timer模块
  TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR |     //  配置Timer为16位周期定时器
                 TIMER_CFG_A_PERIODIC);
  TimerPrescaleSet(TIMER0_BASE, TIMER_A, 99);             //  预先进行100分频
  TimerLoadSet(TIMER0_BASE, TIMER_A, 30000);              //  设置Timer初值，定时500ms
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);        //  使能Timer超时中断
  IntEnable(INT_TIMER0A);                                 //  使能Timer中断
  IntMasterEnable();                                      //  使能处理器中断
  TimerEnable(TIMER0_BASE, TIMER_A);                      //  使能Timer计数
}



int  main(void){
  JTAG_Wait();                                                //  防止JTAG失效，重要！
  SystemInit();    
  init_lcd();
  I2C_Init();   
  Time_Init();
  uartInit();                                             //  UART初始化
  
  IncPIDInit();
  Infrar_Init();
  Moto_Init();
  
  while(1){ 
    sprintf(dBuf, "当前为程序%d      ", Ti3 + 1 );     //  采样值格式化为电压值
    sprintf(cBuf, "前进: %d s         ", T_F);     //  采样值格式化为电压值
    sprintf(eBuf, "后退: %d s         ", T_B);     //  采样值格式化为电压值
    disp_line(0,dBuf);
    disp_line(1,cBuf);
    
    disp_line(2,eBuf);
    
    if ( KEY_Get(KEY1 | KEY2) ){
      while( KEY_Get(KEY1 | KEY2) != 0x00 );
      Ti1 = 0 ;
      Ti2 = 0 ;
      T_F = 0 ; 
      T_B = 0 ;
      T1 = 0 ;
      Ti3++ ; 
    }
    if ( Ti3 >= 4 ){
      Ti3 = 3 ;
    }
    
    switch ( Ti3 ){
    case 0 :
      Fun_1 ();
      break ;
    case 1 :
      Fun_1 ();
      break ;
    case 2 :
      Fun_2 ();
      break ;
    case 3 :
      Fun_2 ();
      break ;
    default :
      break ;
    }
    
  }
}

//////////////////////////////////////////////////////
/// 基本功能
/////////////：前进+后退 +计时
void Fun_1 ( void ){                                             
  if( T1 == 0  ){
    if ( ( E1 != 0 ) && ( E2 != 0 )  && ( E3 != 0  ) ){
      Front( 800 );
    }else if ( ( E1 == 0 ) && ( E2 != 0 ) ){
      Right_F( 800 );
    }else if ( ( E1 != 0 ) && ( E2 == 0 ) ){
      Left_F( 800 );
    }else if ( ( E1 == 0 ) && ( E2  == 0 ) ){
      Back(600); 
    } 
    if ( ( E1 != 0 ) && ( E2 != 0 ) && ( E0 != 0 )  && ( E3 == 0  )){  
      Stay() ;
      T1 = 1 ;
      SysCtlDelay(5000 * (TheSysClock / 3000));           //  延时约5000ms
    }
  }
  else if ( T1 == 1 ){
    if ( ( G2 != 0 ) && ( G3 != 0 )  && ( E0 != 0  ) ){
      Back(800); 
    }else if ( ( G2 == 0 ) && ( G3 != 0 ) ){
      Left_B( 800 );
    }else if ( ( G2 != 0 ) && ( G3 == 0 ) ){
      Right_B( 800 );
    }else if ( ( G2 == 0 ) && ( G3  == 0 ) ){
      Front(600);
    } 
    if ( ( G2 != 0 ) && ( G3 != 0 ) && ( E0 == 0 )  && ( E3 != 0  )){  
      Stay() ;
      T1 = 2 ;
      SysCtlDelay(5000 * (TheSysClock / 3000));           //  延时约5000ms
    }
    
  }
  
}

void Fun_2 ( void ){
  
    Pdata_X = 80 ;
    Idata_X = 2 ; 
    Ddata_X = 5 ;
    
    Pdata_A = 15 ;
    Idata_A = 0 ; 
    Ddata_A = 2 ;
    char c[40];
    Delay(10);
    Error_x = PIDCalc( fptr_X , Error_Y );
    Error_a = PIDCal( fptr_A , Error_A );
    if ( Error_x < 0 ){
      if ( Error_a > 0  ){
        Error_a = -Error_a ;
      }
    }else if ( Error_x > 0 ){
      if ( Error_a < 0  ){
        Error_a = -Error_a ;
      }
    }
    Error =  (Error_x + 79 )*1.2  - Error_a * 1 ; 
    if ( Error >= 900){
        Error = 900;
    }
    if ( Error <= -900){
        Error = -900;
    }
    if ( Error < - 10  ){
      Back( -Error );
    }else if ( Error >  10  ){
      Front( Error );
    }else {
      Stay();
    }
  
  
}



//  定时器的中断服务函数
void Timer0A_ISR(void)
{
  unsigned long ulStatus;
  
  ulStatus = TimerIntStatus(TIMER0_BASE, true);           //  读取中断状态
  TimerIntClear(TIMER0_BASE, ulStatus);                   //  清除中断状态，重要！
  if (ulStatus & TIMER_TIMA_TIMEOUT)                      //  如果是Timer超时中断
  {
    Ti1 ++ ;
    if ( Ti1 == 17 ){
      Ti2 ++ ;
      Ti1 = 0 ;
    }
    if ( T1 == 0 ){
      T_F = Ti2 ;
    }
    if ( T1 == 1  ){
      if( T_B == 0  ){
        N = T_F + 5  ;
      }
      T_B = Ti2 - N ;
    }
 
  }
}


//  UART0中断服务函数
void UART0_ISR(void)
{
  
  unsigned long ulStatus;
  
  ulStatus = UARTIntStatus(UART0_BASE, true);             //  读取当前中断状态
  UARTIntClear(UART0_BASE, ulStatus);                     //  清除中断状态
  
  if ((ulStatus & UART_INT_RX) || (ulStatus & UART_INT_RT))   //  若是接收中断或者
  {  
    while(1){
      Re_buf[counter]= uartGetc0();
      if(counter==0&&Re_buf[0]!=0x55) return;      //第0号数据不是帧头
      
      counter++; 
      if(counter==11)             //接收到11个数据
      {    
        counter=0;               //重新赋值，准备下一帧数据的接收        
        switch(Re_buf [1])
        {
        case 0x52:	 
          ucStrw[2]=Re_buf[4];
          ucStrw[3]=Re_buf[5];
          Error_A = ((short)(ucStrw[3]<<8| ucStrw[2]))/32768.0*2000;
          break;
        case 0x53: 
          ucStrAngle[2]=Re_buf[4];
          ucStrAngle[3]=Re_buf[5];
          Error_Y = ((short)(ucStrAngle[3]<<8| ucStrAngle[2]))/32768.0*180 - 2 ;
          break;
        }
      }
      break;
    }
  }
}



void Infrar_Init(void){
  SysCtlPeriEnable( Infrared_S_E );                                  //  使能LED1所在的GPIO端口
  SysCtlPeriEnable( Infrared_S_G );                                  //  使能LED1所在的GPIO端口
  
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_0 );                    //  设置LED1所在的管脚为输出
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_1 );            //置LED1所在的管脚为输出
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_2 );                    //  设置LED1所在的管脚为输出
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_3 );                    //  设置LED1所在的管脚为输出
  GPIOPinTypeIn( Infrared_PORT_G , GPIO_PIN_2 );                    //  设置LED1所在的管脚为输出
  GPIOPinTypeIn( Infrared_PORT_G , GPIO_PIN_3 );                    //  设置LED1所在的管脚为输出
}


void Moto_Init(void){ //初始化电机
  
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER1);                 //  使能TIMER1模块
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER2);                 //  使能TIMER2模块
  
  SysCtlPeriEnable(CCP2_PERIPH);                          //  使能CCP2所在的GPIO端口
  SysCtlPeriEnable(CCP3_PERIPH);                          //  使能CCP3所在的GPIO端口
  SysCtlPeriEnable(CCP4_PERIPH);                          //  使能CCP4所在的GPIO端口
  SysCtlPeriEnable(CCP5_PERIPH);                          //  使能CCP5所在的GPIO端口
  
  GPIOPinTypeTimer(CCP2_PORT, CCP2_PIN);                  //  设置相关管脚为Timer功能
  GPIOPinTypeTimer(CCP3_PORT, CCP3_PIN);                  //  设置相关管脚为Timer功能
  GPIOPinTypeTimer(CCP4_PORT, CCP4_PIN);                  //  设置相关管脚为Timer功能
  GPIOPinTypeTimer(CCP5_PORT, CCP5_PIN);                  //  设置相关管脚为Timer功能
  
  TimerConfigure(TIMER1_BASE, TIMER_CFG_16_BIT_PAIR |     //  配置Timer为双16位PWM
                 TIMER_CFG_A_PWM |
                   TIMER_CFG_B_PWM);
  TimerConfigure(TIMER2_BASE, TIMER_CFG_16_BIT_PAIR |     //  配置Timer为双16位PWM
                 TIMER_CFG_A_PWM |
                   TIMER_CFG_B_PWM);
  
  
  TimerControlLevel(TIMER1_BASE, TIMER_BOTH, true);      //  控制PWM输出反相
  TimerControlLevel(TIMER2_BASE, TIMER_BOTH, true);
  TimerLoadSet(TIMER1_BASE, TIMER_BOTH, 1000);           //  设置TimerBoth初值
  TimerLoadSet(TIMER2_BASE, TIMER_BOTH, 1000);
  
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值
  
  TimerEnable(TIMER1_BASE, TIMER_BOTH);                  //  使能Timer计数，PWM开始输出
  TimerEnable(TIMER2_BASE, TIMER_BOTH);
}


void Front( int speed ){
  TimerMatchSet(TIMER1_BASE, TIMER_A, speed);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, speed);     //  设置TimerB匹配值;  
}

void Back(  int speed  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, speed);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, speed);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值;  
}

void Stay(  void  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值;  
}

void Right_F( int speed  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, speed);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值;  
}

void Left_F(  int speed  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, speed);     //  设置TimerB匹配值;  
}


void Right_B( int speed  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, speed);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值;  
}

void Left_B(  int speed  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, speed);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值;  
}

void  Delay(unsigned long x){
  unsigned long DelayValue = 0;
  for (DelayValue = 0; DelayValue < x; DelayValue++);
}  


/*================================ PID 计算部分 ===============================*/
double PIDCalc( PID *pp, double NextPoint )
{ 
   double dError, Error; 
   Error = pp->SetPoint - NextPoint; // 偏差 
   pp->SumError += Error; // 积分 
   dError = pp->LastError - pp->PrevError; // 当前微分 
   pp->PrevError = pp->LastError; 
   pp->LastError = Error; 
   return (pp->Proportion * Error // 比例项 
           + pp->Integral * pp->SumError // 积分项
             + pp->Derivative * dError  // 微分项
               );
} 


//Title:增量式PID算法程序
//Description:给出一个误差增量
//Input: PID的P、I控制常数和之前的误差量（PID *pp）& 当前误差量（ThisError）
//Return: 误差增量templ
//////////////////////////////////////////////////////////////////////
float PIDCal( PID *pp, float ThisError )
{
 //增量式PID算法（需要控制的不是控制量的绝对值，而是控制量的增量）
    float  pError,dError,iError;
    float  templ; 
    fptr_A->Proportion = Pdata_A; //比例常数 Proportional Const
    fptr_A->Integral   = Idata_A; //积分常数Integral Const
    fptr_A->Derivative = Ddata_A; //微分常数 Derivative Const
    pError = ThisError- fptr_A->LastError; 
    iError = ThisError;
    dError = ThisError-2*( fptr_A->LastError)+ fptr_A->PrevError;
    
    //增量计算 
    templ= fptr_A->Proportion*pError +  fptr_A->Integral*iError+ fptr_A->Derivative*dError;  //增量

    //存储误差用于下次运算
     fptr_A->PrevError  =  fptr_A->LastError;
     fptr_A->LastError = ThisError;
     
     return (templ);
}
 