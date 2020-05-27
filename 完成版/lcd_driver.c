//  ������Ҫ��ͷ�ļ�
//  ������Ҫ��ͷ�ļ�
#include  "lcd_driver.h"

#include  <hw_types.h>
#include  <hw_memmap.h>
#include  <hw_sysctl.h>
#include  <hw_gpio.h>
#include  <hw_adc.h>
#include  <sysctl.h>
#include  <gpio.h>
#include  <adc.h>
#include  <stdio.h>


// ���ϳ��ı�ʶ������Ϊ�϶̵���ʽ
#define  SysCtlPeriEnable       SysCtlPeripheralEnable
#define  SysCtlPeriDisable      SysCtlPeripheralDisable
#define  GPIOPinTypeIn          GPIOPinTypeGPIOInput
#define  GPIOPinTypeOut         GPIOPinTypeGPIOOutput
#define  GPIOPinTypeOD          GPIOPinTypeGPIOOutputOD


#define  LCD_PORT               GPIO_PORTF_BASE
#define  CS                     GPIO_PIN_1       
#define  SDAT                   GPIO_PIN_2
#define  SCLK                   GPIO_PIN_3

unsigned char pos[4]={0x80,0x90,0x88,0x98};
unsigned char data[4][16]; 
unsigned int gui_disp_buf[8][64]={0};//������������

const unsigned char ascii_tab[]={"0123456789ABCDEF"};

unsigned char line_sign=0;

extern float fTemp;
extern void  Delay(unsigned long x);

//============================================================================================
//*** ��    ��: sdelay()
//*** ��    �ܣ���ʱ����
//*** ��    ��: ��ʱ��������
//============================================================================================
void sdelay(unsigned long x)
{   
    while(x--);
}           

//============================================================================================    
//*** ��    ��:send_data()
//*** ��    �ܣ�Һ��������λ����
//*** ��    ��: data   ������λ������
//============================================================================================
void send_data(unsigned char data)     //8λΪ��������
{   
    unsigned char i=8,data1;
    while (i--)
    {
	data1=data & 0x80;    //��һ��d7�Ǹ��ǵ�
        if (data1)            //�ǸߵĻ��ͳ�1
        {
            GPIOPinWrite(LCD_PORT , SDAT , 0xff);
	    sdelay(400);
	}
	else                  //����Ļ��ͳ�0
	{
            GPIOPinWrite(LCD_PORT , SDAT , 0x00);
	    sdelay(400);                         //����æ�ߣ���Ҫ�ȵ���æΪֹ��
	}
	GPIOPinWrite(LCD_PORT , SCLK , 0xff);    //sclk=1
        sdelay(400) ;
	GPIOPinWrite(LCD_PORT , SCLK , 0x00);    //sclk=0
	sdelay(400) ;
	data=data<<1;
    }
}
//============================================================================================    
//*** ��    ��:write_data()
//*** ��    ��: д����
//*** ��    ��: data   RW=1,RS=1ʱ���͵�����ָ��
//============================================================================================
void write_data(unsigned char data)              //��λ�Ǵ�д������
{	
     unsigned char data1=0xfa,data2,data3;   //data2�д�Ÿ�4λ���ݣ�data3�д�ŵ�4λ����

     GPIOPinWrite(LCD_PORT , CS , 0xff);     //cs=1
     sdelay(400);
     data2=data & 0xf0;						//ȡ��4λ����
     data3=(data & 0x0f)<<4;					//ȡ��4λ����
     send_data(data1);
     send_data(data2);
     send_data(data3);
     GPIOPinWrite(LCD_PORT , CS , 0x00);     //cs=0
     sdelay(400);	
}
//============================================================================================    
//*** ��    ��:write_comm()
//*** ��    ��: д����
//*** ��    ��: data   RW=0,RS=0ʱ���͵�����ָ��
//============================================================================================
void write_comm(unsigned char data)				//�Ͱ�λ�Ǵ�д������
{
     unsigned char data1=0xf8,data2,data3;   //data2�д�Ÿ�4λ���ݣ�data3�д�ŵ�4λ����

     GPIOPinWrite(LCD_PORT , CS , 0xff);     //cs=1
     sdelay(400);
     data2=data & 0xf0;						//ȡ��4λ����
     data3=(data & 0x0f)<<4;					//ȡ��4λ����
     send_data(data1);
     send_data(data2);
     send_data(data3);
     GPIOPinWrite(LCD_PORT , CS , 0x00);     //cs=0
     sdelay(400);	
}
  
