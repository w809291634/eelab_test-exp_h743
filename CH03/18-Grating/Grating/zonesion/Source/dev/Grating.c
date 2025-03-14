/*********************************************************************************************
* �ļ���Grating.c
* ���ߣ�Lixm 2017.10.17
* ˵���������դ��������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "Grating.h"

#define GRATING_GPIO_PORT                  GPIOA
#define GRATING_GPIO_PIN                   GPIO_PIN_1
#define GRATING_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)

/*********************************************************************************************
* ���ƣ�grating_init()
* ���ܣ������դ��������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void grating_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  GRATING_GPIO_CLK_ENABLE();                                //����GPIO����ʱ��

  gpio_init_struct.Pin = GRATING_GPIO_PIN;                  //ѡ��Ҫ���Ƶ�GPIO����
  gpio_init_struct.Mode = GPIO_MODE_INPUT;                  //��������ģʽΪ����ģʽ
  gpio_init_struct.Pull = GPIO_PULLDOWN;                    //��������Ϊ����ģʽ
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;          //������������
  HAL_GPIO_Init(GRATING_GPIO_PORT, &gpio_init_struct);      //��ʼ��GPIO����
}

/*********************************************************************************************
* ���ƣ�unsigned char get_grating_status(void)
* ���ܣ���ȡ�����դ������״̬
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_grating_status(void)
{   
  if(HAL_GPIO_ReadPin(GRATING_GPIO_PORT,GRATING_GPIO_PIN))  //�жϺ����դ��״̬
    return 0;
  else
    return 1;
}