//  ������Ҫ��ͷ�ļ�
#include  "softi2c.h"
#include  "ZLG7290.h"
//#include  "buzzer.h"
//#include  "CLOCK.h"

#include  <hw_types.h>
#include  <hw_memmap.h>
#include  <hw_sysctl.h>
#include  <hw_gpio.h>
#include  <sysctl.h>
#include  <gpio.h>
#include  <string.h>

#define  ZLG7290    0x70     //оƬ��ַ

#define  SubKey     0x01 
#define  SubCmdBuf  0x07
#define  SubDpRam   0x10

unsigned char disp_buf[]={1,2,3,4,5,6,7,8};

unsigned char key_code;
unsigned char key_press;

//=============================================
// void ZLG7290_ReadData(unsigned char address,unsigned char num)
// ��ZLG7290��num���ֽڷ���temp��
// ���룺address оƬ���ӵ�ַ
//       num �ֽ���
// �������
//=============================================
void ZLG7290_ReadData(unsigned char address,unsigned char num)
{   
    I2C_On(I2C_MODE_SrRECV,ZLG7290,address,I2C_temp,num);
}

//=============================================
// void ZLG7290_WriteData(unsigned char address,unsigned char *buf,unsigned char num)
// ��buf�е�num���ֽ�д��ZLG7290��
// ���룺address оƬ���ӵ�ַ
//       num �ֽ���
// �������
//=============================================
void ZLG7290_WriteData(unsigned char address,unsigned char *buf,unsigned char num)
{   
    I2C_On(I2C_MODE_SEND,ZLG7290,address,buf,num);
}

//  =================================
//  ����ZLG7290��������ֵ
//  =================================
void Read_ZLG7290Key(void)
{
    char  usTmp;
    
    ZLG7290_ReadData(1,1);
    usTmp=I2C_temp[0];
    if(usTmp !=0)
    {
        if(usTmp<0x11)
        {
            key_press=1;
            key_code=usTmp;
         //   bell();
        }
    }
}    

/*********************************************************************************************************
** ��������: 	DelayNS
** ��������: 	�������ʱ
** �䡡��: 		i :	��ʱ������ֵԽ��ʱ��ʱԽ��   
** �䡡��: 		��
** ȫ�ֱ���:	��
** ����ģ��: 	��
********************************************************************************************************/
void delayMS(unsigned int i)
{ 
	unsigned int j,k;
	for(k=0;k<i;k++)
		for(j=0;j<60;j++);
}

/*********************************************************************************************************
** ��������: 	ZLG7290_SendData
** ��������: 	��������
** �䡡��:SubAdd:	��������
**       DATA 	:	����ֵ       
** �䡡��: 		0 �� Fail
**        		1 :  OK
** ȫ�ֱ���:	��
** ����ģ��: 	delayMS
********************************************************************************************************/

unsigned char ZLG7290_SendData(unsigned char SubAdd,unsigned char Data)
{
        I2C_temp[0]=Data;
        if(SubAdd>0x17)
	{
           return 0;
        }
        ZLG7290_WriteData(SubAdd,I2C_temp,1);
	return 1;
}
/*********************************************************************************************************
** ��������: 	ZLG7290_SendCmd
** ��������: 	����������ӵ�ַ7��8��
** �䡡��:DATA1:����1
**       DATA2:	����2        
** �䡡��: 		0 �� Fail
**        		1 :  OK
** ȫ�ֱ���:	��
** ����ģ��: 	ISendStr��delayMS
********************************************************************************************************/

unsigned char ZLG7290_SendCmd(unsigned char Data1,unsigned char Data2)
{
    I2C_temp[0]=Data1;
    I2C_temp[1]=Data2;
    ZLG7290_WriteData(7,I2C_temp,2);
    delayMS(100);
    return 1;
}  

void zlg7290_test(void)
{
    unsigned char i,j;
    ZLG7290_SendData(0x0d,7);
    clear_zlg7290_disp();
    for(i=0;i<0x20;i++)
    {
        for(j=0;j<8;j++)
        {
            disp_buf[j]=i;
        }
         zlg7290_disp();
         delayMS(80000);        
    }
    for(j=0;j<8;j++)
    {
        disp_buf[j]=j+1;
    }
    zlg7290_disp();
    delayMS(400000); 
    clear_zlg7290_disp();     
}    

void zlg7290_disp(void)
{
    char i;
    for(i=0;i<8;i++)
    {
        ZLG7290_SendCmd(0x60+i,disp_buf[i]);
    }
}      

void clear_zlg7290_disp(void)
{
    char i;
    for(i=0;i<8;i++)
    {
        ZLG7290_SendCmd(0x60+i,0x1f);
    }
}

/*void zlg7290_disp_clock(void)
{
    if(OLD_sec !=sec)
    {
        OLD_sec =sec;
        ZLG7290_SendCmd(0x60,hour/10);
        ZLG7290_SendCmd(0x61,hour%10);
        ZLG7290_SendCmd(0x01,0x16+0x80);
    
        ZLG7290_SendCmd(0x63,min/10);
        ZLG7290_SendCmd(0x64,min%10);
        ZLG7290_SendCmd(0x01,0x2e+0x80);    
        ZLG7290_SendCmd(0x66,sec/10);
        ZLG7290_SendCmd(0x67,sec%10);
    }
}      
*/
/*void key_fx(void)
{
     switch (key_code)
     {
          case 1:
               year++;
               if(year>99)
               {
                    year=0;
               }
               break;
          case 2:
               year--;
               if(year==255)
               {
                    year=99;
               }
               break;
          case 3:
               month++;
               if(month>12)
               {
                    month=1;
               }
               break;
          case 4:
               month--;
               if(month<1)
               {
                    month=12;
               }
               break;
          case 5:
               day++;
               if(day>31)
               {
                    day=1;
               }
               break;
          case 6:
               day--;
               if(day<1)
               {
                    day=31;
               }
               break;
          case 7:
               hour++;
               if(hour>23)
               {
                    hour=0;
               }
               break;
          case 8:
               hour--;
               if(hour==255)
               {
                    hour=23;
               }
               break;
          case 9:
               min++;
               if(min>59)
               {
                    min=0;
               }
               break;
          case 10:
               min--;
               if(min==255)
               {
                    min=59;
               }
               break;
          case 11:
               sec=0;
               break;
          case 12:
               sec=0;
               break;
          case 13:
               week++;
               if(week>6)
               {
                    week=0;
               }
               break;
          case 14:
               week--;
               if(week==255)
               {
                    week=6;
               }
               break;
     }
 //    P8563_settime();  
}*/