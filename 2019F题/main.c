#include  "systemInit.h"
#include  <uart.h>
#include  "SoftI2C.H"
#include  "SoftI2C_CFG.H"
#include  "uartGetPut.h"
#include "FDC2214.h"
#include <stdio.h>
#include  "lcd_driver.h"
#include  "ZLG7290.h"
#include  "buzzer.h"
#define  PART_LM3S1138
#include  <pin_map.h>
#include <stdlib.h>
#include<time.h>
#include  "systemInit.h"
#include  <timer.h>
#include  "KEY.H"

//  ���ϳ��ı�ʶ������ɽ϶̵���ʽ
#define  SysCtlPeriEnable       SysCtlPeripheralEnable
#define  SysCtlPeriDisable      SysCtlPeripheralDisable
#define  GPIOPinTypeIn          GPIOPinTypeGPIOInput
#define  GPIOPinTypeOut         GPIOPinTypeGPIOOutput

#define  Key_1                  GPIOPinRead(GPIO_PORTE_BASE , GPIO_PIN_0)
#define  Key_2                  GPIOPinRead(GPIO_PORTE_BASE , GPIO_PIN_1)
#define  Key_3                  GPIOPinRead(GPIO_PORTD_BASE , GPIO_PIN_3)

//  ����KEY
#define  KEY_PERIPH             SYSCTL_PERIPH_GPIOG
#define  KEY_PORT               GPIO_PORTG_BASE
#define  KEY_PIN                GPIO_PIN_5

int T1 , T2 , T3 , T4 ;
unsigned long k;
int Real_Num , Step_All ;
char a[10];
int Data_Mar[100][2] ;
int Ans_Data ;
double comPare[500] = {0}  ;

void  Data_N ( int Array[][2] , int m );
void Time_Init( void ) {
  SysCtlPeriEnable(SYSCTL_PERIPH_GPIOD);                           //  ʹ��iic���ڵ�GPIO�˿�
  GPIOPinTypeIn(GPIO_PORTD_BASE, GPIO_PIN_0);                       //  ����iic���ڹܽ�Ϊ����
  GPIOPinTypeIn(GPIO_PORTD_BASE, GPIO_PIN_3);                       //  ����iic���ڹܽ�Ϊ����
  
  SysCtlPeriEnable(SYSCTL_PERIPH_GPIOE);                           //  ʹ��iic���ڵ�GPIO�˿�
  GPIOPinTypeIn(GPIO_PORTE_BASE, GPIO_PIN_0);                       //  ����iic���ڹܽ�Ϊ����
  GPIOPinTypeIn(GPIO_PORTE_BASE, GPIO_PIN_1);                       //  ����iic���ڹܽ�Ϊ����
 
  
  GPIOPinTypeOut(GPIO_PORTE_BASE, GPIO_PIN_2);                       
  GPIOPinTypeOut(GPIO_PORTE_BASE, GPIO_PIN_3);                      
  GPIOPinWrite( GPIO_PORTE_BASE, GPIO_PIN_2, 0x01<<2);                     
  GPIOPinWrite( GPIO_PORTE_BASE, GPIO_PIN_3, 0x01<<2);
  
  
  SysCtlPeriEnable(SYSCTL_PERIPH_TIMER0);                 //  ʹ��Timerģ��
  TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR |     //  ����TimerΪ16λ���ڶ�ʱ��
                 TIMER_CFG_B_PERIODIC);
  
  TimerPrescaleSet(TIMER0_BASE, TIMER_B, 99);             //  Ԥ�Ƚ���100��Ƶ
  TimerLoadSet(TIMER0_BASE, TIMER_B, 30000);              //  ����Timer��ֵ����ʱ500ms
  TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);        //  ʹ��Timer��ʱ�ж�
  IntEnable(INT_TIMER0B);                                 //  ʹ��Timer�ж�
  IntMasterEnable();                                      //  ʹ�ܴ������ж�
  TimerEnable(TIMER0_BASE, TIMER_B);                      //  ʹ��Timer����
  
}

//  ��������������ڣ�
int main(void)

