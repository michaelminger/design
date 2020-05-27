#include "FDC2114.h"
#include  "systemInit.h"
#include  "SoftI2C.H"
#include  "SoftI2C_CFG.H"
#include  "uartGetPut.h"
#include <stdio.h>
#define PI 3.14159265358979323846
//#define TheSysClock   36000000 //时钟频率
unsigned char CHx_FIN_SEL[4];
double fREFx[4]; 


/*!
 *  @brief      IIC写FDC2114
 *  @param      Slve_Addr     器件地址
 *  @param      reg           寄存器
 *  @param      data          数据
 *  @since      v1.0
 *  Sample usage:       FDC2114_Write16(FDC2114_Addr, FDC2114_CONFIG, 0xFFFF);
 */
void FDC2114_Write16(unsigned char Slve_Addr, unsigned char reg, unsigned short int data)
{
    unsigned char dat;
    I2C_Start();
    
    I2C_Write(Slve_Addr << 1);
    I2C_GetAck();
    I2C_Write(reg);
    I2C_GetAck();
    
    dat=(data >> 8);
    I2C_Write(dat);
    I2C_GetAck();
    
    dat=data & 0xFF;
    I2C_Write(dat);
    I2C_GetAck();
    
    I2C_Stop();
}

/*!
 *  @brief      IIC读FDC2114
 *  @param      Slve_Addr     器件地址
 *  @param      reg           寄存器
 *  @return     寄存器值
 *  @since      v1.0
 *  Sample usage:       FDC2114_Read16(FDC2114_Addr, FDC2114_CONFIG);
 */
unsigned short int FDC2114_Read16(unsigned char Slve_Addr, unsigned char reg)
{
    unsigned short int temp=0;
    unsigned int huanchongqu[30];
    I2C_Start();
    I2C_Write(Slve_Addr << 1);
    I2C_GetAck();    
    I2C_Write(reg);
    I2C_GetAck();
    I2C_Start();
    I2C_Write((Slve_Addr << 1) | 0x01);
    I2C_GetAck();
    huanchongqu[0] = I2C_Read();
    I2C_PutAck(0);
    huanchongqu[1]= I2C_Read();
    I2C_PutAck(1);
    I2C_Stop();
    temp=(huanchongqu[0]<<8)+huanchongqu[1];
    return temp;
}

/*!
 *  @brief      读取FDC2214一个通道的转换值
 *  @param      channel     通道
 *  @param      *data       读取的数据
 *  @return     0：失败；1：成功
 *  @since      v1.0
 *  Sample usage:       FDC2114_GetChannelData(FDC2114_Channel_0, &CH0_DATA);
 */
unsigned char FDC2114_GetChannelData(FDC2114_channel_t channel, unsigned int *data)
{
    short int timeout = 100;
    unsigned char dateReg_H;
   // unsigned char dateReg_L;
    unsigned char bitUnreadConv;
    unsigned short int status;
    unsigned int dataRead;

    switch(channel)
    {
        case FDC2114_Channel_0:
            dateReg_H = FDC2114_DATA_CH0;
            //dateReg_L = FDC2114_DATA_LSB_CH0;
            bitUnreadConv = 0x0008;
        break;

        case FDC2114_Channel_1:
            dateReg_H = FDC2114_DATA_CH1;
           // dateReg_L = FDC2114_DATA_LSB_CH1;
            bitUnreadConv = 0x0004;
        break;

        case FDC2114_Channel_2:
            dateReg_H = FDC2114_DATA_CH2;
           // dateReg_L = FDC2114_DATA_LSB_CH2;
            bitUnreadConv = 0x0002;
        break;

        case FDC2114_Channel_3:
            dateReg_H = FDC2114_DATA_CH3;
           // dateReg_L = FDC2114_DATA_LSB_CH3;
            bitUnreadConv = 0x0001;
        break;
    }

    status = FDC2114_Read16(FDC2114_Addr, FDC2114_STATUS);//读取状态寄存器的值
    while(timeout && !(status & bitUnreadConv))//检查是否有未读数据
    {
        status = FDC2114_Read16(FDC2114_Addr, FDC2114_STATUS);
        timeout--;
    }
    if(timeout)
    {
        dataRead = (unsigned int)(FDC2114_Read16(FDC2114_Addr, dateReg_H) /*<< 16*/);
        //dataRead |= FDC2114_Read16(FDC2114_Addr, dateReg_L);
        *data = dataRead;
        return 1;
    }
    else
    {
        //超时
       
        return 0;
    }
}

