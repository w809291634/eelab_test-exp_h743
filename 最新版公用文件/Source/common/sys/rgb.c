/*********************************************************************************************
* �ļ�: led.c
* ���ߣ�liutong 2015.8.19
* ˵����
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "rgb.h"

/*********************************************************************************************
* ���ƣ�rgb_init
* ���ܣ���ʼ��RGB��Ӧ��GPIO
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void rgb_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  LEDR_GPIO_CLK_ENABLE();                                 /* LEDRʱ��ʹ�� */
  LEDG_GPIO_CLK_ENABLE();                                 /* LEDGʱ��ʹ�� */
  LEDB_GPIO_CLK_ENABLE();                                 /* LEDBʱ��ʹ�� */
  
  gpio_init_struct.Pin = LEDR_GPIO_PIN;                   /* LED���� */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
  gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        /* ���� */
  HAL_GPIO_Init(LEDR_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��LED���� */

  gpio_init_struct.Pin = LEDG_GPIO_PIN ;
  HAL_GPIO_Init(LEDG_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��LED���� */
  
  gpio_init_struct.Pin = LEDB_GPIO_PIN ;
  HAL_GPIO_Init(LEDB_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��LED���� */
  
  turn_off(LEDR);
  turn_off(LEDG);
  turn_off(LEDB);
}

/*********************************************************************************************
* ���ƣ�turn_off
* ���ܣ������Ÿߵ�ƽ���رյ�
* ������rgb
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void turn_off(unsigned char rgb){
  if(rgb & LEDR)
    HAL_GPIO_WritePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN, GPIO_PIN_SET);        //�����Ÿߵ�ƽ,�ر�
  if(rgb & LEDG)
    HAL_GPIO_WritePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN, GPIO_PIN_SET);
  if(rgb & LEDB)
    HAL_GPIO_WritePin(LEDB_GPIO_PORT, LEDB_GPIO_PIN, GPIO_PIN_SET);
}

/*********************************************************************************************
* ���ƣ�turn_on
* ���ܣ������ŵ͵�ƽ���򿪵�
* ������rgb
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void turn_on(unsigned char rgb){
  if(rgb & LEDR)
    HAL_GPIO_WritePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN, GPIO_PIN_RESET);     //�����ŵ͵�ƽ���򿪵�
  if(rgb & LEDG)
    HAL_GPIO_WritePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN, GPIO_PIN_RESET);
  if(rgb & LEDB)
    HAL_GPIO_WritePin(LEDB_GPIO_PORT, LEDB_GPIO_PIN, GPIO_PIN_RESET);
}

/*********************************************************************************************
* ���ƣ�get_rgb_status
* ���ܣ���ȡ�Ƶ�״̬
* ������
* ���أ�status����λ�ֱ��ʾ�Ƶ�״̬
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_rgb_status(void){
  unsigned char status = 0;
  if(HAL_GPIO_ReadPin(LEDR_GPIO_PORT, LEDR_GPIO_PIN))
    status |= LEDR;
  else
    status &= ~LEDR;
  
  if(HAL_GPIO_ReadPin(LEDG_GPIO_PORT, LEDG_GPIO_PIN))
    status |= LEDG; 
  else
    status &= ~LEDG;
  
  if(HAL_GPIO_ReadPin(LEDB_GPIO_PORT, LEDB_GPIO_PIN))
    status |= LEDB;
  else
    status &= ~LEDB;
  return status;                                                //����status
}

/*********************************************************************************************
* ���ƣ�void rgb_ctrl(uint8_t cfg)
* ���ܣ�RGB���ƺ���
* ������cfg:RGB�Ŀ���λ������λ�ֱ����һյ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void rgb_ctrl(uint8_t cfg)
{
  uint8_t set1, set2, set3;
  set1 = cfg & 0x01;
  set2 = (cfg & 0x02) >> 1;
  set3 = (cfg & 0x04) >> 2;
  
  _LEDR(set1);
  _LEDG(set2);
  _LEDB(set3);
}
