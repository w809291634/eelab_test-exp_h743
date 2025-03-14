#include "gt911.h"
#include "iic4.h"
#include "delay.h"
#include "stdio.h"

GT911_point_t GT911Point[GT911_TOUCH_NUM] = {{0}};      // 触摸点位信息存储
unsigned char GT911_pointNum = 0, touchFlag = 0;
void (*GT911_irq_cb)(void);                             //应用层的回调函数

/*********************************************************************************************
* 名称：GT911_gpio_init
* 功能：触摸引脚初始化
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
static void GT911_gpio_init(void)
{
  GPIO_InitTypeDef gpio_init_struct;

  GT911_RST_GPIO_CLK_ENABLE();                        /* RST引脚时钟使能 */
  GT911_EXTI_GPIO_CLK_ENABLE();                       /* EXTI引脚时钟使能 */

  gpio_init_struct.Pin = GT911_RST_GPIO_PIN;
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;         /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLUP;                 /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  /* 快速 */
  HAL_GPIO_Init(GT911_RST_GPIO_PORT, &gpio_init_struct); /* SCL */

  gpio_init_struct.Pin = GT911_EXTI_GPIO_PIN;
  HAL_GPIO_Init(GT911_EXTI_GPIO_PORT, &gpio_init_struct); /* SDA */
}

/*********************************************************************************************
* 名称：GT911_exti_init
* 功能：触摸芯片中断初始化
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
static void GT911_exti_init(void)
{
  GPIO_InitTypeDef gpio_init_struct;                          /* GPIO配置参数存储变量 */
  // GPIO 初始化
  GT911_EXTI_GPIO_CLK_ENABLE(); 
  
  gpio_init_struct.Pin = GT911_EXTI_GPIO_PIN;                 /* 引脚 */
  gpio_init_struct.Mode = GPIO_MODE_IT_RISING;                /* 上升沿触发 */
  gpio_init_struct.Pull = GPIO_NOPULL;                        /* 没有上下拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
  HAL_GPIO_Init(GT911_EXTI_GPIO_PORT, &gpio_init_struct);     /* 初始化引脚 */
  
  // EXTI 初始化
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;                               //使能SYSCFG时钟
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);                       /* 抢占0，子优先级0 */
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);                               /* 使能中断线2 */
}

/*********************************************************************************************
* 名称：EXTI2_IRQHandler
* 功能：外部中断服务函数
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void EXTI2_IRQHandler(void)
{
  touchFlag = 1;
  if(GT911_irq_cb != NULL) GT911_irq_cb();                      //有应用层则进行回调
  __HAL_GPIO_EXTI_CLEAR_IT(GT911_EXTI_GPIO_PIN);                /* 退出时再清一次中断，避免按键抖动误触发 */
}

/*********************************************************************************************
* 名称：GT911_writeReg
* 功能：写寄存器
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
static int GT911_writeReg(unsigned short reg, unsigned char *data, unsigned char len)
{
  unsigned char ret = 0;
  I2C4_Start();
  I2C4_WriteByte(GT911_CMD_WR);
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg >> 8);       // Register_H
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg & 0xFF);     // Register_L
  if(I2C4_WaitAck())return -1;
  for(unsigned char i=0; i<len; i++)
  {
    I2C4_WriteByte(data[i]); 
    if(I2C4_WaitAck())return -1;
    ret++;  
  }
  I2C4_Stop();
  return ret;
}

/*********************************************************************************************
* 名称：GT911_readReg
* 功能：读寄存器
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
static int GT911_readReg(unsigned short reg, unsigned char *data, unsigned char len)
{
  /* 设定读寄存器首地址 */
  I2C4_Start();	
  I2C4_WriteByte(GT911_CMD_WR);  
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg >> 8);       // Register_H
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg & 0xFF);     // Register_L
  if(I2C4_WaitAck())return -1;
//  I2C4_Stop();

  /* 读取数据 */
  I2C4_Start(); 
  I2C4_WriteByte(GT911_CMD_RD); 
  if(I2C4_WaitAck())return -1;
  for(unsigned char i=0; i<len; i++){
    data[i] = I2C4_ReadByte(i == (len - 1) ? 0 : 1);
  } 
  I2C4_Stop();
  return len;
}

/*********************************************************************************************
* 名称：GT911_cfg
* 功能：配置函数
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
int GT911_cfg(void)
{
  unsigned char cfgBuf[14] = {0};
  GT911_writeReg(GT911_CTRL_REG, cfgBuf, 1);    // 写入配置
  GT911_readReg(GT911_PID_REG, cfgBuf, 14);     // 读取PID
  if(cfgBuf[0] == 0x39 && cfgBuf[1] == 0x31 && cfgBuf[2] == 0x31)
    return 0;
  return -1;
}

/*********************************************************************************************
* 名称：GT911_scan
* 功能：GT911触控扫描函数
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void GT911_scan(void)
{
  if(touchFlag == 0) return ;
  
  unsigned char pointBuf[GT911_TOUCH_NUM * 8] = {0};    // 存储点位信息
  unsigned char info = 0;
  
  GT911_readReg(GT911_GSTID_REG, &info, 1);
  GT911_pointNum = info & 0x0F;                         // 获取点数量
  if(GT911_pointNum > GT911_TOUCH_NUM)
    GT911_pointNum = GT911_TOUCH_NUM;
  
  /* 有触摸点 */
  if(info > 0x80)
  {
    GT911_readReg(GT911_GSTID_REG + 1, pointBuf, GT911_pointNum * 8);
    for(unsigned char i=0; i<GT911_pointNum; i++){
      GT911Point[i].id = pointBuf[i*8];
      GT911Point[i].x = 854 - (pointBuf[4+(i*8)]<<8 | pointBuf[3+(i*8)]);
      GT911Point[i].y = (pointBuf[2+(i*8)]<<8 | pointBuf[1+(i*8)]);
      GT911Point[i].size = (pointBuf[6+(i*8)]<<8 | pointBuf[5+(i*8)]);
    }
  }
  /* 没有触摸点时为最大值 */
  else if(info == 0x80)
  {
    for(unsigned char i=0; i<GT911_TOUCH_NUM; i++){
      GT911Point[i].x = 0xFFFF;
      GT911Point[i].y = 0xFFFF;
      GT911Point[i].size = 0;
    }
    GT911_pointNum = 0;
  }
  
  info = 0;
  GT911_writeReg(GT911_GSTID_REG, &info, 1);
  touchFlag = 0;
}

/*********************************************************************************************
* 名称：GT911_init
* 功能：GT911触控初始化函数
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
int GT911_init(void)
{
  GT911_gpio_init();
  I2C4_IO_Init();
  delay_ms(10);     // 等待
  
  // 设定地址为 0x28/0x29
  GT911_RST_L;
  GT911_EXTI_H;
  delay_ms(1);      // 大于10us
  GT911_RST_H;
  delay_ms(6);      // 大于5ms
  
  GT911_exti_init();// EXTI 转为输入态
  delay_ms(50);     // 延时一段时间再发送配置信息
  return GT911_cfg();
}
