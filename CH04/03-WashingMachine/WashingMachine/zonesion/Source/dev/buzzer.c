#include "buzzer.h"

/*********************************************************************************************
* ����:buzzer_init()
* ����:��������ʼ��
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void buzzer_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  BUZZER_GPIO_CLK_ENABLE();                                 /* ʱ��ʹ�� */
  
  gpio_init_struct.Pin = BUZZER_GPIO_PIN;                   /* LED���� */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;              /* ������� */
  gpio_init_struct.Pull = GPIO_PULLDOWN;                    /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;          /* ���� */
  HAL_GPIO_Init(BUZZER_GPIO_PORT, &gpio_init_struct);       /* ��ʼ������ */  
  
  BUZZER_CLOSE;
}

/*********************************************************************************************
* ����:buzzer_tweet()
* ����:��������һ��
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void buzzer_tweet(void)
{
  BUZZER_OPEN;
  delay_ms(30);
  BUZZER_CLOSE;
}
