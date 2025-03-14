/*********************************************************************************************
* 文件：Hall.c
* 作者：Lixm 2017.10.17
* 说明：霍尔驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "Hall.h"

// PC5--霍尔
#define HALL_GPIO_PORT                  GPIOC
#define HALL_GPIO_PIN                   GPIO_PIN_5
#define HALL_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)

/*********************************************************************************************
* 名称：hall_init()
* 功能：霍尔传感器初始化  
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void hall_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  HALL_GPIO_CLK_ENABLE();                                 //开启GPIO外设时钟

  gpio_init_struct.Pin = HALL_GPIO_PIN;                   //选择要控制的GPIO引脚
  gpio_init_struct.Mode = GPIO_MODE_INPUT;                //设置引脚模式为输入模式
  gpio_init_struct.Pull = GPIO_PULLUP;                    //设置引脚为上拉模式
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        //设置引脚速率
  HAL_GPIO_Init(HALL_GPIO_PORT, &gpio_init_struct);       //初始化GPIO配置
}

/*********************************************************************************************
* 名称：unsigned char get_hall_status(void)
* 功能：获取霍尔传感器状态
* 参数：无
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_hall_status(void)
{ 
  if(HAL_GPIO_ReadPin(HALL_GPIO_PORT,HALL_GPIO_PIN))
    return 1;
  else
    return 0;
}