{ 
  jtagWait();                                             //  ��ֹJTAGʧЧ����Ҫ��
  clockInit();                                            //  ʱ�ӳ�ʼ��������6MHz
  I2C_Init();
  uartInit();
  init_lcd();
  Time_Init();
  buzzerInit();//  ��������ʼ��
  
  if(FDC2214_Init()==1){
    uartPuts("��ʼ���ɹ�\r\n");
    disp_line(0,"  ��ʼ���ɹ��� ");
    SysCtlDelay(100 * (TheSysClock / 3000));
    GUI_ClearSCR(0);
  }
  else{
    uartPuts("��ʼ��ʧ������ϵͳ������");
    disp_line(0,"  ��ʼ��ʧ�ܣ�  ");
    disp_line(1,"����ϵͳ������");
    while(1);
  }
  
  
  while(1){
    if ( ( T1 == 0 ) && ( T2 == 0 ) && ( T3 == 0 ) && ( T4 == 0 ) ){
      T3 = 1 ;
      GUI_ClearSCR(0);
      disp_line(0,"ʵ���Լ칦�ܣ� 1 ");
      disp_line(0,"ʵ���Լ칦�ܣ� 1 ");
      disp_line(1,"����ֽ�Ź��ܣ� 2 ");
      disp_line(2,"ֽ�����Ͳ���:  3 ");
      disp_line(3,"                        ");
    }
    if ( T1 == 1 ){
      T3 = 0 ;
      TimerDisable(TIMER0_BASE, TIMER_B);                      //  ʹ��Timer����
      int N1 = 0 , N2 = 0 ,N3 = 0 ;
      int Step = 0 , i ; 
      while( T1 == 1 ) {
        FDC2214_GetChannelData(FDC2214_Channel_0, &k);
        Real_Num = ( int )( k / 1000.0 );
        sprintf(a,"��������%d\r\n", Real_Num);
        uartPuts(a); 
        sprintf(a,"��������%d", Real_Num);
        disp_line(3,a);
        T1 = 1 ;
        if ( Real_Num > 50000 ){                        
          GPIOPinWrite( GPIO_PORTE_BASE, GPIO_PIN_2, 0x00 );buzzerSound(2000);                                      //  ����������2000Hz����
          SysCtlDelay(800 * (TheSysClock / 3000));                //  ��ʱԼ800ms
        }else {                        
          GPIOPinWrite( GPIO_PORTE_BASE, GPIO_PIN_2, 0x01<<2 );
          buzzerQuiet();                                         //  ����������
        }
        if ( N1 == 0 ){
          N1 = 1 ;
          disp_line(0,"ȷ��ֽ��ֵ��   ");
          disp_line(1,"����2 ��ʼ��¼1          ");
          disp_line(2,"                        ");
          disp_line(3,"                        ");
        }
        
        if ( ( Key_2 == 0 ) && ( N2 == 0 ) ){
          N2 = 1 ;
          Data_Mar[Step][0] = Step  ;
          Data_Mar[Step][1] = Real_Num ;
          
          sprintf(a,"%d  %d  ", Data_Mar[Step][0] ,Data_Mar[Step][1] );
          uartPuts(a); 
          
          sprintf(a,"��%d ҳ��¼�ɹ�    ", Step+1);
          if( Step >= 10  )
            sprintf(a,"��%dҳ��¼�ɹ�    ", Step+1);
          uartPuts(a); 
          disp_line(1,a);
          Step ++ ;
          Data_Mar[Step][0] = Step  ;
          Step ++ ;
          sprintf(a,"��%d ҳ��ʼ��¼    ", Step+1);
          if( Step >= 10  )
            sprintf(a,"��%dҳ��ʼ��¼    ", Step+1);
          uartPuts(a); 
          disp_line(2,a);
          Step_All = Step  ;
          N3 = 1 ;
          T1 = 1 ;
        }
        if( Key_2 != 0  ){
          N2 = 0 ;
        }
        if( ( Key_1 == 0 ) && ( N3 == 1 ) ){
          disp_line(1,"�Լ�ɹ�          ");
          disp_line(2,"һ������ҳ�� ");
          disp_line(3,"                        ");
          
          for (i = 0  ; i < Step_All ; i ++  ){
            sprintf(a,"%d  %d  \r\n", Data_Mar[i][0] ,Data_Mar[i][1] );
            uartPuts(a); 
          }
          
          Data_N ( Data_Mar , Step_All   );
          
          for ( i = 0 ; i < Step_All ; i ++  ){
            sprintf(a,"%d %lf   \r\n", i , comPare[i] );
            uartPuts(a); 
          }
          
          T1 = 0 ;
          T2 = 0 ;
          T3 = 0 ;
          T4 = 0 ;
          N1 = 0 ;
          N3 = 0 ;
        }
      }
      
      TimerEnable(TIMER0_BASE, TIMER_B);                      //  ʹ��Timer����
    }else if ( T2 == 1  ){
      int N1 = 0 , N2 = 0 ,Err_1 , Err_2 , Ans , Error , N4 = 0 ;
      T3 = 0 ;
      TimerDisable(TIMER0_BASE, TIMER_B);                      //  ʹ��Timer����
      while( T2 == 1 ) {
        
        if ( N1 == 0 ){
          N1 = 1 ;
          sprintf(a,"���ֽ��ҳ����%d", Step_All );
          disp_line(0,a);
          disp_line(1,"����1 ��ʼ���          ");
          disp_line(2,"                        ");
          disp_line(3,"                        ");
        }
        
        if ( N2 == 0 ){
          while( N2 == 0 ){
            FDC2214_GetChannelData(FDC2214_Channel_0, &k);
            Real_Num = ( int )( k / 1000.0 );
            sprintf(a,"��������%d\r\n", Real_Num);
            uartPuts(a); 
            N2 = 0 ;
            if(  Key_1 == 0 ){
              N2 = 1 ;
            }
          }
          
          for ( Error = 0 ; Error < 2 * Step_All ; Error  ++  ){
            if ( comPare[Error] >=  Real_Num ){
              break ;
            }
          }
          
          Err_1 = comPare[Error] - Real_Num ;
          Err_2 = Real_Num - comPare[Error - 1 ];
          
          Ans = ( ( Err_1 >= Err_2 ? ( Error - 1 ) : Error ) );
          sprintf(a,"ҳ��Ϊ: %d��    ", ( Ans + 1 ));
          uartPuts(a); 
          disp_line(1,a);
          disp_line(3,"����2 �˳�          ");
          while(1){
            if ( Key_2 == 0 ){
              N4 = 1 ;
            }
            if ( ( Key_2 != 0) && ( N4 == 1 ) ){
              T1 = 0 ;
              T2 = 0 ;
              T3 = 0 ;
              T4 = 0 ;
              break ;
              
            }
          }
        }
      }
      TimerEnable(TIMER0_BASE, TIMER_B);                      //  ʹ��Timer����
    }else if( T4 == 1 ){
      int K1 = 0 , K2 = 0 , K3 = 0;
      T3 = 0 ;
      TimerDisable(TIMER0_BASE, TIMER_B);                      //  ʹ��Timer����
      while( T4 == 1){
        
        if( K1 == 0 ){
          K1 = 1;         
          disp_line(0,"ֽ�����Ͳ���:   ");
          disp_line(0,"ֽ�����Ͳ���:   ");
          disp_line(1,"����1 ��ʼ���          ");
          disp_line(2,"                        ");
          disp_line(3,"                        ");
        }
        
        if( K2 == 0 ){
          while( K2 == 0 ){
            FDC2214_GetChannelData(FDC2214_Channel_0, &k);
            Real_Num = ( int )( k / 1000.0 );
            sprintf(a,"��������%d\r\n", Real_Num);
            uartPuts(a);
            disp_line(3,a);
            K2 = 0 ;
            if(  Key_1 == 0 ){
              K2 = 1 ;
            }
          }
          //��ֽ��8441  ��ֽ��7389  ��ҵֽ��7706
/*          if(){                                   
            disp_line(1,"      ��ֽ      ");
            disp_line(3,"����2 �˳�          ");
          }
          else if(){
            disp_line(1,"      ��ֽ      ");
            disp_line(3,"����2 �˳�          ");
          }
          else if(){
            disp_line(1,"     ��ҵֽ     ");
            disp_line(3,"����2 �˳�          ");
          }*/
          
          while(1){
            if ( Key_2 == 0 ){
              K3 = 1 ;
            }
            if ( ( Key_2 != 0) && ( K3 == 1 ) ){
              T1 = 0 ;
              T2 = 0 ;
              T3 = 0 ;
              T4 = 0 ;
              break ;
              
            }
          }
        } 
      } 
      TimerEnable(TIMER0_BASE, TIMER_B);                      //  ʹ��Timer����
    }
  }
}


