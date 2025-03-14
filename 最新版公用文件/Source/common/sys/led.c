/*********************************************************************************************
* �ļ�: led.c
* ���ߣ�liutong 2015.8.19
* ˵����
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "led.h"

/*********************************************************************************************
* ���ƣ�led_init
* ���ܣ���ʼ��led��Ӧ��GPIO
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void led_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  LED1_GPIO_CLK_ENABLE();                                 /* LED1ʱ��ʹ�� */
  LED2_GPIO_CLK_ENABLE();                                 /* LED2ʱ��ʹ�� */
  LED3_GPIO_CLK_ENABLE();                                 /* LED3ʱ��ʹ�� */
  LED4_GPIO_CLK_ENABLE();                                 /* LED4ʱ��ʹ�� */
  
  gpio_init_struct.Pin = LED1_GPIO_PIN;                   /* LED���� */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
  gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        /* ���� */
  HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��LED���� */

  gpio_init_struct.Pin = LED2_GPIO_PIN | LED3_GPIO_PIN | LED4_GPIO_PIN;
  HAL_GPIO_Init(LED2_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��LED���� */

  led_turn_off(LED1_NUM);
  led_turn_off(LED2_NUM);
  led_turn_off(LED3_NUM);
  led_turn_off(LED4_NUM);
}

/*********************************************************************************************
* ���ƣ�led_turn_off
* ���ܣ������Ÿߵ�ƽ���رյ�
* ������cmd
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void led_turn_off(unsigned char cmd){
  if(cmd & LED1_NUM)
    HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET);                        //�����ŵ͵�ƽ,�ر�
  if(cmd & LED2_NUM)
    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_RESET);
  if(cmd & LED3_NUM)
    HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_RESET);
  if(cmd & LED4_NUM)
    HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_GPIO_PIN, GPIO_PIN_RESET);
}

/*********************************************************************************************
* ���ƣ�led_turn_on
* ���ܣ������ŵ͵�ƽ���򿪵�
* ������cmd
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void led_turn_on(unsigned char cmd){
  if(cmd & LED1_NUM)
    HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_SET);                          //�����Ÿߵ�ƽ���򿪵�
  if(cmd & LED2_NUM)
    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_SET);
  if(cmd & LED3_NUM)
    HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_SET);
  if(cmd & LED4_NUM)
    HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_GPIO_PIN, GPIO_PIN_SET);
}

/*********************************************************************************************
* ���ƣ�led_setState
* ���ܣ����� LED ��״̬
* ������cmd ������Ƶ�LED ��
            state ָ��Ϊ�µ�״̬
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void led_setState(uint16_t cmd, unsigned char state)
{
  if(state)
  {
    if(cmd & LED1_NUM)
      led_turn_on(LED1_NUM);
    if(cmd & LED2_NUM)         
      led_turn_on(LED2_NUM);
    if(cmd & LED3_NUM)         
      led_turn_on(LED3_NUM);
    if(cmd & LED4_NUM)         
      led_turn_on(LED4_NUM);
  }
  else
  {
    if(cmd & LED1_NUM)
      led_turn_off(LED1_NUM);
    if(cmd & LED2_NUM)         
      led_turn_off(LED2_NUM);
    if(cmd & LED3_NUM)         
      led_turn_off(LED3_NUM);
    if(cmd & LED4_NUM)         
      led_turn_off(LED4_NUM);
  }
}

/*********************************************************************************************
* ���ƣ�led_ctrl
* ���ܣ�LED ����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�����λΪ1��ʾ����
*********************************************************************************************/
void led_ctrl(uint16_t cmd)
{
  if(cmd & LED1_NUM)
    led_turn_on(LED1_NUM);
  else                         
    led_turn_off(LED1_NUM);
  if(cmd & LED2_NUM)         
    led_turn_on(LED2_NUM);
  else                         
    led_turn_off(LED2_NUM);
  if(cmd & LED3_NUM)         
    led_turn_on(LED3_NUM);
  else                         
    led_turn_off(LED3_NUM);
  if(cmd & LED4_NUM)         
    led_turn_on(LED4_NUM);
  else                         
    led_turn_off(LED4_NUM);
}

/*********************************************************************************************
* ���ƣ�led_app
* ���ܣ�cycle �ⲿѭ������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ���
*********************************************************************************************/
void led_app(u16 cycle)
{
  static u16 time;time++;
  static u8 ledon;
  if(cycle>1000)cycle=1000;
  if(time>=1000/cycle){
    time=0;
    ledon=!ledon;
    if(ledon) led_setState(LED4_NUM,1);
    else led_setState(LED4_NUM,0);
  }
}

/*********************************************************************************************
* ���ƣ�led_app_ex
* ���ܣ�Ӧ�ò����ѭ��ledӦ�ó���
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ���
*********************************************************************************************/
void led_app_ex(u32 cycle)
{
  static u32 time;time++;
  static u8 ledon;
  if(time>=(1000*1000)/cycle){
    time=0;
    ledon=!ledon;
    if(ledon) led_setState(LED4_NUM,1);
    else led_setState(LED4_NUM,0);
  }
}
