#include "iic4.h"

void I2C4_IO_Init(void)
{
  GPIO_InitTypeDef gpio_init_struct;

  I2C4_SCL_GPIO_CLK_ENABLE();                           /* SCL引脚时钟使能 */
  I2C4_SDA_GPIO_CLK_ENABLE();                           /* SDA引脚时钟使能 */

  gpio_init_struct.Pin = I2C4_SCL_GPIO_PIN;
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;         /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLUP;                 /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  /* 快速 */
  HAL_GPIO_Init(I2C4_SCL_GPIO_PORT, &gpio_init_struct); /* SCL */

  gpio_init_struct.Pin = I2C4_SDA_GPIO_PIN;
  HAL_GPIO_Init(I2C4_SDA_GPIO_PORT, &gpio_init_struct); /* SDA */
  
  I2C4_SDA_OUT;
  I2C4_SCL_H;
  I2C4_SDA_H;
}

/*********************************************************************************************
* 名称：I2C4_Start
* 功能：启动信号
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void I2C4_Start(void)
{
  I2C4_SDA_OUT;
  I2C4_SCL_H;
  I2C4_SDA_H;
  I2C4_Delay(1);
  I2C4_SDA_L;
  I2C4_Delay(1);
  I2C4_SCL_L;
}

/*********************************************************************************************
* 名称：I2C4_Stop
* 功能：停止信号
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void I2C4_Stop(void)
{
  I2C4_SDA_OUT;
  I2C4_SCL_L;
  I2C4_SDA_L;
  I2C4_Delay(1);
  I2C4_SCL_H;
  I2C4_Delay(1);
  I2C4_SDA_H;
}

/*********************************************************************************************
* 名称：I2C4_WaitAck
* 功能：等待应答信号到来
* 参数：无
* 返回：1，接收应答失败
*           0，接收应答成功
* 修改：无
* 注释：无
*********************************************************************************************/
unsigned char I2C4_WaitAck(void)
{
  unsigned char timeCount = 0;
  I2C4_SDA_IN;
  I2C4_SDA_H;
  I2C4_SCL_H;
  I2C4_Delay(1);
  while(I2C4_SDA_R)
  {
    timeCount++;
    I2C4_Delay(1);
    if(timeCount > 250)
    {
      I2C4_Stop();
      I2C4_SCL_L;
      return 1;
    }
  }
  I2C4_SCL_L;
  return 0;
}

/*********************************************************************************************
* 名称：I2C4_Ack
* 功能：主站发送应答
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void I2C4_Ack(void)
{
  I2C4_SDA_OUT;
  I2C4_SDA_L;
  I2C4_Delay(1);
  I2C4_SCL_H;
  I2C4_Delay(1);
  I2C4_SCL_L;
}

/*********************************************************************************************
* 名称：I2C4_NAck
* 功能：主站不发送应答
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void I2C4_NAck(void)
{
  I2C4_SDA_OUT;
  I2C4_SDA_H;
  I2C4_Delay(1);
  I2C4_SCL_H;
  I2C4_Delay(1);
  I2C4_SCL_L;
}

/*********************************************************************************************
* 名称：I2C4_WriteByte
* 功能：写一个字节
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void I2C4_WriteByte(unsigned char data)
{
  I2C4_SDA_OUT;
  I2C4_SCL_L;
  for(unsigned char i=0; i<8; i++)
  {
    if(data & 0x80) I2C4_SDA_H;                                   // 数据线操作 
    else I2C4_SDA_L;
    I2C4_Delay(1);                                                // 低周期为保持时间最小0.9us
    I2C4_SCL_H;	                                                  // 拉高SCL时钟线,给出上升沿,从机检测到后进行数据采样
    I2C4_Delay(1);                                                // 高周期为保持时间最小0.8us
    data <<= 1;                                                   // 数组左移一位
    I2C4_SCL_L;                                                   // 拉低SCL时钟线 
  }
}

/*********************************************************************************************
* 名称：I2C4_ReadByte
* 功能：读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
unsigned char I2C4_ReadByte(unsigned char ack)
{
  unsigned char data = 0;
  I2C4_SDA_IN;
  for(unsigned char i=0; i<8; i++)
  {
    I2C4_SCL_L;                                                 // 拉低时钟线，此时数据线允许变更
    I2C4_Delay(1);                                              // 低周期为保持时间最小0.9us
    I2C4_SCL_H;                                                 // 拉高时钟线，数据线此时稳定
    data <<= 1;
    if(I2C4_SDA_R) data ++;                                     // 读取数据
    I2C4_Delay(1);                                              // 高周期为保持时间最小0.8us
  }
  I2C4_SCL_L;
  I2C4_Delay(1);
  if(ack)
    I2C4_Ack();
  else
    I2C4_NAck();
  return data;
}
