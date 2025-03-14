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
static TIM_HandleTypeDef g_timx_handle;                          /* 定时器x句柄 */

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
extern uint8_t rgb_twinkle;
extern uint8_t tim_second;
extern uint8_t mode_twinkle;
uint8_t count_sec = 0;
uint8_t count_rgb = 0;
uint8_t count_mode = 0;
uint32_t ticks = 0;                                             // 系统的运行时间计数值

void GTIM_TIMX_INT_IRQHandler(void)
{
  if (__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET) {   //如果中断标志被设置
    ticks++;
    
    if((count_rgb % 20) == 19){
      rgb_twinkle = 1;
      count_rgb = 0; 
    }else count_rgb ++; 
    
    if((count_sec % 50) == 49){
      count_sec = 0;
      tim_second = 1;
    }else  count_sec ++;
    
    if((count_mode % 40) == 39){
      mode_twinkle = 1;
      count_mode = 0;
    }else count_mode ++;
    
    __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);                  //清除中断标志
  }
}

/*********************************************************************************************
* 名称：xtime()
* 功能：系统的运行时间获取
* 参数：无
* 返回：系统的运行时间，单位为ms，注意这里需要和配置tim的时基相关
* 修改：
* 注释：
*********************************************************************************************/
uint32_t xtime(void){
  return (uint32_t)(ticks*10);
}