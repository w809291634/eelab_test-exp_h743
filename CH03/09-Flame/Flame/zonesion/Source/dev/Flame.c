/*********************************************************************************************
* �ļ���Flame.c
* ���ߣ�Lixm 2017.10.17
* ˵����������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "Flame.h"

#define FLAME_GPIO_PORT                  GPIOC
#define FLAME_GPIO_PIN                   GPIO_PIN_11
#define FLAME_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)
/*********************************************************************************************
* ���ƣ�flame_init()
* ���ܣ����洫������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void flame_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  FLAME_GPIO_CLK_ENABLE();                                //����������ص�GPIO����ʱ��

  gpio_init_struct.Pin = FLAME_GPIO_PIN;                  //ѡ��Ҫ���Ƶ�GPIO����
  gpio_init_struct.Mode = GPIO_MODE_INPUT;                //��������ģʽΪ����ģʽ
  gpio_init_struct.Pull = GPIO_PULLDOWN;                  //��������Ϊ����ģʽ
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        //������������
  HAL_GPIO_Init(FLAME_GPIO_PORT, &gpio_init_struct);      //��ʼ��GPIO����
}

/*********************************************************************************************
* ���ƣ�unsigned char get_flame_status(void)
* ���ܣ���ȡ���洫����״̬
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_flame_status(void)
{ 
  if(HAL_GPIO_ReadPin(FLAME_GPIO_PORT,FLAME_GPIO_PIN))
    return 0;
  else
    return 1;
}