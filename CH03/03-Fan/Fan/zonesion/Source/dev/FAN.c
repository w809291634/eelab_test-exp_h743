/*********************************************************************************************
* 文件：FAN.c
* 作者：Lixm 2017.10.17
* 说明：风扇驱动代码
* 修改：fuyou 2018.8.25 修正风扇控制驱动
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "FAN.h"

/***  使用IO控制  ***/
/*********************************************************************************************
* 名称：fan_init()
* 功能：风扇传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void fan_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  PWM_GPIO_CLK_ENABLE();                                  /* 时钟使能 */
  
  gpio_init_struct.Pin = PWM_GPIO_PIN;                    /* 引脚 */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        /* 中速 */
  HAL_GPIO_Init(PWM_GPIO_PORT, &gpio_init_struct);        /* 初始化引脚 */

  HAL_GPIO_WritePin(PWM_GPIO_PORT, PWM_GPIO_PIN, GPIO_PIN_RESET);
}

/*********************************************************************************************
* 名称：void fan_control(unsigned char cmd)
* 功能：风扇控制驱动
* 参数：控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void fan_control(unsigned char cmd)
{ 
  if(cmd & 0x01)
    HAL_GPIO_WritePin(PWM_GPIO_PORT, PWM_GPIO_PIN, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(PWM_GPIO_PORT, PWM_GPIO_PIN, GPIO_PIN_RESET);
}

/***  使用PWM控制  ***/
TIM_HandleTypeDef g_tim2_handle;                        /* 定时器句柄 */
TIM_OC_InitTypeDef g_tim2_ch3handle;                    /* 定时器2通道3句柄 */
static u32 cycle;                                       // 这个值不要小于100，否则占空比不准确
/*********************************************************************************************
* 名称：fan_pwm_init()
* 功能：风扇传感器PWM初始化  PB10 连接 TIM2——CH3
* 参数：arr：自动重装值  psc：时钟预分频数
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void fan_pwm_init(u32 arr,u32 psc)
{
  cycle=arr+1;                                                  // 用来计算占空比。需要加1

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  g_tim2_handle.Instance = PWM_TIMX;                            /* 定时器 */
  g_tim2_handle.Init.Prescaler = psc;                           /* 定时器分频 */
  g_tim2_handle.Init.CounterMode = TIM_COUNTERMODE_UP;          /* 向上计数模式 */
  g_tim2_handle.Init.Period = arr;                              /* 自动重装载值 */
  g_tim2_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;/* 使能影子寄存器预加载 */
  if (HAL_TIM_PWM_Init(&g_tim2_handle) != HAL_OK)               /* 初始化PWM */
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;    /* 定时器的时钟源选择内部时钟 */
  if (HAL_TIM_ConfigClockSource(&g_tim2_handle, &sClockSourceConfig) != HAL_OK) /* 配置时钟源 */
  {
    Error_Handler();
  }

  g_tim2_ch3handle.OCMode = TIM_OCMODE_PWM1;                    /* PWM模式1 */
  g_tim2_ch3handle.Pulse = arr;                                 /* 设置比较值 */
  g_tim2_ch3handle.OCPolarity = TIM_OCPOLARITY_HIGH;            /* 输出比较极性为高 */
  g_tim2_ch3handle.OCFastMode = TIM_OCFAST_DISABLE;             /* 快速输出模式失能 */
  if (HAL_TIM_PWM_ConfigChannel(&g_tim2_handle, &g_tim2_ch3handle, PWM_TIMX_CHY) != HAL_OK) /* 配置TIM通道 */
  {
    Error_Handler();
  }

  HAL_TIM_PWM_Start(&g_tim2_handle, PWM_TIMX_CHY);              /* 开启PWM通道 */
  fan_pwm_control(0);                                           /* 设置初始比较值为0 */
} 

/*********************************************************************************************
* 名称：HAL_TIM_PWM_MspInit
* 功能：定时器底层驱动，时钟使能，引脚配置
* 参数：htim:定时器句柄
* 返回：无
* 修改：无
* 注释：此函数会被HAL_TIM_PWM_Init()调用
*******************************************************************************************/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef gpio_init_struct;

  if (htim->Instance == PWM_TIMX)
  {
    PWM_TIMX_CLK_ENABLE();                                      /* 使能定时器 */
    PWM_GPIO_CLK_ENABLE();                                      /* PWM GPIO 时钟使能 */

    gpio_init_struct.Pin = PWM_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init_struct.Alternate = PWM_GPIO_AFTIMX;
    HAL_GPIO_Init(PWM_GPIO_PORT, &gpio_init_struct);            /* TIMX PWM CHY 引脚模式设置 */
  }
}

/*********************************************************************************************
* 名称：fan_pwm_control
* 功能：风扇PWM驱动控制
* 参数：pwm 占空比 0-100
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void fan_pwm_control(uint32_t pwm)
{ 
  uint32_t _pwm=cycle/100*pwm;
  __HAL_TIM_SET_COMPARE(&g_tim2_handle, PWM_TIMX_CHY, _pwm);    /* 修改比较值，修改占空比 */
}