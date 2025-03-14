/*********************************************************************************************
* 文件：Relay.c
* 作者：Lixm 2017.10.17
* 说明：继电器驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "Relay.h"

/*********************************************************************************************
* 名称：relay_init()
* 功能：继电器传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void relay_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  RELAY1_GPIO_CLK_ENABLE();                                     /* 时钟使能 */
  RELAY2_GPIO_CLK_ENABLE();                                     /* 时钟使能 */
  
  gpio_init_struct.Pin = RELAY1_GPIO_PIN;                       /* 引脚 */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                  /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLDOWN;                        /* 下拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;              /* 中速 */
  HAL_GPIO_Init(RELAY1_GPIO_PORT, &gpio_init_struct);           /* 初始化引脚 */

  gpio_init_struct.Pin = RELAY2_GPIO_PIN;
  HAL_GPIO_Init(RELAY2_GPIO_PORT, &gpio_init_struct);           /* 初始化引脚 */
  
  relay_control(0x00);
}

/*********************************************************************************************
* 名称：void relay_control(unsigned char cmd)
* 功能：继电器控制驱动
* 参数：控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void relay_control(unsigned char cmd)
{ 
  if(cmd & 0x01)
    HAL_GPIO_WritePin(RELAY1_GPIO_PORT, RELAY1_GPIO_PIN, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(RELAY1_GPIO_PORT, RELAY1_GPIO_PIN, GPIO_PIN_RESET);
  if(cmd & 0x02)
    HAL_GPIO_WritePin(RELAY2_GPIO_PORT, RELAY2_GPIO_PIN, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(RELAY2_GPIO_PORT, RELAY2_GPIO_PIN, GPIO_PIN_RESET);
}