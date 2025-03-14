/*********************************************************************************************
* �ļ���ServoMotor.c
* ���ߣ�Lixm 2017.10.17
* ˵�����̵�����������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "ServoMotor.h"
#include <math.h> 

/* PA5 ���� TIM2_CH1
 */
#define PWM_GPIO_PORT                    GPIOA
#define PWM_GPIO_PIN                     GPIO_PIN_5
#define PWM_GPIO_CLK_ENABLE()            do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)      /* ʱ��ʹ�� */
#define PWM_GPIO_AFTIMX                  GPIO_AF1_TIM2

#define PWM_TIMX                         TIM2
#define PWM_TIMX_CHY                     TIM_CHANNEL_1
#define PWM_TIMX_CLK_ENABLE()            do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)       /* TIMʱ��ʹ�� */
TIM_HandleTypeDef g_timx_handle;                                /* ��ʱ����� */
TIM_OC_InitTypeDef g_timx_chyhandle;                            /* ��ʱ��xͨ��y��� */
/*********************************************************************************************
* ���ƣ�ServoMotor_init()
* ���ܣ����PWM��ʼ��  PA5/TIM2_CH1
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void ServoMotor_init(u32 arr,u32 psc)
{
  //�˲������ֶ��޸�IO������
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  g_timx_handle.Instance = PWM_TIMX;                            /* ��ʱ�� */
  g_timx_handle.Init.Prescaler = psc;                           /* ��ʱ����Ƶ */
  g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;          /* ���ϼ���ģʽ */
  g_timx_handle.Init.Period = arr;                              /* �Զ���װ��ֵ */
  g_timx_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;/* ʹ��Ӱ�ӼĴ���Ԥ���� */
  if (HAL_TIM_PWM_Init(&g_timx_handle) != HAL_OK)               /* ��ʼ��PWM */
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;    /* ��ʱ����ʱ��Դѡ���ڲ�ʱ�� */
  if (HAL_TIM_ConfigClockSource(&g_timx_handle, &sClockSourceConfig) != HAL_OK) /* ����ʱ��Դ */
  {
    Error_Handler();
  }

  g_timx_chyhandle.OCMode = TIM_OCMODE_PWM1;                    /* PWMģʽ1 */
  g_timx_chyhandle.Pulse = arr;                                 /* ���ñȽ�ֵ */
  g_timx_chyhandle.OCPolarity = TIM_OCPOLARITY_HIGH;            /* ����Ƚϼ���Ϊ�� */
  g_timx_chyhandle.OCFastMode = TIM_OCFAST_DISABLE;             /* �������ģʽʧ�� */
  if (HAL_TIM_PWM_ConfigChannel(&g_timx_handle, &g_timx_chyhandle, PWM_TIMX_CHY) != HAL_OK) /* ����TIMͨ�� */
  {
    Error_Handler();
  }

  HAL_TIM_PWM_Start(&g_timx_handle, PWM_TIMX_CHY);              /* ����PWMͨ�� */
}

/*********************************************************************************************
* ���ƣ�ServoPWM_control
* ���ܣ���ʱ���ײ�������ʱ��ʹ�ܣ���������
        �˺����ᱻHAL_TIM_PWM_Init()����
* ������htim:��ʱ�����
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef gpio_init_struct;

  if (htim->Instance == PWM_TIMX)
  {
    PWM_TIMX_CLK_ENABLE();                                      /* ʹ�ܶ�ʱ�� */
    PWM_GPIO_CLK_ENABLE();                                      /* PWM GPIO ʱ��ʹ�� */

    gpio_init_struct.Pin = PWM_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init_struct.Alternate = PWM_GPIO_AFTIMX;
    HAL_GPIO_Init(PWM_GPIO_PORT, &gpio_init_struct);            /* TIMX PWM CHY ����ģʽ���� */
  }
}

/*********************************************************************************************
* ���ƣ�ServoPWM_control
* ���ܣ��ŷ�PWM��������
        20ms -- 20000 arrֵ -- 20000 us -- 1us��Ӧ1��arrֵ
        0.5ms -- 500 arrֵ   2.5%
        2.5ms -- 2500 arrֵ  12.5%
        ����Ϊ 20ms  0.5ms-2.5ms �ߵ�λռ�� ���ƶ��0-180��
* ������angle �Ƕ� 0-180
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#define OFFSET  1.0f                                            //����Ƕ�����ֵ
void Servo_control(u8 angle)
{ 
  u32 pwm = (u32)round((2000* angle)/180.0f);
  __HAL_TIM_SET_COMPARE(&g_timx_handle, PWM_TIMX_CHY, (500+pwm)/OFFSET);    /* �޸ıȽ�ֵ���޸�ռ�ձ� */
}
