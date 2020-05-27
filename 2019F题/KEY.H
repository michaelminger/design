#ifndef  _KEY_H_
#define  _KEY_H_


//  定义KEY名称
#define  KEY1       0x01
#define  KEY2       0x02


//  初始化KEY
extern  void  KEY_Init(unsigned char  ucKEY);


//  读取KEY
extern  unsigned char  KEY_Get(unsigned char  ucKEY);


#endif  //  _KEY_H_

