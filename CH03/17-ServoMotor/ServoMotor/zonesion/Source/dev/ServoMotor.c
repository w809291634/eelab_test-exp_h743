/*********************************************************************************************
* 文件：ServoMotor.c
* 作者：Lixm 2017.10.17
* 说明：继电器驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "ServoMotor.h"
#include <math.h> 

/* PA5 连接 TIM2_CH1
 */
#define PWM_GPIO_PORT                    GPIOA
#define PWM_GPIO_PIN                     GPIO_PIN_5
#define PWM_GPIO_CLK_ENABLE()            do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)      /* 时钟使能 */
#define PWM_GPIO_AFTIMX                  GPIO_AF1_TIM2

#define PWM_TIMX                         TIM2
#define PWM_TIMX_CHY                     TIM_CHANNEL_1
#define PWM_TIMX_CLK_ENABLE()            do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)       /* TIM时钟使能 */
TIM_HandleTypeDef g_timx_handle;                                /* 定时器句柄 */
TIM_OC_InitTypeDef g_timx_chyhandle;                            /* 定时器x通道y句柄 */
/*********************************************************************************************
* 名称：ServoMotor_init()
* 功能：舵机PWM初始化  PA5/TIM2_CH1
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void ServoMotor_init(u32 arr,u32 psc)
{
  //此部分需手动修改IO口设置
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  g_timx_handle.Instance = PWM_TIMX;                            /* 定时器 */
  g_timx_handle.Init.Prescaler = psc;                           /* 定时器分频 */
  g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;          /* 向上计数模式 */
  g_timx_handle.Init.Period = arr;                              /* 自动重装载值 */
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
  g_timx_chyhandle.Pulse = arr;                                 /* 设置比较值 */
  g_timx_chyhandle.OCPolarity = TIM_OCPOLARITY_HIGH;            /* 输出比较极性为高 */
  g_timx_chyhandle.OCFastMode = TIM_OCFAST_DISABLE;             /* 快速输出模式失能 */
  if (HAL_TIM_PWM_ConfigChannel(&g_timx_handle, &g_timx_chyhandle, PWM_TIMX_CHY) != HAL_OK) /* 配置TIM通道 */
  {
    Error_Handler();
  }

  HAL_TIM_PWM_Start(&g_timx_handle, PWM_TIMX_CHY);              /* 开启PWM通道 */
}

/*********************************************************************************************
* 名称：ServoPWM_control
* 功能：定时器底层驱动，时钟使能，引脚配置
        此函数会被HAL_TIM_PWM_Init()调用
* 参数：htim:定时器句柄
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
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
* 名称：ServoPWM_control
* 功能：伺服PWM驱动控制
        20ms -- 20000 arr值 -- 20000 us -- 1us对应1个arr值
        0.5ms -- 500 arr值   2.5%
        2.5ms -- 2500 arr值  12.5%
        周期为 20ms  0.5ms-2.5ms 高电位占空 控制舵机0-180度
* 参数：angle 角度 0-180
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
#define OFFSET  1.0f                                            //舵机角度修正值
void Servo_control(u8 angle)
{ 
  u32 pwm = (u32)round((2000* angle)/180.0f);
  __HAL_TIM_SET_COMPARE(&g_timx_handle, PWM_TIMX_CHY, (500+pwm)/OFFSET);    /* 修改比较值，修改占空比 */
}
