/*********************************************************************************************
* �ļ���Relay.c
* ���ߣ�Lixm 2017.10.17
* ˵�����̵�����������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "Relay.h"

/*********************************************************************************************
* ���ƣ�relay_init()
* ���ܣ��̵�����������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void relay_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  RELAY1_GPIO_CLK_ENABLE();                                     /* ʱ��ʹ�� */
  RELAY2_GPIO_CLK_ENABLE();                                     /* ʱ��ʹ�� */
  
  gpio_init_struct.Pin = RELAY1_GPIO_PIN;                       /* ���� */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                  /* ������� */
  gpio_init_struct.Pull = GPIO_PULLDOWN;                        /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;              /* ���� */
  HAL_GPIO_Init(RELAY1_GPIO_PORT, &gpio_init_struct);           /* ��ʼ������ */

  gpio_init_struct.Pin = RELAY2_GPIO_PIN;
  HAL_GPIO_Init(RELAY2_GPIO_PORT, &gpio_init_struct);           /* ��ʼ������ */
  
  relay_control(0x00);
}

/*********************************************************************************************
* ���ƣ�void relay_control(unsigned char cmd)
* ���ܣ��̵�����������
* ��������������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void relay_control(unsigned char cmd)
{ 
  if(cmd & 0x01)
    HAL_GPIO_WritePin(RELAY1_GPIO_PORT, RELAY1_GPIO_PIN, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(RELAY1_GPIO_PORT, RELAY1_GPIO_PIN, GPIO_PIN_RESET);
  if(cmd & 0x02)
    HAL_GPIO_WritePin(RELAY2_GPIO_PORT, RELAY2_GPIO_PIN, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(RELAY2_GPIO_PORT, RELAY2_GPIO_PIN, GPIO_PIN_RESET);
}