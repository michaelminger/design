/*************************************************************************************************
    SoftI2C.C
    模拟I2C总线程序（主模式，通用）
*************************************************************************************************/
#include  "SoftI2C.H"
#include  "SoftI2C_CFG.H"

unsigned char I2C_temp[8]={"01234567"};                               //时间交换区,全局变量声明/

/*************************************************************************************************
功能：I2C总线初始化，使总线处于空闲状态
参数：无
返回：无
说明：在main()函数的开始处，应当执行一次本函数
*************************************************************************************************/
void  I2C_Init(void)
{
    I2C_PinEnableSCL();
    I2C_PinEnableSDA();

    I2C_PinTypeOutSCL();
    I2C_PinTypeOutSDA();

    I2C_PinWriteSCL(1);     I2C_TimeDelay();
    I2C_PinWriteSDA(1);     I2C_TimeDelay();
}


/*************************************************************************************************
功能：产生I2C总线的起始条件
参数：无
返回：无
说明：SCL处于高电平期间，当SDA出现下降沿时启动I2C总线
      本函数也用来产生重复起始条件
*************************************************************************************************/
void  I2C_Start(void)
{
    I2C_PinWriteSDA(1);     I2C_TimeDelay();
    I2C_PinWriteSCL(1);     I2C_TimeDelay();
    I2C_PinWriteSDA(0);     I2C_TimeDelay();
    I2C_PinWriteSCL(0);     I2C_TimeDelay();
}


/*************************************************************************************************
功能：向I2C总线写1个字节的数据
参数：dat是要写到总线上的数据
返回：无
*************************************************************************************************/
void  I2C_Write(unsigned char  dat)
{
    unsigned int  t  =  8;

    do
    {
        if ( (dat & 0x80)  ==  0x00 )
        {
            I2C_PinWriteSDA(0);
        }
        else
        {
            I2C_PinWriteSDA(1);
        }

        dat  <<=  1;

        I2C_PinWriteSCL(1);     I2C_TimeDelay();
        I2C_PinWriteSCL(0);     I2C_TimeDelay();
    } while ( --t != 0 );
}


/*************************************************************************************************
功能：从从机读取1个字节的数据
参数：无
返回：读取的1个字节数据
*************************************************************************************************/
unsigned char  I2C_Read(void)
{
    unsigned char  ucData;
    unsigned int   t  =  8;

    I2C_PinTypeInSDA();

    do
    {
        I2C_PinWriteSCL(1);     I2C_TimeDelay();
        ucData <<= 1;
        if ( I2C_PinReadSDA() ) ucData++;
        I2C_PinWriteSCL(0);     I2C_TimeDelay();
    } while ( --t != 0 );

    I2C_PinTypeOutSDA();

    return(ucData);
}


/*************************************************************************************************
功能：读取从机应答位（应答或非应答），用于判断：从机是否成功接收主机数据
参数：无
返回：0－从机应答
      1－从机非应答
说明：从机在收到每一个字节后都要产生应答位，主机如果收到非应答则应当终止传输
*************************************************************************************************/
bool  I2C_GetAck(void)
{
    bool  bAck;

    I2C_PinWriteSDA(1);     I2C_TimeDelay();
    I2C_PinWriteSCL(1);     I2C_TimeDelay();

    I2C_PinTypeInSDA();
    bAck = I2C_PinReadSDA();
    I2C_PinTypeOutSDA();

    I2C_PinWriteSCL(0);     I2C_TimeDelay();

    return(bAck);
}


/*************************************************************************************************
功能：主机产生应答位（应答或非应答），用于通知从机：主机是否成功接收从机数据
参数：Ack = 0：主机应答
      Ack = 1：主机非应答
返回：无
说明：主机在收到每一个字节后都要产生应答，在收到最后一个字节时，应当产生非应答
*************************************************************************************************/
void I2C_PutAck(bool  bAck)
{
    I2C_PinWriteSDA(bAck);      I2C_TimeDelay();
    I2C_PinWriteSCL(1);         I2C_TimeDelay();
    I2C_PinWriteSCL(0);         I2C_TimeDelay();
}


/*************************************************************************************************
功能：产生I2C总线的停止条件
参数：无
返回：无
说明：SCL处于高电平期间，当SDA出现上升沿时停止I2C总线
*************************************************************************************************/
void  I2C_Stop(void)
{
    I2C_PinWriteSDA(0);     I2C_TimeDelay();
    I2C_PinWriteSCL(1);     I2C_TimeDelay();
    I2C_PinWriteSDA(1);     I2C_TimeDelay();
}


/*************************************************************************************************
功能：启动I2C总线收发数据
参数：ucMode    操作模式，决定I2C总线的收发格式，取下列值之一：
                I2C_MODE_RECV     接收模式
                I2C_MODE_SEND     发送模式
                I2C_MODE_SrRECV   带重复起始条件的接收模式
      ucSLA     从机地址
      ucAddr    子地址（在从机内部，寄存器的编址或数据的存储地址）
      pucBuf    发送或接收的数据缓冲区
      uiSize    发送或接收的数据长度（以字节计）
返回：0－正常
      1－异常（无应答）
*************************************************************************************************/
int  I2C_On(unsigned char   ucMode,
            unsigned char   ucSLA,
            unsigned char   ucAddr,
            unsigned char  *pucBuf,
            unsigned int    uiSize)
{
    //  启动I2C总线
    I2C_Start();

    if ( ucMode  !=  I2C_MODE_RECV )                        //  如果不是接收模式
    {
        //  发送SLA+W
        I2C_Write(ucSLA & 0xFE);
        if ( I2C_GetAck() )
        {
            I2C_Stop();
            return(1);
        }

        //  发送子地址
        I2C_Write(ucAddr);
        if ( I2C_GetAck() )
        {
            I2C_Stop();
            return(1);
        }

        if ( ucMode  ==  I2C_MODE_SEND )                    //  如果是发送模式
        {
            //  发送数据
            do
            {
                I2C_Write(*pucBuf++);

                if ( I2C_GetAck() )
                {
                    I2C_Stop();
                    return(1);
                }
            } while ( --uiSize != 0 );

            //  发送完毕，正常结束
            I2C_Stop();
            return(0);
        }
        else
        {
            I2C_Start();                                    //  发送重复起始条件
        }
    }

    //  发送SLA+R
    I2C_Write(ucSLA | 0x01);
    if ( I2C_GetAck() )
    {
        I2C_Stop();
        return(1);
    }

    //  接收数据
    for (;;)
    {
        *pucBuf++ = I2C_Read();

        if ( --uiSize == 0 )
        {
            I2C_PutAck(1);                                  //  接收完最后一个数据时发送NACK
            break;
        }

        I2C_PutAck(0);
    }

    //  接收完毕，正常结束
    I2C_Stop();
    return(0);
}

