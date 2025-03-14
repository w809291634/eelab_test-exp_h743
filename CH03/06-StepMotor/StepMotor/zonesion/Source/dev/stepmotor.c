/*********************************************************************************************
* �ļ���stepmotor.c
* ���ߣ�Lixm 2017.10.17
* ˵�������������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stepmotor.h"

TIM_HandleTypeDef g_timx_handle;                                /* ��ʱ����� */
TIM_OC_InitTypeDef g_timx_chyhandle;                            /* ��ʱ��ͨ����� */
/*********************************************************************************************
* ���ƣ�stepmotor_init()
* ���ܣ����������������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void stepmotor_init(void)
{
  // PC1 DIR
  // PB8 ����STEP����  TIM4 Channel3 
  // PB9 EN
  GPIO_InitTypeDef gpio_init_struct={0};
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();                                 //�������������ص�GPIO����ʱ��
  __HAL_RCC_GPIOB_CLK_ENABLE(); 

  /* ��ʼ�� DIR �� EN ���� */
  gpio_init_struct.Pin = GPIO_PIN_1;                            /* DIR���� */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                  /* ������� */
  gpio_init_struct.Pull = GPIO_PULLDOWN;                        /* ����ģʽ */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;              /* ���� */
  HAL_GPIO_Init(GPIOC, &gpio_init_struct);                      /* ��ʼ������ */
  gpio_init_struct.Pin = GPIO_PIN_9;                            /* EN���� */
  HAL_GPIO_Init(GPIOB, &gpio_init_struct);                      /* ��ʼ������ */

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);           //�������ʧ��
  
  /* ��ʼ��PWM���� */
  g_timx_handle.Instance = TIM4;                                /* ��ʱ�� */
  g_timx_handle.Init.Prescaler = 199;                           /* ��ʱ����Ƶ��TIM4λ��APB1(200MHZ),��Ƶ��ʱ��ԴΪ1MHZ */
  g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;          /* ���ϼ���ģʽ */
  g_timx_handle.Init.Period = 999;                              /* �Զ���װ��ֵ */
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
  g_timx_chyhandle.Pulse = 0;                                   /* ���ñȽ�ֵ */
  g_timx_chyhandle.OCPolarity = TIM_OCPOLARITY_HIGH;            /* ����Ƚϼ���Ϊ�� */
  g_timx_chyhandle.OCFastMode = TIM_OCFAST_DISABLE;             /* �������ģʽʧ�� */
  if (HAL_TIM_PWM_ConfigChannel(&g_timx_handle, &g_timx_chyhandle, TIM_CHANNEL_3) != HAL_OK) /* ����TIMͨ�� */
  {
    Error_Handler();
  }

  HAL_TIM_PWM_Start(&g_timx_handle, TIM_CHANNEL_3);              /* ����PWMͨ�� */
}

/*********************************************************************************************
* ���ƣ�void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
* ���ܣ���ʱ���ײ�������ʱ��ʹ�ܣ���������
* ������htim:��ʱ�����
* ���أ���
* �޸ģ�
* ע�ͣ��˺����ᱻHAL_TIM_PWM_Init()����
*********************************************************************************************/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef gpio_init_struct;
  
  if (htim->Instance == TIM4)
  {
    __HAL_RCC_TIM4_CLK_ENABLE();                                /* ʹ�ܶ�ʱ�� */
    __HAL_RCC_GPIOB_CLK_ENABLE();                               /* PWM GPIO ʱ��ʹ�� */

    gpio_init_struct.Pin = GPIO_PIN_8;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_struct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);                    /* TIMX PWM CHY ����ģʽ���� */
  }
}

/*********************************************************************************************
* ���ƣ�void stepmotor_control(unsigned char cmd)
* ���ܣ����������������
* ��������������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void stepmotor_control(unsigned char cmd)
{ 
  if(cmd & 0x01){
    if(cmd & 0x02){
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
      __HAL_TIM_SET_COMPARE(&g_timx_handle, TIM_CHANNEL_3, 500);/* �޸ıȽ�ֵ */
    }else{
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
      __HAL_TIM_SET_COMPARE(&g_timx_handle, TIM_CHANNEL_3, 500);/* �޸ıȽ�ֵ */
    }
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);         //�������ʹ��
  }
  else HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
}