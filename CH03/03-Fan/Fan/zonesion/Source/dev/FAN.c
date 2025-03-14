/*********************************************************************************************
* �ļ���FAN.c
* ���ߣ�Lixm 2017.10.17
* ˵����������������
* �޸ģ�fuyou 2018.8.25 �������ȿ�������
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "FAN.h"

/***  ʹ��IO����  ***/
/*********************************************************************************************
* ���ƣ�fan_init()
* ���ܣ����ȴ�������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void fan_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  PWM_GPIO_CLK_ENABLE();                                  /* ʱ��ʹ�� */
  
  gpio_init_struct.Pin = PWM_GPIO_PIN;                    /* ���� */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
  gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        /* ���� */
  HAL_GPIO_Init(PWM_GPIO_PORT, &gpio_init_struct);        /* ��ʼ������ */

  HAL_GPIO_WritePin(PWM_GPIO_PORT, PWM_GPIO_PIN, GPIO_PIN_RESET);
}

/*********************************************************************************************
* ���ƣ�void fan_control(unsigned char cmd)
* ���ܣ����ȿ�������
* ��������������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void fan_control(unsigned char cmd)
{ 
  if(cmd & 0x01)
    HAL_GPIO_WritePin(PWM_GPIO_PORT, PWM_GPIO_PIN, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(PWM_GPIO_PORT, PWM_GPIO_PIN, GPIO_PIN_RESET);
}

/***  ʹ��PWM����  ***/
TIM_HandleTypeDef g_tim2_handle;                        /* ��ʱ����� */
TIM_OC_InitTypeDef g_tim2_ch3handle;                    /* ��ʱ��2ͨ��3��� */
static u32 cycle;                                       // ���ֵ��ҪС��100������ռ�ձȲ�׼ȷ
/*********************************************************************************************
* ���ƣ�fan_pwm_init()
* ���ܣ����ȴ�����PWM��ʼ��  PB10 ���� TIM2����CH3
* ������arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void fan_pwm_init(u32 arr,u32 psc)
{
  cycle=arr+1;                                                  // ��������ռ�ձȡ���Ҫ��1

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  g_tim2_handle.Instance = PWM_TIMX;                            /* ��ʱ�� */
  g_tim2_handle.Init.Prescaler = psc;                           /* ��ʱ����Ƶ */
  g_tim2_handle.Init.CounterMode = TIM_COUNTERMODE_UP;          /* ���ϼ���ģʽ */
  g_tim2_handle.Init.Period = arr;                              /* �Զ���װ��ֵ */
  g_tim2_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;/* ʹ��Ӱ�ӼĴ���Ԥ���� */
  if (HAL_TIM_PWM_Init(&g_tim2_handle) != HAL_OK)               /* ��ʼ��PWM */
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;    /* ��ʱ����ʱ��Դѡ���ڲ�ʱ�� */
  if (HAL_TIM_ConfigClockSource(&g_tim2_handle, &sClockSourceConfig) != HAL_OK) /* ����ʱ��Դ */
  {
    Error_Handler();
  }

  g_tim2_ch3handle.OCMode = TIM_OCMODE_PWM1;                    /* PWMģʽ1 */
  g_tim2_ch3handle.Pulse = arr;                                 /* ���ñȽ�ֵ */
  g_tim2_ch3handle.OCPolarity = TIM_OCPOLARITY_HIGH;            /* ����Ƚϼ���Ϊ�� */
  g_tim2_ch3handle.OCFastMode = TIM_OCFAST_DISABLE;             /* �������ģʽʧ�� */
  if (HAL_TIM_PWM_ConfigChannel(&g_tim2_handle, &g_tim2_ch3handle, PWM_TIMX_CHY) != HAL_OK) /* ����TIMͨ�� */
  {
    Error_Handler();
  }

  HAL_TIM_PWM_Start(&g_tim2_handle, PWM_TIMX_CHY);              /* ����PWMͨ�� */
  fan_pwm_control(0);                                           /* ���ó�ʼ�Ƚ�ֵΪ0 */
} 

/*********************************************************************************************
* ���ƣ�HAL_TIM_PWM_MspInit
* ���ܣ���ʱ���ײ�������ʱ��ʹ�ܣ���������
* ������htim:��ʱ�����
* ���أ���
* �޸ģ���
* ע�ͣ��˺����ᱻHAL_TIM_PWM_Init()����
*******************************************************************************************/
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
* ���ƣ�fan_pwm_control
* ���ܣ�����PWM��������
* ������pwm ռ�ձ� 0-100
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void fan_pwm_control(uint32_t pwm)
{ 
  uint32_t _pwm=cycle/100*pwm;
  __HAL_TIM_SET_COMPARE(&g_tim2_handle, PWM_TIMX_CHY, _pwm);    /* �޸ıȽ�ֵ���޸�ռ�ձ� */
}