#include "FDC2214.h"
#include  "systemInit.h"
#include  "SoftI2C.H"
#include  "uartGetPut.h"
#include <stdio.h>
#define PI 3.14159265358979323846
#define ClockFrequency   36000000 //ʱ��Ƶ��

unsigned char CHx_FIN_SEL[4];
double fREFx[4]; 


/*!
 *  @brief      IICдFDC2214
 *  @param      Slve_Addr     ������ַ
 *  @param      reg           �Ĵ���
 *  @param      data          ����
 *  @since      v1.0
 *  Sample usage:       FDC2214_Write16(FDC2214_Addr, FDC2214_CONFIG, 0xFFFF);
 */
void FDC2214_Write16(unsigned char Slve_Addr, unsigned char reg, unsigned short int data)
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
 *  @brief      IIC��FDC2214
 *  @param      Slve_Addr     ������ַ
 *  @param      reg           �Ĵ���
 *  @return     �Ĵ���ֵ
 *  @since      v1.0
 *  Sample usage:       FDC2214_Read16(FDC2214_Addr, FDC2214_CONFIG);
 */
unsigned short int FDC2214_Read16(unsigned char Slve_Addr, unsigned char reg)
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
 *  @brief      ��ȡFDC2214һ��ͨ����ת��ֵ
 *  @param      channel     ͨ��
 *  @param      *data       ��ȡ������
 *  @return     0��ʧ�ܣ�1���ɹ�
 *  @since      v1.0
 *  Sample usage:       FDC2214_GetChannelData(FDC2214_Channel_0, &CH0_DATA);
 */
unsigned char FDC2214_GetChannelData(FDC2214_channel_t channel, unsigned int *data)
{
     short int timeout = 100;
    unsigned char dateReg_H;
    unsigned char dateReg_L;
    unsigned char bitUnreadConv;
    unsigned short int status;
    unsigned int dataRead;

    switch(channel)
    {
        case FDC2214_Channel_0:
            dateReg_H = FDC2214_DATA_CH0;
            dateReg_L = FDC2214_DATA_LSB_CH0;
            bitUnreadConv = 0x0008;
        break;

        case FDC2214_Channel_1:
            dateReg_H = FDC2214_DATA_CH1;
            dateReg_L = FDC2214_DATA_LSB_CH1;
            bitUnreadConv = 0x0004;
        break;

        case FDC2214_Channel_2:
            dateReg_H = FDC2214_DATA_CH2;
            dateReg_L = FDC2214_DATA_LSB_CH2;
            bitUnreadConv = 0x0002;
        break;

        case FDC2214_Channel_3:
            dateReg_H = FDC2214_DATA_CH3;
            dateReg_L = FDC2214_DATA_LSB_CH3;
            bitUnreadConv = 0x0001;
        break;
    }

    status = FDC2214_Read16(FDC2214_Addr, FDC2214_STATUS);//��ȡ״̬�Ĵ�����ֵ
    while(timeout && !(status & bitUnreadConv))//����Ƿ���δ������
    {
        status = FDC2214_Read16(FDC2214_Addr, FDC2214_STATUS);
        timeout--;
    }
    if(timeout)
    {
        dataRead = (unsigned int)(FDC2214_Read16(FDC2214_Addr, dateReg_H) << 16);
        dataRead |= FDC2214_Read16(FDC2214_Addr, dateReg_L);
        *data = dataRead;
        return 1;
    }
    else
    {
        //��ʱ
       
        return 0;
    }
}

/*!
 *  @brief      ����FDC2214һ��ͨ����ת��ʱ�䣬ת��ʱ��(tCx)=(CHx_RCOUNT?16) / fREFx��
 *  @param      channel     ͨ��
 *  @param      rcount      CHx_RCOUNT
 *  @since      v1.0
 *  Sample usage:       FDC2214_SetRcount(FDC2214_Channel_0, 0x0FFF);
 */