/*!
 *  @brief      设置FDC2114一个通道的转换时间，转换时间(tCx)=(CHx_RCOUNT?16) / fREFx。
 *  @param      channel     通道
 *  @param      rcount      CHx_RCOUNT
 *  @since      v1.0
 *  Sample usage:       FDC2114_SetRcount(FDC2114_Channel_0, 0x0FFF);
 */
void FDC2114_SetRcount(FDC2114_channel_t channel, unsigned short int rcount)
{
    if(rcount <= 0x00FF) return;

    switch(channel)
    {
        case FDC2114_Channel_0:
            FDC2114_Write16(FDC2114_Addr, FDC2114_RCOUNT_CH0, rcount);
        break;

        case FDC2114_Channel_1:
            FDC2114_Write16(FDC2114_Addr, FDC2114_RCOUNT_CH1, rcount);
        break;

        case FDC2114_Channel_2:
            FDC2114_Write16(FDC2114_Addr, FDC2114_RCOUNT_CH2, rcount);
        break;

        case FDC2114_Channel_3:
            FDC2114_Write16(FDC2114_Addr, FDC2114_RCOUNT_CH3, rcount);
        break;
    }
}

/*!
 *  @brief      设置FDC2114一个通道的建立时间，使LC传感器可以在通道x开始转换之前稳定下来，建立时间(tSx) = (CHx_SETTLECOUNT?16) ÷ fREFx。
                CHx_SETTLECOUNT = 0x00, 0x01时(tSx) = 32 ÷ fREFx。
 *  @param      channel     通道
 *  @param      count       CHx_SETTLECOUNT
 *  @since      v1.0
 *  @note       CHx_SETTLECOUNT > Vpk × fREFx × C × π^2 / (32 × IDRIVEX)
 *  Sample usage:       FDC2114_SetSettleCount(FDC2114_Channel_0, 0x00FF);
 */
void FDC2114_SetSettleCount(FDC2114_channel_t channel, unsigned short int count)
{
    switch(channel)
    {
        case FDC2114_Channel_0:
            FDC2114_Write16(FDC2114_Addr, FDC2114_SETTLECOUNT_CH0, count);
        break;

        case FDC2114_Channel_1:
            FDC2114_Write16(FDC2114_Addr, FDC2114_SETTLECOUNT_CH1, count);
        break;

        case FDC2114_Channel_2:
            FDC2114_Write16(FDC2114_Addr, FDC2114_SETTLECOUNT_CH2, count);
        break;

        case FDC2114_Channel_3:
            FDC2114_Write16(FDC2114_Addr, FDC2114_SETTLECOUNT_CH3, count);
        break;
    }
}

/*!
 *  @brief      设置FDC2214一个通道的的时钟频率,
                差分传感器配置：
                0x01 -- 传感器频率0.01MHz和8.75MHz
                0x02 -- 传感器频率5MHz和10MHz
                单端传感器配置：
                0x01 -- 传感器频率0.01MHz和10MHz
 *  @param      channel               通道
 *  @param      frequency_select      时钟选择
 *  @param      divider               分频器，fREFx = fCLK / CHx_FREF_DIVIDER，
 *  @note       fREFx is > 4 × fSENSOR(外部LC振荡器频率)
 *  @since      v1.0
 *  Sample usage:       FDC2114_SetChannelClock(FDC2114_Channel_0, 0x01, 0xFF);
 */
