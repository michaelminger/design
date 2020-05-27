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
  SysCtlPeriEnable( Infrared_S_E );                                  //  ʹ��LED1���ڵ�GPIO�˿�
  SysCtlPeriEnable( Infrared_S_G );                                  //  ʹ��LED1���ڵ�GPIO�˿�
  
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_0 );                    //  ����LED1���ڵĹܽ�Ϊ���
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_1 );            //��LED1���ڵĹܽ�Ϊ���
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_2 );                    //  ����LED1���ڵĹܽ�Ϊ���
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_3 );                    //  ����LED1���ڵĹܽ�Ϊ���
  GPIOPinTypeIn( Infrared_PORT_G , GPIO_PIN_2 );                    //  ����LED1���ڵĹܽ�Ϊ���
}


void Moto_Init(void){ //��ʼ�����
  
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER1);                 //  ʹ��TIMER1ģ��
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER2);                 //  ʹ��TIMER2ģ��
  
  SysCtlPeriEnable(CCP2_PERIPH);                          //  ʹ��CCP2���ڵ�GPIO�˿�
  SysCtlPeriEnable(CCP3_PERIPH);                          //  ʹ��CCP3���ڵ�GPIO�˿�
  SysCtlPeriEnable(CCP4_PERIPH);                          //  ʹ��CCP4���ڵ�GPIO�˿�
  SysCtlPeriEnable(CCP5_PERIPH);                          //  ʹ��CCP5���ڵ�GPIO�˿�
  
  GPIOPinTypeTimer(CCP2_PORT, CCP2_PIN);                  //  ������عܽ�ΪTimer����
  GPIOPinTypeTimer(CCP3_PORT, CCP3_PIN);                  //  ������عܽ�ΪTimer����
  GPIOPinTypeTimer(CCP4_PORT, CCP4_PIN);                  //  ������عܽ�ΪTimer����
  GPIOPinTypeTimer(CCP5_PORT, CCP5_PIN);                  //  ������عܽ�ΪTimer����
  
  TimerConfigure(TIMER1_BASE, TIMER_CFG_16_BIT_PAIR |     //  ����TimerΪ˫16λPWM
                 TIMER_CFG_A_PWM |
                   TIMER_CFG_B_PWM);
  TimerConfigure(TIMER2_BASE, TIMER_CFG_16_BIT_PAIR |     //  ����TimerΪ˫16λPWM
                 TIMER_CFG_A_PWM |
                   TIMER_CFG_B_PWM);
  
  
  TimerControlLevel(TIMER1_BASE, TIMER_BOTH, true);      //  ����PWM�������
  TimerControlLevel(TIMER2_BASE, TIMER_BOTH, true);
  TimerLoadSet(TIMER1_BASE, TIMER_BOTH, 1000);           //  ����TimerBoth��ֵ
  TimerLoadSet(TIMER2_BASE, TIMER_BOTH, 1000);
  
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ
  
  TimerEnable(TIMER1_BASE, TIMER_BOTH);                  //  ʹ��Timer������PWM��ʼ���
  TimerEnable(TIMER2_BASE, TIMER_BOTH);
}


void Time_Init(void){
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER0);                 //  ʹ��Timerģ�� 
  TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR |     //  ����TimerΪ16λ���ڶ�ʱ��
                 TIMER_CFG_A_PERIODIC); 
  TimerPrescaleSet(TIMER0_BASE, TIMER_A, 99);             //  Ԥ�Ƚ���100��Ƶ
  TimerLoadSet(TIMER0_BASE, TIMER_A, 3000);              //  ����Timer��ֵ����ʱ500ms
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT );        //  ʹ��Timer��ʱ�ж�
  IntEnable(INT_TIMER0A);                                 //  ʹ��Timer�ж�
  IntMasterEnable();                                      //  ʹ�ܴ������ж�
  TimerEnable(TIMER0_BASE, TIMER_A);                      //  ʹ��Timer����
}

void Front( void ){
  TimerMatchSet(TIMER1_BASE, TIMER_A, 600);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 600);     //  ����TimerBƥ��ֵ;  
}

void Back( void ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 600);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 600);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;  
}

void Stay( void ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;  
}

void Right( void ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 800);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;  
}

void Left( void ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 800);     //  ����TimerBƥ��ֵ;  
}
//  ��������������ڣ�
int main(void)
{
  jtagWait();                                             //  ��ֹJTAGʧЧ����Ҫ��
  clockInit();                                            //  ʱ�ӳ�ʼ��������6MHz
  
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
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 800);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;  
    }else if ( ( E1 != 0 ) && ( E2 == 0 ) ){
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 800);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;  
    }else if ( ( E1 == 0 ) && ( E2 == 0 ) ){
      Front();
    } 
    if ( ( E1 != 0 ) && ( E2 != 0 ) && ( G2 != 0 )  && ( E0 == 0  )){  
      Stay() ;
      SysCtlDelay(5000 * (TheSysClock / 3000));           //  ��ʱԼ5000ms
    }
  }
}


//  ��ʱ�����жϷ�����
void Timer0A_ISR(void)
{
  unsigned long ulStatus;
  
  ulStatus = TimerIntStatus(TIMER0_BASE, true);           //  ��ȡ�ж�״̬
  TimerIntClear(TIMER0_BASE, ulStatus);                   //  ����ж�״̬����Ҫ��
  
  if (ulStatus & TIMER_TIMA_TIMEOUT)                      //  �����Timer��ʱ�ж�
  {
    
  }
}