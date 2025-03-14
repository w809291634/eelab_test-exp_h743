/*********************************************************************************************
* 文件：iic.c
* 作者：zonesion
* 说明：iic驱动程序
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "iic1.h"
#include "extend.h"
#include "delay.h"
/*********************************************************************************************
* 宏定义
*********************************************************************************************/
// SCL--PF1   SDA---PF0
#define I2C_GPIO                GPIOF
#define PIN_SCL                 GPIO_PIN_1
#define PIN_SDA                 GPIO_PIN_0

#define I2C_SCL_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* 时钟使能 */
#define I2C_SDA_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* 时钟使能 */

#define SDA_R                   HAL_GPIO_ReadPin(I2C_GPIO,PIN_SDA)
/*********************************************************************************************
* 名称：iic_init()
* 功能：I2C初始化函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic_init(void)
{
  GPIO_InitTypeDef gpio_init_struct;

  I2C_SCL_GPIO_CLK_ENABLE();                            /* SCL引脚时钟使能 */
  I2C_SDA_GPIO_CLK_ENABLE();                            /* SDA引脚时钟使能 */

  gpio_init_struct.Pin = PIN_SCL;
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;          /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLUP;                  /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;   /* 快速 */
  HAL_GPIO_Init(I2C_GPIO, &gpio_init_struct);           /* SCL */

  gpio_init_struct.Pin = PIN_SDA;
  HAL_GPIO_Init(I2C_GPIO, &gpio_init_struct);           /* SDA */
}

/*********************************************************************************************
* 名称：sda_out()
* 功能：设置SDA为输出
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sda_out(void)
{
  GPIO_InitTypeDef gpio_init_struct;

  gpio_init_struct.Pin = PIN_SDA;
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;          /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLUP;                  /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;   /* 快速 */
  HAL_GPIO_Init(I2C_GPIO, &gpio_init_struct);           /* SDA */
}

/*********************************************************************************************
* 名称：sda_in()
* 功能：设置SDA为输入
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sda_in(void)
{
  GPIO_InitTypeDef gpio_init_struct;

  gpio_init_struct.Pin = PIN_SDA;
  gpio_init_struct.Mode = GPIO_MODE_INPUT;              /* 输入模式 */
  gpio_init_struct.Pull = GPIO_PULLUP;                  /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;   /* 快速 */
  HAL_GPIO_Init(I2C_GPIO, &gpio_init_struct);           /* SDA */
}

/*********************************************************************************************
* 名称：iic_start()
* 功能：I2C起始信号
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic_start(void)
{
  sda_out();
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);           //拉高数据线
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);           //拉高时钟线
  delay_us(5);                                                  //延时
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_RESET);         //拉低数据线
  delay_us(5);                                                  //延时
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);         //钳住I2C总线，准备发送或接收数据 
}

/*********************************************************************************************
* 名称：iic_stop()
* 功能：I2C停止信号
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic_stop(void)
{
  sda_out();
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_RESET);         //拉低数据线
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);           //拉高时钟线
  delay_us(5);                                                  //延时5us
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);           //拉高数据线，发送I2C总线结束信号
  delay_us(5);                                                  //延时5us
}

/*********************************************************************************************
* 名称：iic_send_ack()
* 功能：I2C发送应答信号
* 参数：ack：为0：产生ACK应答  为1：不产生ACK应答
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic_send_ack(char ack)
{
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);         //拉低时钟线
  sda_out();
  if(ack)
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);         //不产生ACK应答
  else
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_RESET);       //产生ACK应答
  delay_us(5);                                                  //延时 
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);           //拉高时钟线
  delay_us(5);                                                  //延时
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);         //拉低时钟线
}

/*********************************************************************************************
* 名称：iic_recv_ack()
* 功能：I2C接收应答
* 参数：无
* 返回：应答信号
* 修改：
* 注释：
*********************************************************************************************/
unsigned char iic_recv_ack(void)
{
  unsigned char CY = 0;
  sda_in();
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);           //拉高数据线
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);           //拉高时钟线 
  delay_us(5);                                                  //延时
  CY = SDA_R;                                                   //读应答信号
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);         //释放总线 
  return CY;
}

/*********************************************************************************************
* 名称：iic_write_byte()
* 功能：I2C写一个字节数据，返回ACK或者NACK，从高到低，依次发送
* 参数：data -- 要写的数据
* 返回：0：收到应答 1：没有收到应答
* 修改：
* 注释：
*********************************************************************************************/
unsigned char iic_write_byte(unsigned char data)
{
  unsigned char i;
  sda_out();
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);         //拉低时钟线
  for(i = 0;i < 8;i++){                                         
    if(data & 0x80)                                             //数据线操作
      HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);
    else
      HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_RESET); 
    delay_us(5);                                                //延时5us
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);         //输出SDA稳定后，拉高SCL给出上升沿，从机检测到后进行数据采样
    delay_us(5);                                                //延时5us
    data <<= 1;                                                 //数组左移一位
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);       //拉低时钟线
  } 
  /* 处理应答 */
  sda_in();
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);           //拉高数据线
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);           //拉高时钟线
  delay_us(5);                                                  //延时5us，等待从机应答
  if(SDA_R)return 1;                                            //SDA为高，收到NACK
  else{                                                         //SDA为低，收到ACK
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);       //释放总线
    return 0;
  }
}

/*********************************************************************************************
* 名称：iic_read_byte()
* 功能：I2C读一个字节数据，返回ACK或者NACK，从高到低，依次发送。
* 参数：ack：为0：产生ACK应答  为1：不产生ACK应答。应答信号为低电平时，规定为有效应答位
* 返回：data：读取的字节
* 修改：
* 注释：
*********************************************************************************************/
unsigned char iic_read_byte(unsigned char ack)
{  
  unsigned char i,data = 0;
  sda_in();                                                     // 设置数据线为输入模式
  for(i=0;i<8;i++ )
  {
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);       // 拉低时钟线，此时数据线允许变更
    delay_us(5);                                                // 等待数据就绪
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);         // 拉高时钟线，数据线此时稳定
    data<<=1;
    if(SDA_R)data++;                                            // 读取数据
    delay_us(5);                                                // 数据保持时间
  }
  iic_send_ack(ack);                                            // 发送应答信号
  return data;
}

/*********************************************************************************************
* 名称：delay()
* 功能：延时
* 参数：t -- 设置时间
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void delay(unsigned int t)                                      //延时函数
{
  unsigned char i;
  while(t--){
    for(i = 0;i < 200;i++);
  }
}