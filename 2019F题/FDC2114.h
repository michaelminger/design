#ifndef __FDC2114_H__
#define __FDC2114_H__


#include "stdio.h"


#define SWAP16(data)    (unsigned int)((((unsigned short int)(data) & (0xFF<<0 ))<<8)|(((unsigned int)(data) & (0xFF<<8))>>8))
#define FDC2114_Addr                 (0x2A)
#define FDC2114_DATA_CH0             (0x00)
#define FDC2114_DATA_LSB_CH0         (0x01)
#define FDC2114_DATA_CH1             (0x02)
#define FDC2114_DATA_LSB_CH1         (0x03)
#define FDC2114_DATA_CH2             (0x04)
#define FDC2114_DATA_LSB_CH2         (0x05)
#define FDC2114_DATA_CH3             (0x06)
#define FDC2114_DATA_LSB_CH3         (0x07)
#define FDC2114_RCOUNT_CH0           (0x08)
#define FDC2114_RCOUNT_CH1           (0x09)
#define FDC2114_RCOUNT_CH2           (0x0A)
#define FDC2114_RCOUNT_CH3           (0x0B)
#define FDC2114_OFFSET_CH0           (0x0C)
#define FDC2114_OFFSET_CH1           (0x0D)
#define FDC2114_OFFSET_CH2           (0x0E)
#define FDC2114_OFFSET_CH3           (0x0F)
#define FDC2114_SETTLECOUNT_CH0      (0x10)
#define FDC2114_SETTLECOUNT_CH1      (0x11)
#define FDC2114_SETTLECOUNT_CH2      (0x12)
#define FDC2114_SETTLECOUNT_CH3      (0x13)
#define FDC2114_CLOCK_DIVIDERS_CH0   (0x14)
#define FDC2114_CLOCK_DIVIDERS_CH1   (0x15)
#define FDC2114_CLOCK_DIVIDERS_CH2   (0x16)
#define FDC2114_CLOCK_DIVIDERS_CH3   (0x17)
#define FDC2114_STATUS               (0x18)
#define FDC2114_ERROR_CONFIG         (0x19)
#define FDC2114_CONFIG               (0x1A)
#define FDC2114_MUX_CONFIG           (0x1B)
#define FDC2114_RESET_DEV            (0x1C)
#define FDC2114_DRIVE_CURRENT_CH0    (0x1E)
#define FDC2114_DRIVE_CURRENT_CH1    (0x1F)
#define FDC2114_DRIVE_CURRENT_CH2    (0x20)
#define FDC2114_DRIVE_CURRENT_CH3    (0x21)
#define FDC2114_MANUFACTURER_ID      (0x7E)
#define FDC2114_DEVICE_ID            (0x7F)

#define FDC2114_ID                   (0x3054)


typedef enum
{
    FDC2114_Channel_0 = 0x00, 
    FDC2114_Channel_1 = 0x01, 
    FDC2114_Channel_2 = 0x02, 
    FDC2114_Channel_3 = 0x03  
}FDC2114_channel_t;


typedef enum
{
    FDC2114_Channel_Sequence_0_1      = 0x00,
    FDC2114_Channel_Sequence_0_1_2    = 0x01,
    FDC2114_Channel_Sequence_0_1_2_3  = 0x02, 
}FDC2114_channel_sequence_t;


typedef enum
{
    FDC2114_Bandwidth_1M   = 0x01, //1MHz
    FDC2114_Bandwidth_3_3M = 0x04, //3.3MHz
    FDC2114_Bandwidth_10M  = 0x05, //10MHz
    FDC2114_Bandwidth_33M  = 0x07  //33MHz
}FDC2114_filter_bandwidth_t;


