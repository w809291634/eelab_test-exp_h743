#ifndef __BUZZER_H__
#define __BUZZER_H__
/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"
#include "led.h"
#include "delay.h"

/*********************************************************************************************
* ���Ŷ��������
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
* �ڲ�ԭ�ͺ���
*********************************************************************************************/
void buzzer_init(void);
void buzzer_tweet(void);

#endif
