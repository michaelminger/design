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

// ���ϳ��ı�ʶ������Ϊ�϶̵���ʽ
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
// ����PID�����ṹ��
///////////////////////////////////////////////////////////////
//λ��ʽPID
typedef struct PID { 
   double SetPoint;   // �趨Ŀ�� Desired Value 
   double Proportion; // �������� Proportional Const 
   double Integral;   // ���ֳ��� Integral Const 
   double Derivative; // ΢�ֳ��� Derivative Const 
   double LastError;  // Error[-1]
   double PrevError;  //Error[-2]
   double SumError;   // Sums of Errors
} PID;

static PID vPID_X;                        //����ṹ������
static PID *fptr_X = &vPID_X ;

static PID vPID_A;                        //����ṹ������
static PID *fptr_A = &vPID_A ;

// ����PID�����ṹ��
///////////////////////////////////////////////////////////////
int Pdata_X  = 80 , Idata_X = 0 , Ddata_X = 0 ;
int Pdata_A  = 10 , Idata_A = 0 , Ddata_A = 0 ;

void IncPIDInit(void)
{
  fptr_X->LastError = 0.0;      //Error[-1]
  fptr_X->PrevError = 0.0;      //Error[-2]
  fptr_X->Proportion = Pdata_X; //�������� Proportional Const
  fptr_X->Integral =   Idata_X; //���ֳ���Integral Const
  fptr_X->Derivative = Ddata_X; //΢�ֳ��� Derivative Const
  fptr_X->SetPoint =0.0;        //Ŀ����30
  
  fptr_A->LastError = 0.0;      //Error[-1]
  fptr_A->PrevError = 0.0;      //Error[-2]
  fptr_A->Proportion = Pdata_A; //�������� Proportional Const
  fptr_A->Integral =   Idata_A; //���ֳ���Integral Const
  fptr_A->Derivative = Ddata_A; //΢�ֳ��� Derivative Const
  fptr_A->SetPoint =0.0;        //Ŀ����30
}


//  ��ֹJTAGʧЧ
void  JTAG_Wait(void){
  SysCtlPeriEnable(KEY_PERIPH);                               //  ʹ��KEY���ڵ�GPIO�˿�
  GPIOPinTypeIn(KEY_PORT , KEY_PIN);                          //  ����KEY���ڹܽ�Ϊ����
  if ( GPIOPinRead(KEY_PORT , KEY_PIN)  ==  0x00 ){           //  �����λʱ����KEY�������
    while(1);                                               //  ��ѭ�����Եȴ�JTAG����
  }
  SysCtlPeriDisable(KEY_PERIPH);                              //  ��ֹKEY���ڵ�GPIO�˿�
}


//  ����ȫ�ֵ�ϵͳʱ�ӱ���
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

//  ϵͳ��ʼ��
void  SystemInit(void){
  SysCtlClockSet(SYSCTL_USE_PLL |                             //  ϵͳʱ�����ã�����PLL
                 SYSCTL_OSC_MAIN |                            //  ������
                   SYSCTL_XTAL_6MHZ |                           //  ���6MHz����
                     SYSCTL_SYSDIV_4);                            //  ��Ƶ���Ϊ20MHz
  
  TheSysClock  =  SysCtlClockGet();                           //  ��ȡϵͳʱ�ӣ���λ��Hz
  
  KEY_Init(KEY1 | KEY2);                                      //  KEY��ʼ��
}


void Time_Init( void ) {
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER0);                 //  ʹ��Timerģ��
  TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR |     //  ����TimerΪ16λ���ڶ�ʱ��
                 TIMER_CFG_A_PERIODIC);
  TimerPrescaleSet(TIMER0_BASE, TIMER_A, 99);             //  Ԥ�Ƚ���100��Ƶ
  TimerLoadSet(TIMER0_BASE, TIMER_A, 30000);              //  ����Timer��ֵ����ʱ500ms
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);        //  ʹ��Timer��ʱ�ж�
  IntEnable(INT_TIMER0A);                                 //  ʹ��Timer�ж�
  IntMasterEnable();                                      //  ʹ�ܴ������ж�
  TimerEnable(TIMER0_BASE, TIMER_A);                      //  ʹ��Timer����
}



