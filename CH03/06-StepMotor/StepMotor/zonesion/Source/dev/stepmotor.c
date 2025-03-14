/*********************************************************************************************
* 文件：stepmotor.c
* 作者：Lixm 2017.10.17
* 说明：步进电机驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "stepmotor.h"

TIM_HandleTypeDef g_timx_handle;                                /* 定时器句柄 */
TIM_OC_InitTypeDef g_timx_chyhandle;                            /* 定时器通道句柄 */
/*********************************************************************************************
* 名称：stepmotor_init()
* 功能：步进电机传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void stepmotor_init(void)
{
  // PC1 DIR
  // PB8 连接STEP引脚  TIM4 Channel3 
  // PB9 EN
  GPIO_InitTypeDef gpio_init_struct={0};
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();                                 //开启步进电机相关的GPIO外设时钟
  __HAL_RCC_GPIOB_CLK_ENABLE(); 

  /* 初始化 DIR 和 EN 引脚 */
  gpio_init_struct.Pin = GPIO_PIN_1;                            /* DIR引脚 */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                  /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLDOWN;                        /* 下拉模式 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;              /* 中速 */
  HAL_GPIO_Init(GPIOC, &gpio_init_struct);                      /* 初始化引脚 */
  gpio_init_struct.Pin = GPIO_PIN_9;                            /* EN引脚 */
  HAL_GPIO_Init(GPIOB, &gpio_init_struct);                      /* 初始化引脚 */

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);           //步进电机失能
  
  /* 初始化PWM引脚 */
  g_timx_handle.Instance = TIM4;                                /* 定时器 */
  g_timx_handle.Init.Prescaler = 199;                           /* 定时器分频，TIM4位于APB1(200MHZ),分频后时钟源为1MHZ */
  g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;          /* 向上计数模式 */
  g_timx_handle.Init.Period = 999;                              /* 自动重装载值 */
  g_timx_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;/* 使能影子寄存器预加载 */
  if (HAL_TIM_PWM_Init(&g_timx_handle) != HAL_OK)               /* 初始化PWM */
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;    /* 定时器的时钟源选择内部时钟 */
  if (HAL_TIM_ConfigClockSource(&g_timx_handle, &sClockSourceConfig) != HAL_OK) /* 配置时钟源 */
  {
    Error_Handler();
  }

  g_timx_chyhandle.OCMode = TIM_OCMODE_PWM1;                    /* PWM模式1 */
  g_timx_chyhandle.Pulse = 0;                                   /* 设置比较值 */
  g_timx_chyhandle.OCPolarity = TIM_OCPOLARITY_HIGH;            /* 输出比较极性为高 */
  g_timx_chyhandle.OCFastMode = TIM_OCFAST_DISABLE;             /* 快速输出模式失能 */
  if (HAL_TIM_PWM_ConfigChannel(&g_timx_handle, &g_timx_chyhandle, TIM_CHANNEL_3) != HAL_OK) /* 配置TIM通道 */
  {
    Error_Handler();
  }

  HAL_TIM_PWM_Start(&g_timx_handle, TIM_CHANNEL_3);              /* 开启PWM通道 */
}

/*********************************************************************************************
* 名称：void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
* 功能：定时器底层驱动，时钟使能，引脚配置
* 参数：htim:定时器句柄
* 返回：无
* 修改：
* 注释：此函数会被HAL_TIM_PWM_Init()调用
*********************************************************************************************/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef gpio_init_struct;
  
  if (htim->Instance == TIM4)
  {
    __HAL_RCC_TIM4_CLK_ENABLE();                                /* 使能定时器 */
    __HAL_RCC_GPIOB_CLK_ENABLE();                               /* PWM GPIO 时钟使能 */

    gpio_init_struct.Pin = GPIO_PIN_8;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_struct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);                    /* TIMX PWM CHY 引脚模式设置 */
  }
}

/*********************************************************************************************
* 名称：void stepmotor_control(unsigned char cmd)
* 功能：步进电机控制驱动
* 参数：控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void stepmotor_control(unsigned char cmd)
{ 
  if(cmd & 0x01){
    if(cmd & 0x02){
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
      __HAL_TIM_SET_COMPARE(&g_timx_handle, TIM_CHANNEL_3, 500);/* 修改比较值 */
    }else{
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
      __HAL_TIM_SET_COMPARE(&g_timx_handle, TIM_CHANNEL_3, 500);/* 修改比较值 */
    }
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);         //步进电机使能
  }
  else HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
}