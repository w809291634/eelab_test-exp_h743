/*********************************************************************************************
* 文件：Relay.h
* 作者：Lixm 2017.10.17
* 说明：继电器驱动代码头文件  
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 宏条件编译
*********************************************************************************************/
#ifndef __RELAY_H__
#define __RELAY_H__

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"

/* 引脚 定义 */
// RELAY1--PE2    RELAY2--PE3
#define RELAY1_GPIO_PORT                  GPIOE
#define RELAY1_GPIO_PIN                   GPIO_PIN_2
#define RELAY1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)

#define RELAY2_GPIO_PORT                  GPIOE
#define RELAY2_GPIO_PIN                   GPIO_PIN_3
#define RELAY2_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)

#define RELAY1_CTRL(a)          if(!a) HAL_GPIO_WritePin(RELAY1_GPIO_PORT, RELAY1_GPIO_PIN, GPIO_PIN_SET); \
                                else HAL_GPIO_WritePin(RELAY1_GPIO_PORT, RELAY1_GPIO_PIN, GPIO_PIN_RESET)
                                  
#define RELAY2_CTRL(a)          if(!a) HAL_GPIO_WritePin(RELAY2_GPIO_PORT, RELAY2_GPIO_PIN, GPIO_PIN_SET); \
                                else HAL_GPIO_WritePin(RELAY2_GPIO_PORT, RELAY2_GPIO_PIN, GPIO_PIN_RESET)
                                  
/*********************************************************************************************
* 内部原型函数
*********************************************************************************************/
void relay_init(void);                                          //继电器传感器初始化
void relay_control(unsigned char cmd);                        //继电器传感器控制

#endif /*__INFRARED_H__*/

