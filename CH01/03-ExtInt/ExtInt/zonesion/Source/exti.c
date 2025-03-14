/*********************************************************************************************
* �ļ���exti.c
* ˵����
* ���ܣ��ⲿ�ж���������
* �޸ģ�������������
        zhoucj   2017.12.18 �޸�ע��
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "exti.h"
#include "key.h"
#include "delay.h"
extern char rgb_status;  
/*********************************************************************************************
* ���ƣ�exti_init
* ���ܣ��ⲿ�жϳ�ʼ��
* ��������
* ���أ���
* �޸ģ���
*********************************************************************************************/
// PB14 -- �ж���14  PB15 -- �ж���15  
void exti_init(void)
{
  GPIO_InitTypeDef gpio_init_struct;                            /* GPIO���ò����洢���� */

  key_init();                                                   //�������ų�ʼ��
  
  // GPIO ��ʼ��
  __HAL_RCC_GPIOB_CLK_ENABLE(); 
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;                               //ʹ��SYSCFGʱ��
  
  gpio_init_struct.Pin = GPIO_PIN_14|GPIO_PIN_15;             /* ���� */
  gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;               /* �½��ش��� */
  gpio_init_struct.Pull = GPIO_NOPULL;                        /* û�������� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
  HAL_GPIO_Init(GPIOB, &gpio_init_struct);                    /* ��ʼ������ */
  
  // EXTI ��ʼ��
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 1);                 /* ��ռ0�������ȼ�1 */
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);                         /* ʹ���ж���2 */
}
/*********************************************************************************************
* ���ƣ�EXTI15_10_IRQHandler
* ���ܣ��ⲿ�ж�15-10�жϴ�����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void EXTI15_10_IRQHandler(void)
{
  if(get_key_status() == K3_PRES || 
     get_key_status() == K4_PRES ){                             //���K3������
    delay_count(1000);                                           //��ʱ����
    if(get_key_status() == K3_PRES || 
       get_key_status() == K4_PRES ){                            //ȷ��K3������
      while(get_key_status() == K3_PRES || 
            get_key_status() == K4_PRES);                       //�ȴ������ɿ�
      rgb_status = !rgb_status;                                 //��תled״̬��־
    }
  }
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != 0x00U)
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);                        /* �˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != 0x00U)
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15); 
}