void FDC2214_SetRcount(FDC2214_channel_t channel, unsigned short int rcount)
{
    if(rcount <= 0x00FF) return;

    switch(channel)
    {
        case FDC2214_Channel_0:
            FDC2214_Write16(FDC2214_Addr, FDC2214_RCOUNT_CH0, rcount);
        break;

        case FDC2214_Channel_1:
            FDC2214_Write16(FDC2214_Addr, FDC2214_RCOUNT_CH1, rcount);
        break;

        case FDC2214_Channel_2:
            FDC2214_Write16(FDC2214_Addr, FDC2214_RCOUNT_CH2, rcount);
        break;

        case FDC2214_Channel_3:
            FDC2214_Write16(FDC2214_Addr, FDC2214_RCOUNT_CH3, rcount);
        break;
    }
}

/*!
 *  @brief      ����FDC2214һ��ͨ���Ľ���ʱ�䣬ʹLC������������ͨ��x��ʼת��֮ǰ�ȶ�����������ʱ��(tSx) = (CHx_SETTLECOUNT?16) �� fREFx��
                CHx_SETTLECOUNT = 0x00, 0x01ʱ(tSx) = 32 �� fREFx��
 *  @param      channel     ͨ��
 *  @param      count       CHx_SETTLECOUNT
 *  @since      v1.0
 *  @note       CHx_SETTLECOUNT > Vpk �� fREFx �� C �� ��^2 / (32 �� IDRIVEX)
 *  Sample usage:       FDC2214_SetSettleCount(FDC2214_Channel_0, 0x00FF);
 */
void FDC2214_SetSettleCount(FDC2214_channel_t channel, unsigned short int count)
{
    switch(channel)
    {
        case FDC2214_Channel_0:
            FDC2214_Write16(FDC2214_Addr, FDC2214_SETTLECOUNT_CH0, count);
        break;

        case FDC2214_Channel_1:
            FDC2214_Write16(FDC2214_Addr, FDC2214_SETTLECOUNT_CH1, count);
        break;

        case FDC2214_Channel_2:
            FDC2214_Write16(FDC2214_Addr, FDC2214_SETTLECOUNT_CH2, count);
        break;

        case FDC2214_Channel_3:
            FDC2214_Write16(FDC2214_Addr, FDC2214_SETTLECOUNT_CH3, count);
        break;
    }
}

/*!
 *  @brief      ����FDC2214һ��ͨ���ĵ�ʱ��Ƶ��,
                ��ִ��������ã�
                0x01 -- ������Ƶ��0.01MHz��8.75MHz
                0x02 -- ������Ƶ��5MHz��10MHz
                ���˴��������ã�
                0x01 -- ������Ƶ��0.01MHz��10MHz
 *  @param      channel               ͨ��
 *  @param      frequency_select      ʱ��ѡ��
 *  @param      divider               ��Ƶ����fREFx = fCLK / CHx_FREF_DIVIDER��
 *  @note       fREFx is > 4 �� fSENSOR(�ⲿLC����Ƶ��)
 *  @since      v1.0
 *  Sample usage:       FDC2214_SetChannelClock(FDC2214_Channel_0, 0x01, 0xFF);
 */
void FDC2214_SetChannelClock(FDC2214_channel_t channel, unsigned char frequency_select, unsigned short int divider)
{
    unsigned short int temp = 0;

    temp = (unsigned short int)(frequency_select << 12) | (divider & 0x03FF);

    CHx_FIN_SEL[channel] = frequency_select;//��¼ʱ��ѡ��
    fREFx[channel] = (double)ClockFrequency / divider;//��¼ʱ��Ƶ��

    switch(channel)
    {
        case FDC2214_Channel_0:
            FDC2214_Write16(FDC2214_Addr, FDC2214_CLOCK_DIVIDERS_CH0, temp);
        break;

        case FDC2214_Channel_1:
            FDC2214_Write16(FDC2214_Addr, FDC2214_CLOCK_DIVIDERS_CH1, temp);
        break;

        case FDC2214_Channel_2:
            FDC2214_Write16(FDC2214_Addr, FDC2214_CLOCK_DIVIDERS_CH2, temp);
        break;

        case FDC2214_Channel_3:
            FDC2214_Write16(FDC2214_Addr, FDC2214_CLOCK_DIVIDERS_CH3, temp);
        break;
    }
}

