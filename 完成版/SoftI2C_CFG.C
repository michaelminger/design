/*************************************************************************************************
    SoftI2C_CFG.C
    模拟I2C总线配置C文件（Luminary Micro的Stellaris系列ARM）
*************************************************************************************************/


#include  "SoftI2C_CFG.H"


//  包含必要的头文件
#include  <hw_types.h>
#include  <hw_memmap.h>
#include  <hw_sysctl.h>
#include  <sysctl.h>
#include  <gpio.h>


//  将较长的标识符定义成较短的形式
#define  SysCtlPeriEnable       SysCtlPeripheralEnable
#define  SysCtlPeriDisable      SysCtlPeripheralDisable
#define  GPIOPinTypeIn          GPIOPinTypeGPIOInput
#define  GPIOPinTypeOutOD       GPIOPinTypeGPIOOutputOD


/*************************************************************************************************
功能：I2C总线延时
参数：无
返回：无
说明：请根据具体情况调整延时值
*************************************************************************************************/
void  I2C_TimeDelay(void)
{
    unsigned int  t  =  500;

    while ( --t  !=  0 );
}


/*************************************************************************************************
功能：使能SCL管脚
参数：无
返回：无
*************************************************************************************************/
void  I2C_PinEnableSCL(void)
{
    SysCtlPeriEnable(SYSCTL_PERIPH_GPIOA);
    
}


/*************************************************************************************************
功能：配置SCL管脚的类型为输入
参数：无
返回：无
*************************************************************************************************/
void  I2C_PinTypeInSCL(void)
{
    GPIOPinTypeIn(GPIO_PORTA_BASE , GPIO_PIN_6);
}


/*************************************************************************************************
功能：配置SCL管脚的类型为输出
参数：无
返回：无
*************************************************************************************************/
void  I2C_PinTypeOutSCL(void)
{
    GPIOPinTypeOutOD(GPIO_PORTA_BASE , GPIO_PIN_6);
}


/*************************************************************************************************
功能：写SCL管脚
参数：bLevel为电平状态，取值0或1
返回：无
*************************************************************************************************/
void  I2C_PinWriteSCL(bool  bLevel)
{
    unsigned char  ucPins  =  0x00;

    if ( bLevel )
    {
        ucPins  =  (1 << 6);
    }

    GPIOPinWrite(GPIO_PORTA_BASE , GPIO_PIN_6 , ucPins);
}


/*************************************************************************************************
功能：读SCL管脚
参数：无
返回：管脚SCL的电平状态，0或1
*************************************************************************************************/
bool  I2C_PinReadSCL(void)
{
    bool  bLevel;

    bLevel  =  GPIOPinRead(GPIO_PORTA_BASE , GPIO_PIN_6);

    return(bLevel);
}


/*************************************************************************************************
功能：使能SDA管脚
参数：无
返回：无
*************************************************************************************************/
void  I2C_PinEnableSDA(void)
{
    SysCtlPeriEnable(SYSCTL_PERIPH_GPIOA);
}


/*************************************************************************************************
功能：配置SDA管脚的类型为输入
参数：无
返回：无
*************************************************************************************************/
void  I2C_PinTypeInSDA(void)
{
    GPIOPinTypeIn(GPIO_PORTA_BASE , GPIO_PIN_7);
}


/*************************************************************************************************
功能：配置SDA管脚的类型为输出
参数：无
返回：无
*************************************************************************************************/
void  I2C_PinTypeOutSDA(void)
{
    GPIOPinTypeOutOD(GPIO_PORTA_BASE , GPIO_PIN_7);
}


/*************************************************************************************************
功能：写SDA管脚
参数：bLevel为电平状态，取值0或1
返回：无
*************************************************************************************************/
void  I2C_PinWriteSDA(bool  bLevel)
{
    unsigned char  ucPins  =  0x00;

    if ( bLevel )
    {
        ucPins  =  (1 << 7);
    }

    GPIOPinWrite(GPIO_PORTA_BASE , GPIO_PIN_7 , ucPins);
}


/*************************************************************************************************
功能：读SDA管脚
参数：无
返回：管脚SDA的电平状态，0或1
*************************************************************************************************/
bool  I2C_PinReadSDA(void)
{
    bool  bLevel;

    bLevel  =  GPIOPinRead(GPIO_PORTA_BASE , GPIO_PIN_7);

    return(bLevel);
}