int  main(void){
  JTAG_Wait();                                                //  ��ֹJTAGʧЧ����Ҫ��
  SystemInit();    
  init_lcd();
  I2C_Init();   
  Time_Init();
  uartInit();                                             //  UART��ʼ��
  
  IncPIDInit();
  Infrar_Init();
  Moto_Init();
  
  while(1){ 
    sprintf(dBuf, "��ǰΪ����%d      ", Ti3 + 1 );     //  ����ֵ��ʽ��Ϊ��ѹֵ
    sprintf(cBuf, "ǰ��: %d s         ", T_F);     //  ����ֵ��ʽ��Ϊ��ѹֵ
    sprintf(eBuf, "����: %d s         ", T_B);     //  ����ֵ��ʽ��Ϊ��ѹֵ
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
/// ��������
/////////////��ǰ��+���� +��ʱ
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
      SysCtlDelay(5000 * (TheSysClock / 3000));           //  ��ʱԼ5000ms
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
      SysCtlDelay(5000 * (TheSysClock / 3000));           //  ��ʱԼ5000ms
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



//  ��ʱ�����жϷ�����
void Timer0A_ISR(void)
{
  unsigned long ulStatus;
  
  ulStatus = TimerIntStatus(TIMER0_BASE, true);           //  ��ȡ�ж�״̬
  TimerIntClear(TIMER0_BASE, ulStatus);                   //  ����ж�״̬����Ҫ��
  if (ulStatus & TIMER_TIMA_TIMEOUT)                      //  �����Timer��ʱ�ж�
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


//  UART0�жϷ�����
void UART0_ISR(void)
{
  
  unsigned long ulStatus;
  
  ulStatus = UARTIntStatus(UART0_BASE, true);             //  ��ȡ��ǰ�ж�״̬
  UARTIntClear(UART0_BASE, ulStatus);                     //  ����ж�״̬
  
  if ((ulStatus & UART_INT_RX) || (ulStatus & UART_INT_RT))   //  ���ǽ����жϻ���
  {  
    while(1){
      Re_buf[counter]= uartGetc0();
      if(counter==0&&Re_buf[0]!=0x55) return;      //��0�����ݲ���֡ͷ
      
      counter++; 
      if(counter==11)             //���յ�11������
      {    
        counter=0;               //���¸�ֵ��׼����һ֡���ݵĽ���        
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
  SysCtlPeriEnable( Infrared_S_E );                                  //  ʹ��LED1���ڵ�GPIO�˿�
  SysCtlPeriEnable( Infrared_S_G );                                  //  ʹ��LED1���ڵ�GPIO�˿�
  
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_0 );                    //  ����LED1���ڵĹܽ�Ϊ���
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_1 );            //��LED1���ڵĹܽ�Ϊ���
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_2 );                    //  ����LED1���ڵĹܽ�Ϊ���
  GPIOPinTypeIn( Infrared_PORT_E , GPIO_PIN_3 );                    //  ����LED1���ڵĹܽ�Ϊ���
  GPIOPinTypeIn( Infrared_PORT_G , GPIO_PIN_2 );                    //  ����LED1���ڵĹܽ�Ϊ���
  GPIOPinTypeIn( Infrared_PORT_G , GPIO_PIN_3 );                    //  ����LED1���ڵĹܽ�Ϊ���
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


void Front( int speed ){
  TimerMatchSet(TIMER1_BASE, TIMER_A, speed);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, speed);     //  ����TimerBƥ��ֵ;  
}

void Back(  int speed  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, speed);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, speed);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;  
}

void Stay(  void  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;  
}

void Right_F( int speed  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, speed);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;  
}

void Left_F(  int speed  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, speed);     //  ����TimerBƥ��ֵ;  
}


void Right_B( int speed  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, speed);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;  
}

void Left_B(  int speed  ) {
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;
  TimerMatchSet(TIMER2_BASE, TIMER_A, speed);     //  ����TimerBƥ��ֵ
  TimerMatchSet(TIMER2_BASE, TIMER_B, 0);     //  ����TimerBƥ��ֵ;  
}

void  Delay(unsigned long x){
  unsigned long DelayValue = 0;
  for (DelayValue = 0; DelayValue < x; DelayValue++);
}  


/*================================ PID ���㲿�� ===============================*/
double PIDCalc( PID *pp, double NextPoint )
{ 
   double dError, Error; 
   Error = pp->SetPoint - NextPoint; // ƫ�� 
   pp->SumError += Error; // ���� 
   dError = pp->LastError - pp->PrevError; // ��ǰ΢�� 
   pp->PrevError = pp->LastError; 
   pp->LastError = Error; 
   return (pp->Proportion * Error // ������ 
           + pp->Integral * pp->SumError // ������
             + pp->Derivative * dError  // ΢����
               );
} 


//Title:����ʽPID�㷨����
//Description:����һ���������
//Input: PID��P��I���Ƴ�����֮ǰ���������PID *pp��& ��ǰ�������ThisError��
//Return: �������templ
//////////////////////////////////////////////////////////////////////
float PIDCal( PID *pp, float ThisError )
{
 //����ʽPID�㷨����Ҫ���ƵĲ��ǿ������ľ���ֵ�����ǿ�������������
    float  pError,dError,iError;
    float  templ; 
    fptr_A->Proportion = Pdata_A; //�������� Proportional Const
    fptr_A->Integral   = Idata_A; //���ֳ���Integral Const
    fptr_A->Derivative = Ddata_A; //΢�ֳ��� Derivative Const
    pError = ThisError- fptr_A->LastError; 
    iError = ThisError;
    dError = ThisError-2*( fptr_A->LastError)+ fptr_A->PrevError;
    
    //�������� 
    templ= fptr_A->Proportion*pError +  fptr_A->Integral*iError+ fptr_A->Derivative*dError;  //����

    //�洢��������´�����
     fptr_A->PrevError  =  fptr_A->LastError;
     fptr_A->LastError = ThisError;
     
     return (templ);
}
 