/*!
 *  @brief      ����INTB�����жϹ���
 *  @param      mode      1�����жϣ�0�����ж�
 *  @since      v1.0
 *  Sample usage:       FDC2214_SetDriveCurrent(FDC2214_Channel_0, FDC2214_Drive_Current_0_025);
 */
void FDC2214_SetINTB(unsigned char mode)
{
    unsigned short int temp = 0;

    temp = FDC2214_Read16(FDC2214_Addr, FDC2214_CONFIG);
    temp &= 0xFF7F;
    if(mode)
        temp |= 0x0080;
    FDC2214_Write16(FDC2214_Addr, FDC2214_CONFIG, temp);

    temp = FDC2214_Read16(FDC2214_Addr, FDC2214_ERROR_CONFIG);
    temp &= 0xFFFE;
    if(mode)
        temp |= 0x0001;
    FDC2214_Write16(FDC2214_Addr, FDC2214_ERROR_CONFIG, temp);
}

/*!
 *  @brief      ����FDC2214��һ��ͨ������ͨ��ģʽ
 *  @param      channel     ͨ��
 *  @since      v1.0
 *  Sample usage:       FDC2214_SetActiveChannel(FDC2214_Channel_0);
 */
void FDC2214_SetActiveChannel(FDC2214_channel_t channel)
{
    unsigned short int temp = 0;
    temp = FDC2214_Read16(FDC2214_Addr, FDC2214_CONFIG);
    temp &= 0x3FFF;
    temp |= (unsigned short int)(channel << 14);

    FDC2214_Write16(FDC2214_Addr, FDC2214_CONFIG, temp);
}

/*!
 *  @brief      ����FDC2214�Ĺ���ģʽ��������˯�ߡ�
 *  @param      mode     0������������1��˯����ģʽѡ����
 *  @since      v1.0
 *  Sample usage:       FDC2214_SetSleepMode(0);
 */
void FDC2214_SetSleepMode(unsigned char mode)
{
    unsigned short int temp = 0;
    temp = FDC2214_Read16(FDC2214_Addr, FDC2214_CONFIG);
    temp &= 0xDFFF;
    if(mode)temp |= 0x2000;

    FDC2214_Write16(FDC2214_Addr, FDC2214_CONFIG, temp);
}

/*!
 *  @brief      FDC2214����ģʽѡ��
 *  @param      mode     0��ȫ��������ģʽ��1���͹��ʼ���ģʽ
 *  @since      v1.0
 *  Sample usage:       FDC2214_SetCurrentMode(1);
 */
void FDC2214_SetCurrentMode(unsigned char mode)
{
    unsigned short int temp = 0;
    temp = FDC2214_Read16(FDC2214_Addr, FDC2214_CONFIG);
    temp &= 0xF7FF;
    if(mode)
        temp |= 0x800;

    FDC2214_Write16(FDC2214_Addr, FDC2214_CONFIG, temp);
}

/*!
 *  @brief      ����FDC2214ʱ��Դ
 *  @param      src     0���ڲ�ʱ��Դ��1���ⲿʱ��Դ
 *  @since      v1.0
 *  Sample usage:       FDC2214_SetClockSource(1);
 */
void FDC2214_SetClockSource(unsigned char src)
{
    unsigned short int temp = 0;
    temp = FDC2214_Read16(FDC2214_Addr, FDC2214_CONFIG);
    temp &= 0xFDFF;
    if(src)
        temp |= 0x200;

    FDC2214_Write16(FDC2214_Addr, FDC2214_CONFIG, temp);
}

