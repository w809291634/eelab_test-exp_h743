/*********************************************************************************************
* 文件：key.h
* 作者：Liyw 2017.5.30
* 说明：按键驱动代码头文件  
* 修改：
*********************************************************************************************/

/*********************************************************************************************
* 宏条件编译
*********************************************************************************************/
#ifndef __TIMER_H__
#define __TIMER_H__

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
/* 通用定时器 定义 */
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  /* TIM3 时钟使能 */
/*********************************************************************************************
* 函数声明
*********************************************************************************************/
void TIMX_Int_Init(unsigned int arr, unsigned short psc);                                       

#endif /*__TIMER_H_*/