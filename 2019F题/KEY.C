//  包含必要的头文件
#include  "KEY.H"
#include  <hw_types.h>
#include  <hw_memmap.h>
#include  <hw_sysctl.h>
#include  <hw_gpio.h>
#include  <sysctl.h>
#include  <gpio.h>


//  将较长的标识符定义成较短的形式
#define  SysCtlPeriEnable       SysCtlPeripheralEnable
#define  SysCtlPeriDisable      SysCtlPeripheralDisable
#define  GPIOPinTypeIn          GPIOPinTypeGPIOInput


/***************************************************************************************************
功能：初始化KEY
参数：ucKEY是按键名称，取值下列值之一或者它们之间的“或运算”组合形式
            KEY1
            KEY2
返回：无
***************************************************************************************************/
void  KEY_Init(unsigned char  ucKEY)
{
    if ( ucKEY & KEY1 )
    {
        SysCtlPeriEnable(SYSCTL_PERIPH_GPIOD);                  //  使能GPIOD端口
        GPIOPinTypeIn(GPIO_PORTD_BASE , GPIO_PIN_1);            //  设置PD1为输入类型
    }

    if ( ucKEY & KEY2 )
    {
        SysCtlPeriEnable(SYSCTL_PERIPH_GPIOG);                  //  使能GPIOG端口
        GPIOPinTypeIn(GPIO_PORTG_BASE , GPIO_PIN_5);            //  设置PG5输入类型
    }
}


/***************************************************************************************************
功能：读取KEY
参数：ucKEY是按键名称，取值下列值之一或者它们之间的“或运算”组合形式
            KEY1
            KEY2
返回：KEY1和KEY2的位组合形式，有下列几种情况
            0x00    没有按键按下
            0x01    KEY1被按下
            0x02    KEY2被按下
            0x03    KEY1和KEY2都被按下
***************************************************************************************************/
unsigned char  KEY_Get(unsigned char  ucKEY)
{
    unsigned char  ucVal  =  0x00;

    if ( ucKEY & KEY1 )
    {
        if ( GPIOPinRead(GPIO_PORTD_BASE , GPIO_PIN_1)  ==  0x00 )      //  如果PD1为低电平
        {
            ucVal  |=  0x01;
        }
    }

    if ( ucKEY & KEY2 )
    {
        if ( GPIOPinRead(GPIO_PORTG_BASE , GPIO_PIN_5)  ==  0x00 )      //  如果PG5为低电平
        {
            ucVal  |=  0x02;
        }
    }

    return(ucVal);
}