void FDC2114_SetChannelClock(FDC2114_channel_t channel, unsigned char frequency_select, unsigned short int divider)
{
    unsigned short int temp = 0;

    temp = (unsigned short int)(frequency_select << 12) | (divider & 0x03FF);

    CHx_FIN_SEL[channel] = frequency_select;//记录时钟选择
    fREFx[channel] = (double)TheSysClock / divider;//记录时钟频率

    switch(channel)
    {
        case FDC2114_Channel_0:
            FDC2114_Write16(FDC2114_Addr, FDC2114_CLOCK_DIVIDERS_CH0, temp);
        break;

        case FDC2114_Channel_1:
            FDC2114_Write16(FDC2114_Addr, FDC2114_CLOCK_DIVIDERS_CH1, temp);
        break;

        case FDC2114_Channel_2:
            FDC2114_Write16(FDC2114_Addr, FDC2114_CLOCK_DIVIDERS_CH2, temp);
        break;

        case FDC2114_Channel_3:
            FDC2114_Write16(FDC2114_Addr, FDC2114_CLOCK_DIVIDERS_CH3, temp);
        break;
    }
}

/*!
 *  @brief      配置INTB引脚中断功能
 *  @param      mode      1：开中断；0：关中断
 *  @since      v1.0
 *  Sample usage:       FDC2114_SetDriveCurrent(FDC2114_Channel_0, FDC2214_Drive_Current_0_025);
 */
void FDC2114_SetINTB(unsigned char mode)
{
    unsigned short int temp = 0;

    temp = FDC2114_Read16(FDC2114_Addr, FDC2114_CONFIG);
    temp &= 0xFF7F;
    if(mode)
        temp |= 0x0080;
    FDC2114_Write16(FDC2114_Addr, FDC2114_CONFIG, temp);

    temp = FDC2114_Read16(FDC2114_Addr, FDC2114_ERROR_CONFIG);
    temp &= 0xFFFE;
    if(mode)
        temp |= 0x0001;
    FDC2114_Write16(FDC2114_Addr, FDC2114_ERROR_CONFIG, temp);
}

/*!
 *  @brief      激活FDC2214的一个通道，单通道模式
 *  @param      channel     通道
 *  @since      v1.0
 *  Sample usage:       FDC2114_SetActiveChannel(FDC2114_Channel_0);
 */
void FDC2114_SetActiveChannel(FDC2114_channel_t channel)
{
    unsigned short int temp = 0;
    temp = FDC2114_Read16(FDC2114_Addr, FDC2114_CONFIG);
    temp &= 0x3FFF;
    temp |= (unsigned short int)(channel << 14);

    FDC2114_Write16(FDC2114_Addr, FDC2114_CONFIG, temp);
}

/*!
 *  @brief      设置FDC2214的工作模式：正常、睡眠。
 *  @param      mode     0：正常工作；1：睡激活模式选择。眠
 *  @since      v1.0
 *  Sample usage:       FDC2114_SetSleepMode(0);
 */
void FDC2114_SetSleepMode(unsigned char mode)
{
    unsigned short int temp = 0;
    temp = FDC2114_Read16(FDC2114_Addr, FDC2114_CONFIG);
    temp &= 0xDFFF;
    if(mode)temp |= 0x2000;

    FDC2114_Write16(FDC2114_Addr, FDC2114_CONFIG, temp);
}

/*!
 *  @brief      FDC2114激活模式选择。
 *  @param      mode     0：全电流激活模式；1：低功率激活模式
 *  @since      v1.0
 *  Sample usage:       FDC2114_SetCurrentMode(1);
 */
void FDC2114_SetCurrentMode(unsigned char mode)
{
    unsigned short int temp = 0;
    temp = FDC2114_Read16(FDC2114_Addr, FDC2114_CONFIG);
    temp &= 0xF7FF;
    if(mode)
        temp |= 0x800;

    FDC2114_Write16(FDC2114_Addr, FDC2114_CONFIG, temp);
}

/*!
 *  @brief      设置FDC2214时钟源
 *  @param      src     0：内部时钟源；1：外部时钟源
 *  @since      v1.0
 *  Sample usage:       FDC2114_SetClockSource(1);
 */
void FDC2114_SetClockSource(unsigned char src)
{
    unsigned short int temp = 0;
    temp = FDC2114_Read16(FDC2114_Addr, FDC2114_CONFIG);
    temp &= 0xFDFF;
    if(src)
        temp |= 0x200;

    FDC2114_Write16(FDC2114_Addr, FDC2114_CONFIG, temp);
}

