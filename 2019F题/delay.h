#ifndef __IAR_DELAY_H
#define __IAR_DELAY_H

#include <intrinsics.h>

#define  XTAL  6  //可定义为你所用的晶振频率（单位Mhz）

#define  delay_us(x)   __delay_cycles ((unsigned long)(x * XTAL) ) 
#define  delay_ms(x)   __delay_cycles ((unsigned long)(x * XTAL*1000) )
#define  delay_s(x)   __delay_cycles ( (unsigned long)(x * XTAL*1000000) )

#endif 