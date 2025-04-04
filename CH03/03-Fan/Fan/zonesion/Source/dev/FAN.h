/*********************************************************************************************
* 文件：FAN.h
* 作者：Lixm 2017.10.17
* 说明：风扇驱动代码头文件  
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 宏条件编译
*********************************************************************************************/
#ifndef __FAN_H__
#define __FAN_H__

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"

/* PB10 连接 TIM2——CH3
 */
#define PWM_GPIO_PORT                    GPIOB
#define PWM_GPIO_PIN                     GPIO_PIN_10
#define PWM_GPIO_CLK_ENABLE()            do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)      /* 时钟使能 */
#define PWM_GPIO_AFTIMX                  GPIO_AF1_TIM2

#define PWM_TIMX                         TIM2
#define PWM_TIMX_CHY                     TIM_CHANNEL_3
#define PWM_TIMX_CLK_ENABLE()            do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)       /* TIM时钟使能 */

/*********************************************************************************************
* 内部原型函数
*********************************************************************************************/
void fan_init(void);                                            //风扇传感器初始化
void fan_control(unsigned char cmd);                            //风扇传感器控制
void fan_pwm_init(u32 arr,u32 psc);
void fan_pwm_control(uint32_t pwm);
#endif /*__FAN_H__*/
