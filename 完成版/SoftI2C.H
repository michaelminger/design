/*************************************************************************************************
    SoftI2C.H
    模拟I2C总线程序的头文件（主模式，通用）
*************************************************************************************************/


#ifndef  _SoftI2C_H_
#define  _SoftI2C_H_


//  定义I2C操作模式
#define  I2C_MODE_RECV      0                               /*  接收模式                        */
#define  I2C_MODE_SEND      1                               /*  发送模式                        */
#define  I2C_MODE_SrRECV    2                               /*  带重复起始条件的接收模式        */


extern unsigned char I2C_temp[8];

//  I2C总线初始化
extern  void  I2C_Init(void);


//  启动I2C总线收发数据
extern  int  I2C_On(unsigned char   ucMode,                 //  操作模式
                    unsigned char   ucSLA,                  //  从机地址
                    unsigned char   ucAddr,                 //  子地址
                    unsigned char  *pucBuf,                 //  数据缓冲区
                    unsigned int    uiSize);                //  数据长度


#endif    //  _SoftI2C_H_

