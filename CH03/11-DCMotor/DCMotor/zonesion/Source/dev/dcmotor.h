#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__
#include "extend.h"

extern vu32 dcmotor_dir;
extern double dcmotor_speed;

// ������ʱ������
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  /* TIM3 ʱ��ʹ�� */

// �������������������Ŷ���
#define DCMOTOR_ENCODER_PORT                GPIOB
#define DCMOTOR_ENCODER_PIN                 GPIO_PIN_1
#define DCMOTOR_ENCODER_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)
#define DCMOTOR_ENCODER_IRQn                EXTI1_IRQn
#define DCMOTOR_ENCODER_IRQHandler          EXTI1_IRQHandler

// ����������Ŷ���
#define DCMOTOR_GPIO1_PORT                  GPIOA
#define DCMOTOR_GPIO1_PIN                   GPIO_PIN_7
#define DCMOTOR_GPIO1_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)

#define DCMOTOR_GPIO2_PORT                  GPIOB
#define DCMOTOR_GPIO2_PIN                   GPIO_PIN_0
#define DCMOTOR_GPIO2_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

// ������ƺ�
#define DCMOTOR_FORWARD       { HAL_GPIO_WritePin(DCMOTOR_GPIO1_PORT,DCMOTOR_GPIO1_PIN,GPIO_PIN_SET); \
                                HAL_GPIO_WritePin(DCMOTOR_GPIO2_PORT,DCMOTOR_GPIO2_PIN,GPIO_PIN_RESET); \
                                dcmotor_dir=1;}
#define DCMOTOR_BACKWARD      { HAL_GPIO_WritePin(DCMOTOR_GPIO1_PORT,DCMOTOR_GPIO1_PIN,GPIO_PIN_RESET); \
                                HAL_GPIO_WritePin(DCMOTOR_GPIO2_PORT,DCMOTOR_GPIO2_PIN,GPIO_PIN_SET); \
                                dcmotor_dir=0;}
#define DCMOTOR_STOP          { HAL_GPIO_WritePin(DCMOTOR_GPIO1_PORT,DCMOTOR_GPIO1_PIN,GPIO_PIN_SET); \
                                HAL_GPIO_WritePin(DCMOTOR_GPIO2_PORT,DCMOTOR_GPIO2_PIN,GPIO_PIN_SET);}

void dcmotor_init(void);
void encoder_init(u32 arr,u32 psc);
#endif
