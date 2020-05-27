#include  "buzzer.h"
#include  <hw_types.h>
#include  <hw_memmap.h>
#include  <sysctl.h>
#include  <gpio.h>
#include  <timer.h>


#define  PART_LM3S1138
#include  <pin_map.h>


#define  SysCtlPeriEnable       SysCtlPeripheralEnable
#define  GPIOPinTypeOut         GPIOPinTypeGPIOOutput


//  声明全局的系统时钟变量
extern unsigned long TheSysClock;


//  蜂鸣器初始化
void buzzerInit(void)
{
    SysCtlPeriEnable(SYSCTL_PERIPH_TIMER1);                 //  使能TIMER1模块
    SysCtlPeriEnable(CCP3_PERIPH);                          //  使能CCP3所在的GPIO端口
    GPIOPinTypeTimer(CCP3_PORT, CCP3_PIN);                  //  设置相关管脚为Timer功能

    TimerConfigure(TIMER1_BASE, TIMER_CFG_16_BIT_PAIR |     //  配置TimerB为16位PWM
                                TIMER_CFG_B_PWM);
}


//  蜂鸣器发出指定频率的声音
//  usFreq是发声频率，取值 (系统时钟/65536)+1 ～ 20000，单位：Hz
void buzzerSound(unsigned short usFreq)
{
    unsigned long ulVal;

    if ((usFreq <= TheSysClock / 65536UL) || (usFreq > 20000))
    {
        buzzerQuiet();
    }
    else
    {
        GPIOPinTypeTimer(CCP3_PORT, CCP3_PIN);              //  设置相关管脚为Timer功能
        ulVal = TheSysClock / usFreq;
        TimerLoadSet(TIMER1_BASE, TIMER_B, ulVal);          //  设置TimerB初值
        TimerMatchSet(TIMER1_BASE, TIMER_B, ulVal / 2);     //  设置TimerB匹配值
        TimerEnable(TIMER1_BASE, TIMER_B);                  //  使能TimerB计数
    }
}


//  蜂鸣器停止发声
void buzzerQuiet(void)
{
    TimerDisable(TIMER1_BASE, TIMER_B);                     //  禁止TimerB计数
    GPIOPinTypeOut(CCP3_PORT, CCP3_PIN);                    //  配置CCP3管脚为GPIO输出
    GPIOPinWrite(CCP3_PORT, CCP3_PIN, 0x00);                //  使CCP3管脚输出低电平
}