/*!
 *  @brief      高电流传感器驱动，只适用于单通道模式。
 *  @param      mode     0：正常（最大1.5mA）；1：高电流传感器驱动（>1.5mA）
 *  @since      v1.0
 *  Sample usage:       FDC2114_SetHighCurrentMode(0);
 */
void FDC2114_SetHighCurrentMode(unsigned char mode)
{
    unsigned short int temp = 0;
    temp = FDC2114_Read16(FDC2114_Addr, FDC2114_CONFIG);
    temp &= 0xFFBF;
    if(mode)
        temp |= 0x40;

    FDC2114_Write16(FDC2114_Addr, FDC2114_CONFIG, temp);
}

/*!
 *  @brief      设置FDC2214的MUX CONFIG寄存器
 *  @param      autoscan      自动扫描模式 0：关闭，1：打开
 *  @param      channels      自动扫描通道
 *  @param      bandwidth     数字滤波器带宽，大于外部LC振荡器频率
 *  @since      v1.0
 *  Sample usage:       FDC2114_SetMUX_CONFIG(1, FDC2214_Channel_Sequence_0_1_2, FDC2114_Bandwidth_10M);
 */
void FDC2114_SetMUX_CONFIG(unsigned char autoscan, FDC2114_channel_sequence_t channels, FDC2114_filter_bandwidth_t bandwidth)
{
    unsigned short int temp = 0;

    if(autoscan)
    {
        temp = (unsigned short int)(autoscan << 15) | (channels << 13) | 0x0208 | bandwidth;
    }
    else
    {
        temp = 0x0208 | bandwidth;
    }

    FDC2114_Write16(FDC2114_Addr, FDC2114_MUX_CONFIG, temp);
}

/*!
 *  @brief      传感器复位
 *  @since      v1.0
 *  Sample usage:       FDC2114_Reset();
 */
void FDC2114_Reset(void)
{
    FDC2114_Write16(FDC2114_Addr, FDC2114_RESET_DEV, 0x8000);
}

/*!
 *  @brief      设置FDC2214的一个通道传感器驱动电流，确保振荡幅度在1.2V和1.8V之间。
 *  @param      channel      通道
 *  @param      current      电流大小
 *  @since      v1.0
 *  Sample usage:       FDC2114_SetDriveCurrent(FDC2114_Channel_0, FDC2214_Drive_Current_0_025);
 */
void FDC2114_SetDriveCurrent(FDC2114_channel_t channel, FDC2114_drive_current_t current)
{
    unsigned short int temp = 0;
    temp = (unsigned short int)(current << 11);

    switch(channel)
    {
        case FDC2114_Channel_0:
            FDC2114_Write16(FDC2114_Addr, FDC2114_DRIVE_CURRENT_CH0, temp);
        break;

        case FDC2114_Channel_1:
            FDC2114_Write16(FDC2114_Addr, FDC2114_DRIVE_CURRENT_CH1, temp);
        break;

        case FDC2114_Channel_2:
            FDC2114_Write16(FDC2114_Addr, FDC2114_DRIVE_CURRENT_CH2, temp);
        break;

        case FDC2114_Channel_3:
            FDC2114_Write16(FDC2114_Addr, FDC2114_DRIVE_CURRENT_CH3, temp);
        break;
    }
}

/*!
 *  @brief      计算频率
 *  @param      channel      通道
 *  @param      datax        读取的转换值
 *  @return     频率，单位Hz
 *  @since      v1.0
 *  Sample usage:       FDC2114_CalculateFrequency(FDC2114_Channel_0, 0xFF);
 */
double FDC2114_CalculateFrequency(FDC2114_channel_t channel, unsigned int datax)
{
    double frequency = 0.0;

    frequency = (double)(CHx_FIN_SEL[channel] * fREFx[channel] * datax) / 268435456.0;//2^28

    return frequency;
}

