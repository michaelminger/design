#include  "uartGetPut.h"
#include  <hw_types.h>
#include  <hw_memmap.h>
#include  <sysctl.h>
#include  <gpio.h>
#include  <uart.h>
#include  <ctype.h>


#define  SysCtlPeriEnable       SysCtlPeripheralEnable


//  UART初始化
void uartInit(void)
{
    SysCtlPeriEnable(SYSCTL_PERIPH_UART2);                  //  使能UART模块
    SysCtlPeriEnable(SYSCTL_PERIPH_GPIOG);                  //  使能RX/TX所在的GPIO端口

    GPIOPinTypeUART(GPIO_PORTG_BASE,                        //  配置RX/TX所在管脚为
                    GPIO_PIN_0 | GPIO_PIN_1);               //  UART收发功能

    UARTConfigSet(UART2_BASE,                               //  配置UART端口
                  9600,                                     //  波特率：9600
                  UART_CONFIG_WLEN_8 |                      //  数据位：8
                  UART_CONFIG_STOP_ONE |                    //  停止位：1
                  UART_CONFIG_PAR_NONE);                    //  校验位：无

    UARTEnable(UART2_BASE);                                 //  使能UART端口
}


//  通过UART发送一个字符
void uartPutc(const char c)
{
    UARTCharPut(UART2_BASE, c);
}


//  通过UART发送字符串
void uartPuts(const char *s)
{
    while (*s != '\0') uartPutc(*(s++));
}


//  通过UART接收一个字符
char uartGetc(void)
{
    return(UARTCharGet(UART2_BASE));
}


//  功能：通过UART接收字符串，回显，退格<Backspace>修改，回车<Enter>结束
//  参数：*s保存接收数据的缓冲区，只接收可打印字符（ASCII码32～127）
//        size是缓冲区*s的总长度，要求size >= 2（包括末尾'\0'，建议用sizeof()来获取）
//  返回：接收到的有效字符数目
int uartGets(char *s, int size)
{
    char c;
    int n = 0;

    *s = '\0';
    if (size < 2) return(0);
    size--;

    for (;;)
    {
        c = uartGetc();                                     //  接收1个字符
        uartPutc(c);                                        //  回显输入的字符

        if (c == '\b')                                      //  遇退格<Backspace>修改
        {
            if (n > 0)
            {
                *(--s) = '\0';
                n--;
                uartPuts(" \b");                            //  显示空格和退格<Backspace>
            }
        }

        if (c == '\r')                                      //  遇回车<Enter>结束
        {
            uartPuts("\r\n");                               //  显示回车换行<CR><LF>
            break;
        }

        if (n < size)                                       //  如果小于长度限制
        {
            if (isprint(c))                                 //  如果接收到的是可打印字符
            {
                *(s++) = c;                                 //  保存接收到的字符到缓冲区
                *s = '\0';
                n++;
            }
        }
    }

    return(n);                                              //  返回接收到的有效字符数目
}

