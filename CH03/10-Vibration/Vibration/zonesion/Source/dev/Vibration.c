/*********************************************************************************************
* 文件：Vibration.c
* 作者：Lixm 2017.10.17
* 说明：震动驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "Vibration.h"

#define VIBRATION_GPIO_PORT                  GPIOA
#define VIBRATION_GPIO_PIN                   GPIO_PIN_0
#define VIBRATION_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
/*********************************************************************************************
* 名称：vibration_init()
* 功能：震动传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vibration_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  VIBRATION_GPIO_CLK_ENABLE();                            //开启震动相关的GPIO外设时钟

  gpio_init_struct.Pin = VIBRATION_GPIO_PIN;              //选择要控制的GPIO引脚
  gpio_init_struct.Mode = GPIO_MODE_INPUT;                //设置引脚模式为输入模式
  gpio_init_struct.Pull = GPIO_PULLUP;                    //设置引脚为上拉模式
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        //设置引脚速率
  HAL_GPIO_Init(VIBRATION_GPIO_PORT, &gpio_init_struct);  //初始化GPIO配置
}

/*********************************************************************************************
* 名称：unsigned char get_vibration_status(void)
* 功能：获取震动传感器状态
* 参数：无
* 返回：1 表示有震动 
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_vibration_status(void)                     
{  
  if(HAL_GPIO_ReadPin(VIBRATION_GPIO_PORT,VIBRATION_GPIO_PIN)) //获取震动传感器的状态
    return 0;
  else
    return 1;
}