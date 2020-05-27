#include  "systemInit.h"
#include  <uart.h>
#include  <ctype.h>
#include  <string.h>
#include  "uartGetPut.h"
#include  <stdio.h>
#include  <timer.h>
#define  PART_LM3S1138
#include  <pin_map.h>
#include <math.h>
#include  "buzzer.h"

#define  Infrared_S_E        SYSCTL_PERIPH_GPIOE
#define  Infrared_S_G        SYSCTL_PERIPH_GPIOG
#define  Infrared_PORT_E     GPIO_PORTE_BASE
#define  Infrared_PORT_G     GPIO_PORTG_BASE

#define  E0               GPIOPinRead(Infrared_PORT_E, GPIO_PIN_0)
#define  E1               GPIOPinRead(Infrared_PORT_E, GPIO_PIN_1)
#define  E2               GPIOPinRead(Infrared_PORT_E, GPIO_PIN_2)
#define  E3               GPIOPinRead(Infrared_PORT_E, GPIO_PIN_3)
#define  G2               GPIOPinRead(Infrared_PORT_G, GPIO_PIN_2)

int T1 , T2 , T3 ; 

void Infrar_Init(void){
  SysCtlPeriEnable( Infrared_S_E );                                  //  使能LED1所在的GPIO端口
  SysCtlPeriEnable( Infrared_S_G );                                  //  使能LED1所在的GPIO端口
  
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_0 );                    //  设置LED1所在的管脚为输出
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_1 );            //置LED1所在的管脚为输出
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_2 );                    //  设置LED1所在的管脚为输出
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_3 );                    //  设置LED1所在的管脚为输出
  GPIOPinTypeIn( Infrared_PORT_G , GPIO_PIN_2 );                    //  设置LED1所在的管脚为输出
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


void Time_Init(void){
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER0);                 //  使能Timer模块 
  TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR |     //  配置Timer为16位周期定时器
                 TIMER_CFG_A_PERIODIC); 
  TimerPrescaleSet(TIMER0_BASE, TIMER_A, 99);             //  预先进行100分频
  TimerLoadSet(TIMER0_BASE, TIMER_A, 3000);              //  设置Timer初值，定时500ms
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT );        //  使能Timer超时中断
  IntEnable(INT_TIMER0A);                                 //  使能Timer中断
  IntMasterEnable();                                      //  使能处理器中断
  TimerEnable(TIMER0_BASE, TIMER_A);                      //  使能Timer计数
}

void Front( void ){
  TimerMatchSet(TIMER1_BASE, TIMER_A, 600);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 600);     //  设置TimerB匹配值;  
}

void Back( void ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 600);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 600);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值;  
}

void Stay( void ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值;  
}

void Right( void ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 800);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值;  
}

void Left( void ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 800);     //  设置TimerB匹配值;  
}
//  主函数（程序入口）
int main(void)
{
  jtagWait();                                             //  防止JTAG失效，重要！
  clockInit();                                            //  时钟初始化：晶振，6MHz
  
  //  uartInit();
  Moto_Init();
  /////////////////////////////////////////////////////////////////////////////
  Time_Init();
  Infrar_Init();
  
  while(1)
  {
    if ( ( E1 != 0 ) && ( E2 != 0 ) && ( G2 != 0 ) ){
      Back(); 
    }else if ( ( E1 == 0 ) && ( E2 != 0 ) ){
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 800);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值;  
    }else if ( ( E1 != 0 ) && ( E2 == 0 ) ){
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER1_BASE, TIMER_B, 800);     //  设置TimerB匹配值;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  设置TimerB匹配值
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  设置TimerB匹配值;  
    }else if ( ( E1 == 0 ) && ( E2 == 0 ) ){
      Front();
    } 
    if ( ( E1 != 0 ) && ( E2 != 0 ) && ( G2 != 0 )  && ( E0 == 0  )){  
      Stay() ;
      SysCtlDelay(5000 * (TheSysClock / 3000));           //  延时约5000ms
    }
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
    
  }
}