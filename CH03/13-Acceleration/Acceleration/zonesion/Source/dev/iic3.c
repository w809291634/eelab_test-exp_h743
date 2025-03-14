#include "iic3.h"
#include "delay.h"

/*********************************************************************************************
* 名称:I2C_GPIOInit()
* 功能:初始化I2C
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C3_GPIOInit(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  I2C_SCL_CLK_ENABLE();                                   /* 时钟使能 */
  I2C_SDA_CLK_ENABLE();                                   /* 时钟使能 */
  
  gpio_init_struct.Pin = I2C_SCL_PIN;                     /* 引脚 */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     /* 高速 */
  HAL_GPIO_Init(I2C_SCL_GPIO, &gpio_init_struct);         /* 初始化引脚 */

  gpio_init_struct.Pin = I2C_SDA_PIN ;
  HAL_GPIO_Init(I2C_SDA_GPIO, &gpio_init_struct);         /* 初始化引脚 */
  
  SDA_H;
  SCL_H;
}
/*********************************************************************************************
* 名称:I2C_Start()
* 功能:I2C初始信号
* 参数:无
* 返回:0
* 修改:
* 注释:
*********************************************************************************************/
void I2C3_Start(void)
{
    SDA_OUT;
    SDA_H;
    SCL_H;
    IIC3_DelayUs(2);
    SDA_L;
    IIC3_DelayUs(2);
    SCL_L;
}

/*********************************************************************************************
* 名称:I2C_Stop()
* 功能:I2C停止信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C3_Stop(void)
{
    SDA_OUT;
    SCL_L;
    SDA_L;
    IIC3_DelayUs(2);
    SCL_H;
    SDA_H;
    IIC3_DelayUs(2);
}

/*********************************************************************************************
* 名称:I2C_Ack()
* 功能:I2C应答信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C3_Ack(void)
{
    SCL_L;
    SDA_OUT;
    SDA_L;
    IIC3_DelayUs(2);
    SCL_H;
    IIC3_DelayUs(2);
    SCL_L;
}

/*********************************************************************************************
* 名称:I2C_NoAck()
* 功能:I2C非应答信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C3_NoAck(void)
{
    SCL_L;
    SDA_OUT;
    SDA_H;
    IIC3_DelayUs(2);
    SCL_H;
    IIC3_DelayUs(2);
    SCL_L;
}

/*********************************************************************************************
* 名称:I2C_WaitAck()
* 功能:I2C等待应答信号
* 参数:无
* 返回:1，接收应答失败  0，接收应答成功
* 修改:
* 注释:
*********************************************************************************************/
int I2C3_WaitAck(void)
{
    uint8_t ucErrTime=0;
    SDA_H;
    SDA_IN;
    IIC3_DelayUs(2);
    SCL_H;
    IIC3_DelayUs(2);
    while (SDA_R)
    {
      ucErrTime++;
      if(ucErrTime > 250)
      {
        I2C3_Stop();
        return 1;
      }
    }
    SCL_L;
    return 0;
}

/*********************************************************************************************
* 名称:I2C_WriteByte()
* 功能:I2C写字节数据
* 参数:char SendByte -- 发送数据
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C3_WriteByte(uint8_t SendByte) //数据从高位到低位//
{
    uint8_t i=8;
    SDA_OUT;
    while(i--)
    {
        if(SendByte&0x80) SDA_H;
        else SDA_L;
        SendByte<<=1;
        IIC3_DelayUs(1);
        SCL_H;
        IIC3_DelayUs(2);
        SCL_L;
        IIC3_DelayUs(1);
    }
    SDA_H;
}

/*********************************************************************************************
* 名称:I2C_ReadByte()
* 功能:I2C读字节数据，数据从高位到低位
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
uint8_t I2C3_ReadByte(void)
{
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;
    SDA_IN;
    while(i--)
    {
        ReceiveByte<<=1;
        SCL_L;
        IIC3_DelayUs(2);
        SCL_H;
        IIC3_DelayUs(2);
        if(SDA_R)
        {
            ReceiveByte|=0x01;
        }
        //SCL_L;
        //IIC_DelayUs(1);
    }
    SCL_L;
    return ReceiveByte;
}

