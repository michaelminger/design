//  包含必要的头文件
//  包含必要的头文件
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


// 将较长的标识符定义为较短的形式
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
unsigned int gui_disp_buf[8][64]={0};//建立缓冲区域

const unsigned char ascii_tab[]={"0123456789ABCDEF"};

unsigned char line_sign=0;

extern float fTemp;
extern void  Delay(unsigned long x);

//============================================================================================
//*** 函    数: sdelay()
//*** 功    能：延时函数
//*** 参    数: 延时计数数据
//============================================================================================
void sdelay(unsigned long x)
{   
    while(x--);
}           

//============================================================================================    
//*** 函    数:send_data()
//*** 功    能：液晶串行移位数据
//*** 参    数: data   串行移位的数据
//============================================================================================
void send_data(unsigned char data)     //8位为待送数据
{   
    unsigned char i=8,data1;
    while (i--)
    {
	data1=data & 0x80;    //看一下d7是高是低
        if (data1)            //是高的话送出1
        {
            GPIOPinWrite(LCD_PORT , SDAT , 0xff);
	    sdelay(400);
	}
	else                  //否则的话送出0
	{
            GPIOPinWrite(LCD_PORT , SDAT , 0x00);
	    sdelay(400);                         //不读忙线，但要等到不忙为止。
	}
	GPIOPinWrite(LCD_PORT , SCLK , 0xff);    //sclk=1
        sdelay(400) ;
	GPIOPinWrite(LCD_PORT , SCLK , 0x00);    //sclk=0
	sdelay(400) ;
	data=data<<1;
    }
}
//============================================================================================    
//*** 函    数:write_data()
//*** 功    能: 写数据
//*** 参    数: data   RW=1,RS=1时传送的数据指令
//============================================================================================
void write_data(unsigned char data)              //八位是待写入数据
{	
     unsigned char data1=0xfa,data2,data3;   //data2中存放高4位数据，data3中存放低4位数据

     GPIOPinWrite(LCD_PORT , CS , 0xff);     //cs=1
     sdelay(400);
     data2=data & 0xf0;						//取高4位数据
     data3=(data & 0x0f)<<4;					//取低4位数据
     send_data(data1);
     send_data(data2);
     send_data(data3);
     GPIOPinWrite(LCD_PORT , CS , 0x00);     //cs=0
     sdelay(400);	
}
//============================================================================================    
//*** 函    数:write_comm()
//*** 功    能: 写命令
//*** 参    数: data   RW=0,RS=0时传送的命今指令
//============================================================================================
void write_comm(unsigned char data)				//低八位是待写入数据
{
     unsigned char data1=0xf8,data2,data3;   //data2中存放高4位数据，data3中存放低4位数据

     GPIOPinWrite(LCD_PORT , CS , 0xff);     //cs=1
     sdelay(400);
     data2=data & 0xf0;						//取高4位数据
     data3=(data & 0x0f)<<4;					//取低4位数据
     send_data(data1);
     send_data(data2);
     send_data(data3);
     GPIOPinWrite(LCD_PORT , CS , 0x00);     //cs=0
     sdelay(400);	
}
  
//============================================================================================    
//*** 函    数:init_lcd()
//*** 功    能: LCM初始化
//*** 参    数: 无
//============================================================================================
void init_lcd(void)
{	
      SysCtlPeriEnable(SYSCTL_PERIPH_GPIOF);          //  使能GPIOF端口
      GPIOPinTypeOut(LCD_PORT , CS);           //  设置PF1为输出类型
      GPIOPinTypeOut(LCD_PORT , SDAT);         //  设置PF2为输出类型
      GPIOPinTypeOut(LCD_PORT , SCLK);         //  设置PF3为输出类型


      GPIOPinWrite(LCD_PORT , CS , 0x00);      //CS=0
      GPIOPinWrite(LCD_PORT , SDAT , 0x00);    //SDAT=0
      GPIOPinWrite(LCD_PORT , SCLK , 0x00);    //SCLK=0


      write_comm(0x30);			//基本指令集

      write_comm(0x01);			//清除显示屏幕，把DDRAM位址计数器调整为"00H"
      sdelay(40000);
      write_comm(0x03);			//把DDRAM位址计数器调整为"00H"，游标回原点，该功能不影响显示DDRAM
      sdelay(40000);
      write_comm(0x06);			//光标右移
      sdelay(4000);
      write_comm(0x0c);			//显示屏打开
      sdelay(4000);

}
//============================================================================================    
//*** 函    数:screen()
//*** 功    能: 全屏显示
//*** 参    数: data   基本指令集下要显示一维数组
//============================================================================================
void screen(unsigned char data[])
{
      unsigned char i,j;

      for (i=0;i<4;i++)
      {
          switch(i)
          {
              case 0:
                  write_comm(0x80);    //设定DDRAM第一行，第一列
                  break;
              case 1:   
                  write_comm(0x90);    //设定DDRAM第二行，第一列
                  break;
              case 2: 
                  write_comm(0x88);    //设定DDRAM第三行，第一列
                  break;
              case 3:   
                  write_comm(0x98);    //设定DDRAM第四行，第一列
                  break;
          }
          for(j=0;j<16;j++) 
          write_data(data[i*16+j]); 
      }		       
}	

//============================================================================================    
//*** 函    数:disp_line(unsigned char line,unsigned char data[])
//*** 功    能: 写一行数据
//*** 参    数: line   行（0~3）
//*** 参    数: data   基本指令集下要显示一维数组(16字节）
//============================================================================================
void disp_line(unsigned char line,unsigned char data[])
{
     unsigned char j;

     switch(line)
     {
             case 0:
	         write_comm(0x80);    //设定DDRAM第一行，第一列
		 break;
	     case 1:   
		 write_comm(0x90);    //设定DDRAM第二行，第一列
	         break;
	     case 2: 
		 write_comm(0x88);    //设定DDRAM第三行，第一列
	         break;
	     case 3:   
	         write_comm(0x98);    //设定DDRAM第四行，第一列
	         break;
     }
     for(j=0;j<16;j++) 
     write_data(data[j]);
     write_comm(0xa0);
}	


//============================================================================================    
//*** 函    数:GUI_ClearSCR()
//*** 功    能: 清屏（扩充指令集下）
//*** 参    数: mode:  模式
//                   0:基本指令集
//                   1:扩充指令集
//============================================================================================
void GUI_ClearSCR(char mode)
{
    unsigned char x,y;
    unsigned char i,j;
    switch(mode)
    {
        case 0: 
                write_comm(0x32);  //基本指令集
                write_comm(0x01);  //清除屏幕
                break;
        case 1:
                write_comm(0x36);  //扩充指令集
                for(i=0;i<8;i++)   //填写缓冲区域
                {
                    for(j=0;j<64;j++)
                    gui_disp_buf[i][j]=0x00;
                }  
                x=0x80;y=0x80; 
                write_comm(y);             //设置坐标 
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
                write_comm(0x32);//基本指令集，绘图显示
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
//*** 函    数:location(int y,int x)
//*** 功    能:汉字定位
//*** 参    数:
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


