/*********************************************************************************************
* �ļ���Relay.h
* ���ߣ�Lixm 2017.10.17
* ˵�����̵�����������ͷ�ļ�  
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ����������
*********************************************************************************************/
#ifndef __RELAY_H__
#define __RELAY_H__

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"

/* ���� ���� */
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
* �ڲ�ԭ�ͺ���
*********************************************************************************************/
void relay_init(void);                                          //�̵�����������ʼ��
void relay_control(unsigned char cmd);                        //�̵�������������

#endif /*__INFRARED_H__*/