/*!
 *  @brief      计算外部电容大小
 *  @param      frequency      频率大小，单位Hz
 *  @param      inductance     板载电感大小，单位uH
 *  @param      capacitance    板载电容大小，单位pF
 *  @return     外部电容大小，单位pF
 *  @since      v1.0
 *  Sample usage:       FDC2114_CalculateCapacitance(FDC2114_Channel_0, 6000000, 18, 33);
 */
double FDC2114_CalculateCapacitance(double frequency, float inductance, float capacitance)
{
    double cap = 0.0;

    cap = (double)(1.0 / (inductance * (2 * PI * frequency) * (2 * PI * frequency)));

    return cap;
}

/*!
 *  @brief      FDC2214初始化函数
 *  @return     1：成功；0：失败
 *  @since      v1.0
 *  Sample usage:       FDC2114_Init();
 */
unsigned char FDC2114_Init(void)
{   char a[16];
    unsigned short int deviceID = 0;
    deviceID = FDC2114_Read16(FDC2114_Addr, FDC2114_DEVICE_ID);//读器件ID
    //sprintf(a,"地址是：%d\r\n",deviceID);
     // uartPuts(a);
    if(deviceID == FDC2114_ID)
    {
       // printf("\r\nFDC2214 ID:%d",deviceID);
        FDC2114_Reset();
        
    
        FDC2114_SetRcount(FDC2114_Channel_0, 5000);//2000us 转换周期
       /*FDC2114_SetRcount(FDC2114_Channel_1, 5000);//2000us 转换周期
         FDC2114_SetRcount(FDC2114_Channel_2, 5000);//2000us 转换周期
        FDC2114_SetRcount(FDC2114_Channel_3, 5000);//2000us 转换周期*/
        FDC2114_SetSettleCount(FDC2114_Channel_0, 200);//200 cycles 等待时间
        FDC2114_SetSettleCount(FDC2114_Channel_1, 200);//200 cycles 等待时间
       /* FDC2114_SetSettleCount(FDC2114_Channel_2, 200);//200 cycles 等待时间
        FDC2114_SetSettleCount(FDC2114_Channel_3, 200);//200 cycles 等待时间*/
        FDC2114_SetChannelClock(FDC2114_Channel_0, 2, 1);//设置时钟分频，1分频，40MHz
       /* FDC2114_SetChannelClock(FDC2114_Channel_1, 2, 1);//设置时钟分频，1分频，40MHz
        FDC2114_SetChannelClock(FDC2114_Channel_2, 2, 1);//设置时钟分频，1分频，40MHz
        FDC2114_SetChannelClock(FDC2114_Channel_3, 2, 1);//设置时钟分频，1分频，40MHz*/
        
        FDC2114_SetINTB(0);//关闭INTB中断
       // FDC2114_SetActiveChannel(FDC2114_Channel_0);//开启通道0
        FDC2114_SetCurrentMode(0);//全电流激活模式
        FDC2114_SetClockSource(0);//外部时钟源
        FDC2114_SetHighCurrentMode(0);//正常电流传感器驱动
        
        FDC2114_SetMUX_CONFIG(1, FDC2114_Channel_Sequence_0_1_2_3, FDC2114_Bandwidth_10M);//打开自动扫描，10MHz带宽
        
        FDC2114_SetDriveCurrent(FDC2114_Channel_0, FDC2114_Drive_Current_0_081);//通道0驱动电流0.081mA
        /*FDC2114_SetDriveCurrent(FDC2114_Channel_1, FDC2114_Drive_Current_0_081);//通道1驱动电流0.081mA
        FDC2114_SetDriveCurrent(FDC2114_Channel_2, FDC2114_Drive_Current_0_081);//通道2驱动电流0.081mA
        FDC2114_SetDriveCurrent(FDC2114_Channel_3, FDC2114_Drive_Current_0_081);//通道3驱动电流0.081mA*/
        
        
        FDC2114_SetSleepMode(0);//退出睡眠状态，开始工作
       // delay_ms(100);
      SysCtlDelay(100 * (TheSysClock / 3000));             //  延时约10ms，消除按键抖动
        return 1;
    }
    else
    {
       // printf("\r\nFDC2114 Init Failed!!");
      return 0;
    }

    
}