/*!
 *  @brief      �ߵ���������������ֻ�����ڵ�ͨ��ģʽ��
 *  @param      mode     0�����������1.5mA����1���ߵ���������������>1.5mA��
 *  @since      v1.0
 *  Sample usage:       FDC2214_SetHighCurrentMode(0);
 */
void FDC2214_SetHighCurrentMode(unsigned char mode)
{
    unsigned short int temp = 0;
    temp = FDC2214_Read16(FDC2214_Addr, FDC2214_CONFIG);
    temp &= 0xFFBF;
    if(mode)
        temp |= 0x40;

    FDC2214_Write16(FDC2214_Addr, FDC2214_CONFIG, temp);
}

/*!
 *  @brief      ����FDC2214��MUX CONFIG�Ĵ���
 *  @param      autoscan      �Զ�ɨ��ģʽ 0���رգ�1����
 *  @param      channels      �Զ�ɨ��ͨ��
 *  @param      bandwidth     �����˲������������ⲿLC����Ƶ��
 *  @since      v1.0
 *  Sample usage:       FDC2214_SetMUX_CONFIG(1, FDC2214_Channel_Sequence_0_1_2, FDC2214_Bandwidth_10M);
 */
void FDC2214_SetMUX_CONFIG(unsigned char autoscan, FDC2214_channel_sequence_t channels, FDC2214_filter_bandwidth_t bandwidth)
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

    FDC2214_Write16(FDC2214_Addr, FDC2214_MUX_CONFIG, temp);
}

/*!
 *  @brief      ��������λ
 *  @since      v1.0
 *  Sample usage:       FDC2214_Reset();
 */
void FDC2214_Reset(void)
{
    FDC2214_Write16(FDC2214_Addr, FDC2214_RESET_DEV, 0x8000);
}

/*!
 *  @brief      ����FDC2214��һ��ͨ������������������ȷ���񵴷�����1.2V��1.8V֮�䡣
 *  @param      channel      ͨ��
 *  @param      current      ������С
 *  @since      v1.0
 *  Sample usage:       FDC2214_SetDriveCurrent(FDC2214_Channel_0, FDC2214_Drive_Current_0_025);
 */
void FDC2214_SetDriveCurrent(FDC2214_channel_t channel, FDC2214_drive_current_t current)
{
    unsigned short int temp = 0;
    temp = (unsigned short int)(current << 11);

    switch(channel)
    {
        case FDC2214_Channel_0:
            FDC2214_Write16(FDC2214_Addr, FDC2214_DRIVE_CURRENT_CH0, temp);
        break;

        case FDC2214_Channel_1:
            FDC2214_Write16(FDC2214_Addr, FDC2214_DRIVE_CURRENT_CH1, temp);
        break;

        case FDC2214_Channel_2:
            FDC2214_Write16(FDC2214_Addr, FDC2214_DRIVE_CURRENT_CH2, temp);
        break;

        case FDC2214_Channel_3:
            FDC2214_Write16(FDC2214_Addr, FDC2214_DRIVE_CURRENT_CH3, temp);
        break;
    }
}

/*!
 *  @brief      ����Ƶ��
 *  @param      channel      ͨ��
 *  @param      datax        ��ȡ��ת��ֵ
 *  @return     Ƶ�ʣ���λHz
 *  @since      v1.0
 *  Sample usage:       FDC2214_CalculateFrequency(FDC2214_Channel_0, 0xFF);
 */
double FDC2214_CalculateFrequency(FDC2214_channel_t channel, unsigned int datax)
{
    double frequency = 0.0;

    frequency = (double)(CHx_FIN_SEL[channel] * fREFx[channel] * datax) / 268435456.0;//2^28

    return frequency;
}

/*!
 *  @brief      �����ⲿ���ݴ�С
 *  @param      frequency      Ƶ�ʴ�С����λHz
 *  @param      inductance     ���ص�д�С����λuH
 *  @param      capacitance    ���ص��ݴ�С����λpF
 *  @return     �ⲿ���ݴ�С����λpF
 *  @since      v1.0
 *  Sample usage:       FDC2214_CalculateCapacitance(FDC2214_Channel_0, 6000000, 18, 33);
 */