typedef enum
{
    FDC2114_Drive_Current_0_016 = 0x00, //0.016mA
    FDC2114_Drive_Current_0_018 = 0x01, //0.018mA
    FDC2114_Drive_Current_0_021 = 0x02, //0.021mA
    FDC2114_Drive_Current_0_025 = 0x03, //0.025mA
    FDC2114_Drive_Current_0_028 = 0x04, //0.028mA
    FDC2114_Drive_Current_0_033 = 0x05, //0.033mA
    FDC2114_Drive_Current_0_038 = 0x06, //0.038mA
    FDC2114_Drive_Current_0_044 = 0x07, //0.044mA
    FDC2114_Drive_Current_0_052 = 0x08, //0.052mA
    FDC2114_Drive_Current_0_060 = 0x09, //0.060mA
    FDC2114_Drive_Current_0_069 = 0x0A, //0.069mA
    FDC2114_Drive_Current_0_081 = 0x0B, //0.081mA
    FDC2114_Drive_Current_0_093 = 0x0C, //0.093mA
    FDC2114_Drive_Current_0_108 = 0x0D, //0.108mA
    FDC2114_Drive_Current_0_126 = 0x0E, //0.126mA
    FDC2114_Drive_Current_0_146 = 0x0F, //0.146mA
    FDC2114_Drive_Current_0_169 = 0x10, //0.169mA
    FDC2114_Drive_Current_0_196 = 0x11, //0.196mA
    FDC2114_Drive_Current_0_228 = 0x12, //0.228mA
    FDC2114_Drive_Current_0_264 = 0x13, //0.264mA
    FDC2114_Drive_Current_0_307 = 0x14, //0.307mA
    FDC2114_Drive_Current_0_356 = 0x15, //0.356mA
    FDC2114_Drive_Current_0_413 = 0x16, //0.413mA
    FDC2114_Drive_Current_0_479 = 0x17, //0.479mA
    FDC2114_Drive_Current_0_555 = 0x18, //0.555mA
    FDC2114_Drive_Current_0_644 = 0x19, //0.644mA
    FDC2114_Drive_Current_0_747 = 0x1A, //0.747mA
    FDC2114_Drive_Current_0_867 = 0x1B, //0.867mA
    FDC2114_Drive_Current_1_006 = 0x1C, //1.006mA
    FDC2114_Drive_Current_1_167 = 0x1D, //1.167mA
    FDC2114_Drive_Current_1_354 = 0x1E, //1.354mA
    FDC2114_Drive_Current_1_571 = 0x1F  //1.571mA
}FDC2114_drive_current_t;

void FDC2114_Write16(unsigned char Slve_Addr, unsigned char reg, unsigned short int data);
void FDC2114_Write16(unsigned char Slve_Addr, unsigned char reg, unsigned short int data);
unsigned short int FDC2114_Read16(unsigned char Slve_Addr, unsigned char reg);
unsigned char FDC2114_GetChannelData(FDC2114_channel_t channel, unsigned int *data);
void FDC2114_SetRcount(FDC2114_channel_t channel, unsigned short int rcount);
void FDC2114_SetSettleCount(FDC2114_channel_t channel, unsigned short int count);
void FDC2114_SetChannelClock(FDC2114_channel_t channel, unsigned char frequency_select, unsigned short int divider);
void FDC2114_SetINTB(unsigned char mode);
void FDC2114_SetActiveChannel(FDC2114_channel_t channel);
void FDC2114_SetSleepMode(unsigned char mode);
void FDC2114_SetCurrentMode(unsigned char mode);
void FDC2114_SetClockSource(unsigned char src);
void FDC2114_SetHighCurrentMode(unsigned char mode);
void FDC2114_SetMUX_CONFIG(unsigned char autoscan, FDC2114_channel_sequence_t channels, FDC2114_filter_bandwidth_t bandwidth);
void FDC2114_Reset(void);
void FDC2114_SetDriveCurrent(FDC2114_channel_t channel, FDC2114_drive_current_t current);
double FDC2114_CalculateFrequency(FDC2114_channel_t channel, unsigned int datax);
double FDC2114_CalculateCapacitance(double frequency, float inductance, float capacitance);
unsigned char FDC2114_Init(void);



#endif
