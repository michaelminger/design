#ifndef  __UART_GET_PUT_H__
#define  __UART_GET_PUT_H__


//  UART初始化
extern void uartInit(void);

//  通过UART发送一个字符
extern void uartPutc(const char c);

//  通过UART发送字符串
extern void uartPuts(const char *s);

//  通过UART接收一个字符
extern char uartGetc(void);

//  通过UART接收字符串，回显，退格<Backspace>修改，回车<Enter>结束
extern int uartGets(char *s, int size);


#endif  //  __UART_GET_PUT_H__