//============================================================================================    
//*** ��    ��:init_lcd()
//*** ��    ��: LCM��ʼ��
//*** ��    ��: ��
//============================================================================================
void init_lcd(void)
{	
      SysCtlPeriEnable(SYSCTL_PERIPH_GPIOF);          //  ʹ��GPIOF�˿�
      GPIOPinTypeOut(LCD_PORT , CS);           //  ����PF1Ϊ�������
      GPIOPinTypeOut(LCD_PORT , SDAT);         //  ����PF2Ϊ�������
      GPIOPinTypeOut(LCD_PORT , SCLK);         //  ����PF3Ϊ�������


      GPIOPinWrite(LCD_PORT , CS , 0x00);      //CS=0
      GPIOPinWrite(LCD_PORT , SDAT , 0x00);    //SDAT=0
      GPIOPinWrite(LCD_PORT , SCLK , 0x00);    //SCLK=0


      write_comm(0x30);			//����ָ�

      write_comm(0x01);			//�����ʾ��Ļ����DDRAMλַ����������Ϊ"00H"
      sdelay(40000);
      write_comm(0x03);			//��DDRAMλַ����������Ϊ"00H"���α��ԭ�㣬�ù��ܲ�Ӱ����ʾDDRAM
      sdelay(40000);
      write_comm(0x06);			//�������
      sdelay(4000);
      write_comm(0x0c);			//��ʾ����
      sdelay(4000);

}
//============================================================================================    
//*** ��    ��:screen()
//*** ��    ��: ȫ����ʾ
//*** ��    ��: data   ����ָ���Ҫ��ʾһά����
//============================================================================================
void screen(unsigned char data[])
{
      unsigned char i,j;

      for (i=0;i<4;i++)
      {
          switch(i)
          {
              case 0:
                  write_comm(0x80);    //�趨DDRAM��һ�У���һ��
                  break;
              case 1:   
                  write_comm(0x90);    //�趨DDRAM�ڶ��У���һ��
                  break;
              case 2: 
                  write_comm(0x88);    //�趨DDRAM�����У���һ��
                  break;
              case 3:   
                  write_comm(0x98);    //�趨DDRAM�����У���һ��
                  break;
          }
          for(j=0;j<16;j++) 
          write_data(data[i*16+j]); 
      }		       
}	

//============================================================================================    
//*** ��    ��:disp_line(unsigned char line,unsigned char data[])
//*** ��    ��: дһ������
//*** ��    ��: line   �У�0~3��
//*** ��    ��: data   ����ָ���Ҫ��ʾһά����(16�ֽڣ�
//============================================================================================
void disp_line(unsigned char line,unsigned char data[])
{
     unsigned char j;

     switch(line)
     {
             case 0:
	         write_comm(0x80);    //�趨DDRAM��һ�У���һ��
		 break;
	     case 1:   
		 write_comm(0x90);    //�趨DDRAM�ڶ��У���һ��
	         break;
	     case 2: 
		 write_comm(0x88);    //�趨DDRAM�����У���һ��
	         break;
	     case 3:   
	         write_comm(0x98);    //�趨DDRAM�����У���һ��
	         break;
     }
     for(j=0;j<16;j++) 
     write_data(data[j]);
     write_comm(0xa0);
}	


//============================================================================================    
//*** ��    ��:GUI_ClearSCR()
//*** ��    ��: ����������ָ��£�
//*** ��    ��: mode:  ģʽ
//                   0:����ָ�
//                   1:����ָ�
//============================================================================================
void GUI_ClearSCR(char mode)
{
    unsigned char x,y;
    unsigned char i,j;
    switch(mode)
    {
        case 0: 
                write_comm(0x32);  //����ָ�
                write_comm(0x01);  //�����Ļ
                break;
        case 1:
                write_comm(0x36);  //����ָ�
                for(i=0;i<8;i++)   //��д��������
                {
                    for(j=0;j<64;j++)
                    gui_disp_buf[i][j]=0x00;
                }  
                x=0x80;y=0x80; 
                write_comm(y);             //�������� 
                write_comm(x); 
                for (j=0;j<32;j++)
                {
                    for (i=0;i<16;i++)
                    {
                       write_data(0x00);
                    }   
             	    write_comm(++y);
                    write_comm(x);
	        }
                x=0x88;y=0x80;
                write_data(0x00);
                write_comm(y);
                write_comm(x);
                for (j=32;j<64;j++)
                {  
                    for (i=0;i<16;i++)
		    {
                        write_data(0x00);
                    }    
                    write_comm(++y);
	            write_comm(x); 	 
	        }
                write_comm(0x32);//����ָ�����ͼ��ʾ
      break;
    }   
}



void lcd_demo(void)
{
    GUI_ClearSCR(0);
    paint(paint_buffer1);
    Delay(200000);
    GUI_ClearSCR(0);
}

//============================================================================================    
//*** ��    ��:location(int y,int x)
//*** ��    ��:���ֶ�λ
//*** ��    ��:
//============================================================================================
void location(int y,int x)
{
  switch(y)
  {case 0:
    write_comm(0x80 | x);
    break;
   case 1:
    write_comm(0x90 | x);
    break;
   case 2:
    write_comm(0x88 | x);
    break;
   case 3: 
    write_comm(0x98 | x);
    break;
  }
}

void display( char *str)
{
  while(*str != '\0')
  {
    write_data(*str);
    str++;
  }
}


