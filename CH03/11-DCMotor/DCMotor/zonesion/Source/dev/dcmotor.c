#include "dcmotor.h"
#include "delay.h"

static vs32 encoder_count;
vu32 dcmotor_dir;
double dcmotor_speed;
static TIM_HandleTypeDef g_timx_handle;                         // 定时器参数句柄
#define REDUCTION_RATIO       150                               // 电机减速比
#define PULSE_PERIOD          6                                 // 使用单脉冲计数，编码器端一圈大约6计数
/*********************************************************************************************
* 名称:dcmotor_init()
* 功能:直流电机初始化函数
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void dcmotor_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  DCMOTOR_GPIO1_CLK_ENABLE();                                   /* 时钟使能 */
  DCMOTOR_GPIO2_CLK_ENABLE();
  
  gpio_init_struct.Pin = DCMOTOR_GPIO1_PIN;                     /* 引脚 */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                  /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_NOPULL;                          /* 无上下拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;           /* 高速 */
  HAL_GPIO_Init(DCMOTOR_GPIO1_PORT, &gpio_init_struct);         /* 初始化引脚 */

  gpio_init_struct.Pin = DCMOTOR_GPIO2_PIN ;
  HAL_GPIO_Init(DCMOTOR_GPIO2_PORT, &gpio_init_struct);         /* 初始化引脚 */
  
  DCMOTOR_STOP;
  encoder_init(1000,20000-1);                                   //APB1总线200mhz，0.1ms计数周期，0.1*1000=0.1s溢出
}

/*********************************************************************************************
* 名称:encoder_init()
* 功能:直流电机编码器初始化函数 使用PB1获取编码器脉冲计数值,定时器作为一个速度时基
* 参数:arr 定时器 arr寄存器值 psr 预分频值
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void encoder_init(u32 arr,u32 psc)
{
  GPIO_InitTypeDef gpio_init_struct;
  // 定时器初始化
  GTIM_TIMX_INT_CLK_ENABLE();                                   /* 使能TIMx时钟 */

  g_timx_handle.Instance = GTIM_TIMX_INT;                       /* 通用定时器x */
  g_timx_handle.Init.Prescaler = psc;                           /* 预分频系数 */
  g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;          /* 递增计数模式 */
  g_timx_handle.Init.Period = arr;                              /* 自动装载值 */
  HAL_TIM_Base_Init(&g_timx_handle);

  HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 1, 0);               /* 设置中断优先级，抢占优先级1，子优先级0 */
  HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);                       /* 开启ITMx中断 */

  HAL_TIM_Base_Start_IT(&g_timx_handle);                        /* 使能定时器x和定时器x更新中断 */

  // EXTI 引脚 初始化
  DCMOTOR_ENCODER_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;                               /* 使能SYSCFG时钟 */

  gpio_init_struct.Pin = DCMOTOR_ENCODER_PIN;                   /* 引脚 */
  gpio_init_struct.Mode = GPIO_MODE_IT_RISING;                  /* 上升沿触发 */
  gpio_init_struct.Pull = GPIO_PULLDOWN;                        /* 下拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                /* 高速 */
  HAL_GPIO_Init(DCMOTOR_ENCODER_PORT, &gpio_init_struct);       /* 初始化引脚 */
  
  // EXTI 中断初始化
  HAL_NVIC_SetPriority(DCMOTOR_ENCODER_IRQn, 0, 0);             /* 抢占0，子优先级0 */
  HAL_NVIC_EnableIRQ(DCMOTOR_ENCODER_IRQn);                     /* 使能中断线 */
}

/*********************************************************************************************
* 名称:DCMOTOR_ENCODER_IRQHandler()
* 功能:编码器中断服务程序
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void DCMOTOR_ENCODER_IRQHandler(void)
{
  static char last_dir;
  if(last_dir!=dcmotor_dir){
    last_dir=dcmotor_dir;
    encoder_count=0;
  }
  if(dcmotor_dir) encoder_count++;
  else encoder_count--;
  
  __HAL_GPIO_EXTI_CLEAR_IT(DCMOTOR_ENCODER_PIN);                // 清理中断标志位
  delay_us(1);
  HAL_NVIC_ClearPendingIRQ(DCMOTOR_ENCODER_IRQn);               // 清理正在挂起的中断号，避免二次触发
}

/*********************************************************************************************
* 名称:GTIM_TIMX_INT_IRQHandler()
* 功能:定时器中断服务函数  
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void GTIM_TIMX_INT_IRQHandler(void)
{
  double count;
  /* 以下代码没有使用定时器HAL库共用处理函数来处理，而是直接通过判断中断标志位的方式 */
  if (__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET)
  {
    count = encoder_count*1000/100;                         //定时器100ms溢出，这里单位 计数/s
    //count = encoder_count*60*1000/100;                    //计数/min
    encoder_count =0;
    dcmotor_speed = count/REDUCTION_RATIO/PULSE_PERIOD;     //电机减速器端实际转速 r/s
    __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);      //清除定时器溢出中断标志位
  }
}
