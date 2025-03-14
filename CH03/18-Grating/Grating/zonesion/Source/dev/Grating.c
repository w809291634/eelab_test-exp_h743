/*********************************************************************************************
* 文件：Grating.c
* 作者：Lixm 2017.10.17
* 说明：红外光栅驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "Grating.h"

#define GRATING_GPIO_PORT                  GPIOA
#define GRATING_GPIO_PIN                   GPIO_PIN_1
#define GRATING_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)

/*********************************************************************************************
* 名称：grating_init()
* 功能：红外光栅传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void grating_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  GRATING_GPIO_CLK_ENABLE();                                //开启GPIO外设时钟

  gpio_init_struct.Pin = GRATING_GPIO_PIN;                  //选择要控制的GPIO引脚
  gpio_init_struct.Mode = GPIO_MODE_INPUT;                  //设置引脚模式为输入模式
  gpio_init_struct.Pull = GPIO_PULLDOWN;                    //设置引脚为下拉模式
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;          //设置引脚速率
  HAL_GPIO_Init(GRATING_GPIO_PORT, &gpio_init_struct);      //初始化GPIO配置
}

/*********************************************************************************************
* 名称：unsigned char get_grating_status(void)
* 功能：获取红外光栅传感器状态
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_grating_status(void)
{   
  if(HAL_GPIO_ReadPin(GRATING_GPIO_PORT,GRATING_GPIO_PIN))  //判断红外光栅的状态
    return 0;
  else
    return 1;
}