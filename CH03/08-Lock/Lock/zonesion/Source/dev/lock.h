/*********************************************************************************************
* 宏条件编译
*********************************************************************************************/
#ifndef __LOCK_H__
#define __LOCK_H__

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"

//LOCK--PC10
#define LOCK_GPIO_PORT                  GPIOC
#define LOCK_GPIO_PIN                   GPIO_PIN_10
#define LOCK_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)

#define LOCK_OPEN                       HAL_GPIO_WritePin(LOCK_GPIO_PORT, LOCK_GPIO_PIN, GPIO_PIN_SET);
#define LOCK_CLOSE                      HAL_GPIO_WritePin(LOCK_GPIO_PORT, LOCK_GPIO_PIN, GPIO_PIN_RESET);
/*********************************************************************************************
* 内部原型函数
*********************************************************************************************/
void lock_init(void);                                          //门锁传感器初始化

#endif /*__INFRARED_H__*/

