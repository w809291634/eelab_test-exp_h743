#ifndef _TIMER_H
#define _TIMER_H
#include "extend.h"

/* 通用定时器 定义 */
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  /* TIM3 时钟使能 */

void TIMX_Int_Init(uint16_t arr, uint16_t psc);
#endif
