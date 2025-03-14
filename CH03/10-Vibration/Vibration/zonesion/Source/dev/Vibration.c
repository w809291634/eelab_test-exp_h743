/*********************************************************************************************
* �ļ���Vibration.c
* ���ߣ�Lixm 2017.10.17
* ˵��������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "Vibration.h"

#define VIBRATION_GPIO_PORT                  GPIOA
#define VIBRATION_GPIO_PIN                   GPIO_PIN_0
#define VIBRATION_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
/*********************************************************************************************
* ���ƣ�vibration_init()
* ���ܣ��𶯴�������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vibration_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  VIBRATION_GPIO_CLK_ENABLE();                            //��������ص�GPIO����ʱ��

  gpio_init_struct.Pin = VIBRATION_GPIO_PIN;              //ѡ��Ҫ���Ƶ�GPIO����
  gpio_init_struct.Mode = GPIO_MODE_INPUT;                //��������ģʽΪ����ģʽ
  gpio_init_struct.Pull = GPIO_PULLUP;                    //��������Ϊ����ģʽ
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        //������������
  HAL_GPIO_Init(VIBRATION_GPIO_PORT, &gpio_init_struct);  //��ʼ��GPIO����
}

/*********************************************************************************************
* ���ƣ�unsigned char get_vibration_status(void)
* ���ܣ���ȡ�𶯴�����״̬
* ��������
* ���أ�1 ��ʾ���� 
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_vibration_status(void)                     
{  
  if(HAL_GPIO_ReadPin(VIBRATION_GPIO_PORT,VIBRATION_GPIO_PIN)) //��ȡ�𶯴�������״̬
    return 0;
  else
    return 1;
}