void  Data_N ( int Array[][2] , int m  ){
  int  i = 0 , j = 0 , Num_B = 0  , Num_A = 0 ;
  for( i = 0 ; i <= m  ; i ++  ){
    Num_B = Array[i][0] ;
    Num_A = Array[i][1] ;
    comPare[Num_B] = Num_A ;
  }
  
  if ( comPare[0] == 0 ){
    comPare[0] = 1 ;
  }
  
  for ( j = 1 ; j < m  ; j ++ ){
    double Num_Left = 0 , Num_right = 0;
    int k1 = 0 , k2 = 0   ;
    if ( comPare[j] == 0 ){
      for ( k1 = j - 1  ; k1 >= 0 ; k1 -- ){
        if( comPare[k1] != 0 ){
          Num_Left = comPare[k1];
          break;
        }
      }
      if ( Num_Left == 0  ){
        Num_Left = 1 ;
      }
      for ( k2 = j + 1 ; k2 < 2*m ; k2 ++ ){
        if( comPare[k2] != 0 ){
          Num_right = comPare[k2] ;
          break ;
        }
      }
      if ( Num_right == 0 ){
        comPare[j] = comPare[j - 1 ] + 1 ;
      }else{
        comPare[j] = ( Num_Left + Num_right ) / 2.0  ;
      }
    }
  }
}



//  ��ʱ�����жϷ�����
void Timer0B_ISR(void)
{
  unsigned long ulStatus;
  
  ulStatus = TimerIntStatus(TIMER0_BASE, true);           //  ��ȡ�ж�״̬
  TimerIntClear(TIMER0_BASE, ulStatus);                   //  ����ж�״̬����Ҫ��
  
  if (ulStatus & TIMER_TIMB_TIMEOUT)                      //  �����Timer��ʱ�ж�
  {
    if (  Key_1 == 0 ){
      T1 = 1 ;
      T2 = 0 ;
      T4 = 0 ;
    }
    else if ( Key_2 == 0 ){
      T1 = 0 ;
      T2 = 1 ;
      T4 = 0 ;
    }
    else if ( Key_3 == 0 ){
      T1 = 0 ;
      T2 = 0 ;
      T4 = 1 ;
    }
    
  }
}
