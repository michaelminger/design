#ifndef __IAR_DELAY_H
#define __IAR_DELAY_H

#include <intrinsics.h>

#define  XTAL  6  //�ɶ���Ϊ�����õľ���Ƶ�ʣ���λMhz��

#define  delay_us(x)   __delay_cycles ((unsigned long)(x * XTAL) ) 
#define  delay_ms(x)   __delay_cycles ((unsigned long)(x * XTAL*1000) )
#define  delay_s(x)   __delay_cycles ( (unsigned long)(x * XTAL*1000000) )

#endif 