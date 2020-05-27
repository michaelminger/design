#ifndef  _ZLG7290_H_
#define  _ZLG7290_H_


extern unsigned char disp_buf[8];

extern unsigned char key_code;
extern unsigned char key_press;

extern unsigned char ZLG7290_SendData(unsigned char SubAdd,unsigned char Data);
extern void ZLG7290_SendBuf(unsigned char * disp_buf,unsigned char num);
extern unsigned char ZLG7290_SendCmd(unsigned char Data1,unsigned char Data2);

extern int ReadData_7290(int address); /*单字节*/

//  读出ZLG7290按键代码值
extern void  Read_ZLG7290Key(void);

extern void delayMS(unsigned int i);

extern unsigned char ZLG7290_SendData(unsigned char SubAdd,unsigned char Data);

extern unsigned char ZLG7290_SendCmd(unsigned char Data1,unsigned char Data2);

extern void ZLG7290_SendBuf(unsigned char * disp_buf,unsigned char num);

extern void zlg7290_test(void);

extern void zlg7290_disp(void);

extern void clear_zlg7290_disp(void);

extern void zlg7290_disp_clock(void);

extern void key_fx(void);

#endif  // _ZLG7290_H_