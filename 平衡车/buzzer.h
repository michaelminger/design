#ifndef  __BUZZER_H__
#define  __BUZZER_H__


//  蜂鸣器初始化
extern void buzzerInit(void);


//  蜂鸣器发出指定频率的声音
extern void buzzerSound(unsigned short usFreq);


//  蜂鸣器停止发声
extern void buzzerQuiet(void);


#endif  //  __BUZZER_H__

