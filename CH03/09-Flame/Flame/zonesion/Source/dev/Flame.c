/*********************************************************************************************
* 文件：Flame.c
* 作者：Lixm 2017.10.17
* 说明：火焰驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "Flame.h"

#define FLAME_GPIO_PORT                  GPIOC
#define FLAME_GPIO_PIN                   GPIO_PIN_11
#define FLAME_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)
/*********************************************************************************************
* 名称：flame_init()
* 功能：火焰传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void flame_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  FLAME_GPIO_CLK_ENABLE();                                //开启火焰相关的GPIO外设时钟

  gpio_init_struct.Pin = FLAME_GPIO_PIN;                  //选择要控制的GPIO引脚
  gpio_init_struct.Mode = GPIO_MODE_INPUT;                //设置引脚模式为输入模式
  gpio_init_struct.Pull = GPIO_PULLDOWN;                  //设置引脚为下拉模式
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        //设置引脚速率
  HAL_GPIO_Init(FLAME_GPIO_PORT, &gpio_init_struct);      //初始化GPIO配置
}

/*********************************************************************************************
* 名称：unsigned char get_flame_status(void)
* 功能：获取火焰传感器状态
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_flame_status(void)
{ 
  if(HAL_GPIO_ReadPin(FLAME_GPIO_PORT,FLAME_GPIO_PIN))
    return 0;
  else
    return 1;
}