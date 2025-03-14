#include "timer.h"
#include "led.h"
#include "usart.h"
#include "dcmi.h"

/* 定时器配置句柄 定义 */
TIM_HandleTypeDef g_timx_handle;                          /* 定时器x句柄 */

/*********************************************************************************************
* 名称:TIMX_Int_Init
* 功能:通用定时器TIMX定时中断初始化函数
* 参数:arr: 自动重装值
*        psc: 预分频系数
* 返回:无
* 修改:无
* 注释:定时器的时钟来自APB1 或 APB2, 当D2PPRE1（D2PPRE2）≥2分频的时候
*        通用定时器的时钟为APB1 或 APB2时钟的2倍, 而APB1（APB2）为100M,
*        所以定时器时钟 = 200Mhz
*        定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
*        Ft=定时器工作频率,单位:Mhz
*********************************************************************************************/    
void TIMX_Int_Init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();                             /* 使能TIMx时钟 */

    g_timx_handle.Instance = GTIM_TIMX_INT;                 /* 通用定时器x */
    g_timx_handle.Init.Prescaler = psc;                     /* 预分频系数 */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;    /* 递增计数模式 */
    g_timx_handle.Init.Period = arr;                        /* 自动装载值 */
    HAL_TIM_Base_Init(&g_timx_handle);

    HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 2, 0);         /* 设置中断优先级，抢占优先级2，子优先级1 */
    HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);                 /* 开启ITMx中断 */

    HAL_TIM_Base_Start_IT(&g_timx_handle);                  /* 使能定时器x和定时器x更新中断 */
}

//定时器3中断服务函数
/*********************************************************************************************
* 名称:GTIM_TIMX_INT_IRQHandler
* 功能:定时器中断服务函数
* 参数:无
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/    
void GTIM_TIMX_INT_IRQHandler(void)
{
  /* 以下代码没有使用定时器HAL库共用处理函数来处理，而是直接通过判断中断标志位的方式 */
  if (__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET)
  {
    ov_frame=ov_frame_cnt;
    ov_frame_cnt=0;
    LED4_TOGGLE();                                      //LED指示应用
    //printf("frame:%d\r\n",ov_frame);                  //打印帧率
    __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);  /* 清除定时器溢出中断标志位 */
  }
}
