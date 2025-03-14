#ifndef __BUZZER_H__
#define __BUZZER_H__
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "led.h"
#include "delay.h"

/*********************************************************************************************
* 引脚定义和配置
*********************************************************************************************/
//BUZZER--PG1
#define BUZZER_GPIO_PORT                  GPIOG
#define BUZZER_GPIO_PIN                   GPIO_PIN_1
#define BUZZER_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)

#define BUZZER_OPEN                       HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_SET);
#define BUZZER_CLOSE                      HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_RESET);

#define ON      0
#define OFF     1

/*********************************************************************************************
* 内部原型函数
*********************************************************************************************/
void buzzer_init(void);
void buzzer_tweet(void);

#endif
