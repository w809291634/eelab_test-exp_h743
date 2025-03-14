/*********************************************************************************************
* 文件: delay.h
* 作者：zonesion 2016.12.22
* 说明：延时函数头文件,延时相关函数声明
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 宏条件编译
*********************************************************************************************/
#ifndef _DELAY_H_
#define _DELAY_H_

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"

/*********************************************************************************************
* 函数声明
*********************************************************************************************/
void delay_init(uint16_t sysclk);   /* 初始化延迟函数 */
void delay_ms(uint16_t nms);        /* 延时nms */
void delay_us(uint32_t nus);        /* 延时nus */
void HAL_Delay(uint32_t Delay);     /* HAL库的延时函数，SDIO等需要用到 */

#endif /*_DELAY_H_*/





























