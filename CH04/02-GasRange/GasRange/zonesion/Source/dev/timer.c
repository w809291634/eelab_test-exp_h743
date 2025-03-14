/*********************************************************************************************
* 文件：timer.c
* 作者：chennian 2017.11.2
* 说明：
* 功能：定时器驱动代码
* 修改：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "timer.h"

/* 定时器配置句柄 定义 */
TIM_HandleTypeDef g_timx_handle;                          /* 定时器x句柄 */

/*********************************************************************************************
* 名称：通用定时器TIMX定时中断初始化函数
* 功能：初始化TIM3
* 参数：period：自动重装值。 prescaler：时钟预分频数
* 返回：无
* 修改：
* 注释：定时器的时钟来自APB1 或 APB2, 当D2PPRE1（D2PPRE2）≥2分频的时候
 *              通用定时器的时钟为APB1 或 APB2时钟的2倍, 而APB1（APB2）为100M,
 *              所以定时器时钟 = 200Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
*********************************************************************************************/
void timer_init(void)
{
    GTIM_TIMX_INT_CLK_ENABLE();                             /* 使能TIMx时钟 */
    //配置中断触发时长为10MS
    g_timx_handle.Instance = GTIM_TIMX_INT;                 /* 通用定时器x */
    g_timx_handle.Init.Period = 1000 - 1;                   /* 自动装载值 */
    g_timx_handle.Init.Prescaler = 2000 - 1;                /* 预分频系数 */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;    /* 递增计数模式 */
    HAL_TIM_Base_Init(&g_timx_handle);

    HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 2, 0);         /* 设置中断优先级，抢占优先级2，子优先级0 */
    HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);                 /* 开启ITMx中断 */

    HAL_TIM_Base_Start_IT(&g_timx_handle);                  /* 使能定时器x和定时器x更新中断 */
}


/*********************************************************************************************
* 名称：GTIM_TIMX_INT_IRQHandler
* 功能：通用定时器中断处理函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
extern uint8_t rgb_twinkle;                                     //RGB灯闪烁执行标志位
extern uint8_t gas_check;                                       //燃气检测执行标志位
extern uint8_t fire_effect;                                     //火焰图标闪烁执行标志位
uint8_t count_led = 0;                                          //RGB灯闪烁延时
uint8_t count_gas = 0;                                          //燃气检测延时
uint8_t count_fire = 0;                                         //火焰图案闪烁执行延时

void GTIM_TIMX_INT_IRQHandler(void)
{
  if (__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET) {   //如果中断标志被设置
    //rgb灯闪烁执行标志位控制
    if((count_led % 10) == 9){                                  //RGB灯计时是否满100MS
      rgb_twinkle = 1;                                          //标志位置位
      count_led = 0;                                            //计数清零
    }else count_led ++;                                         //计数加一
    //燃气检测执行标志位控制
    if((count_gas % 100) == 99){                                //燃气检测延时是否到1S
      count_gas = 0;                                            //计数清零
      gas_check = 1;                                            //标志位置位
    }else  count_gas ++;                                        //参数加一
    //火焰图标闪烁标执行标志位控制
    if((count_fire % 5) == 4){                                  //火焰图标闪烁是否到50ms
      fire_effect = 1;                                          //标志位置位
      count_fire = 0;                                           //计数清零
    }else count_fire ++;                                        //计数加一
    
    __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);                  //清除中断标志
  }
}