double FDC2214_CalculateCapacitance(double frequency, float inductance, float capacitance)
{
    double cap = 0.0;

    cap = (double)(1.0 / (inductance * (2 * PI * frequency) * (2 * PI * frequency)));

    return cap;
}

/*!
 *  @brief      FDC2214��ʼ������
 *  @return     1���ɹ���0��ʧ��
 *  @since      v1.0
 *  Sample usage:       FDC2214_Init();
 */
unsigned char FDC2214_Init(void)
{
    unsigned short int deviceID = 0;
    deviceID = FDC2214_Read16(FDC2214_Addr, FDC2214_DEVICE_ID);//������ID
    if(deviceID == FDC2214_ID)
    {
       // printf("\r\nFDC2214 ID:%d",deviceID);
        FDC2214_Reset();
        
    
        FDC2214_SetRcount(FDC2214_Channel_0, 5000);//2000us ת������
        /*FDC2214_SetRcount(FDC2214_Channel_1, 5000);//2000us ת������
        FDC2214_SetRcount(FDC2214_Channel_2, 5000);//2000us ת������
        FDC2214_SetRcount(FDC2214_Channel_3, 5000);//2000us ת������*/
        FDC2214_SetSettleCount(FDC2214_Channel_0, 200);//200 cycles �ȴ�ʱ��
        /*FDC2214_SetSettleCount(FDC2214_Channel_1, 200);//200 cycles �ȴ�ʱ��
        FDC2214_SetSettleCount(FDC2214_Channel_2, 200);//200 cycles �ȴ�ʱ��
        FDC2214_SetSettleCount(FDC2214_Channel_3, 200);//200 cycles �ȴ�ʱ��*/
        FDC2214_SetChannelClock(FDC2214_Channel_0, 2, 1);//����ʱ�ӷ�Ƶ��1��Ƶ��40MHz
        /*FDC2214_SetChannelClock(FDC2214_Channel_1, 2, 1);//����ʱ�ӷ�Ƶ��1��Ƶ��40MHz
        FDC2214_SetChannelClock(FDC2214_Channel_2, 2, 1);//����ʱ�ӷ�Ƶ��1��Ƶ��40MHz
        FDC2214_SetChannelClock(FDC2214_Channel_3, 2, 1);//����ʱ�ӷ�Ƶ��1��Ƶ��40MHz*/
        
        FDC2214_SetINTB(0);//�ر�INTB�ж�
        //FDC2214_SetActiveChannel(FDC2214_Channel_0);//����ͨ��0
        FDC2214_SetCurrentMode(0);//ȫ��������ģʽ
        FDC2214_SetClockSource(0);//�ⲿʱ��Դ
        FDC2214_SetHighCurrentMode(0);//������������������
        
        FDC2214_SetMUX_CONFIG(1, FDC2214_Channel_Sequence_0_1, FDC2214_Bandwidth_10M);//���Զ�ɨ�裬10MHz����
        
        FDC2214_SetDriveCurrent(FDC2214_Channel_0, FDC2214_Drive_Current_0_081);//ͨ��0��������0.081mA
        /*FDC2214_SetDriveCurrent(FDC2214_Channel_1, FDC2214_Drive_Current_0_081);//ͨ��1��������0.081mA
        FDC2214_SetDriveCurrent(FDC2214_Channel_2, FDC2214_Drive_Current_0_081);//ͨ��2��������0.081mA
        FDC2214_SetDriveCurrent(FDC2214_Channel_3, FDC2214_Drive_Current_0_081);//ͨ��3��������0.081mA*/
        
        
        FDC2214_SetSleepMode(0);//�˳�˯��״̬����ʼ����
        SysCtlDelay(100 * (TheSysClock / 3000));             //  ��ʱԼ10ms��������������
        
        return 1;
    }
    else
    {return 0;
       // printf("\r\nFDC2214 Init Failed!!");
    }

    
}



