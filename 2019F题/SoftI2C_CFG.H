/*************************************************************************************************
    SoftI2C_CFG.H
    模拟I2C总线配置头文件
*************************************************************************************************/


#ifndef  _SoftI2C_CFG_H
#define  _SoftI2C_CFG_H

#include  "systemInit.h"
//  定义布尔类型
typedef  unsigned char   bool;


//  I2C总线延时
extern  void  I2C_TimeDelay(void);



//  I2C总线时钟信号SCL管脚操作函数
extern  void  I2C_PinEnableSCL(void);                       //  使能SCL管脚
extern  void  I2C_PinTypeInSCL(void);                       //  配置SCL管脚的类型为输入
extern  void  I2C_PinTypeOutSCL(void);                      //  配置SCL管脚的类型为输出
extern  void  I2C_PinWriteSCL(bool  bLevel);                //  写SCL管脚
extern  bool  I2C_PinReadSCL(void);                         //  读SCL管脚


//  I2C总线数据信号SDA管脚操作函数
extern  void  I2C_PinEnableSDA(void);                       //  使能SDA管脚
extern  void  I2C_PinTypeInSDA(void);                       //  配置SDA管脚的类型为输入
extern  void  I2C_PinTypeOutSDA(void);                      //  配置SDA管脚的类型为输出
extern  void  I2C_PinWriteSDA(bool  bLevel);                //  写SDA管脚
extern  bool  I2C_PinReadSDA(void);                         //  读SDA管脚


#endif  //  _SoftI2C_CFG_H_

