#ifndef __KEY_H__
#define __KEY_H__
/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"

/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define KEY1_GPIO_PORT                  GPIOB
#define KEY1_GPIO_PIN                   GPIO_PIN_12
#define KEY1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* ʱ��ʹ�� */

#define KEY2_GPIO_PORT                  GPIOB
#define KEY2_GPIO_PIN                   GPIO_PIN_13
#define KEY2_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* ʱ��ʹ�� */

#define KEY3_GPIO_PORT                  GPIOB
#define KEY3_GPIO_PIN                   GPIO_PIN_14
#define KEY3_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define KEY4_GPIO_PORT                  GPIOB
#define KEY4_GPIO_PIN                   GPIO_PIN_15
#define KEY4_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN)                     /* ��ȡKEY1���� */
#define KEY2        HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN)                     /* ��ȡKEY2���� */
#define KEY3        HAL_GPIO_ReadPin(KEY3_GPIO_PORT, KEY3_GPIO_PIN)                     /* ��ȡKEY3���� */
#define KEY4        HAL_GPIO_ReadPin(KEY4_GPIO_PORT, KEY4_GPIO_PIN)                     /* ��ȡKEY4���� */

#define K1_PRES                 0x01                            //�궨��K1���ֱ��
#define K2_PRES                 0x02                            //�궨��K2���ֱ��
#define K3_PRES                 0x04                            //�궨��K3���ֱ��
#define K4_PRES                 0x08                            //�궨��K4���ֱ��

#define K1_PREESED              K1_PRES 
#define K2_PREESED              K2_PRES 
#define K3_PREESED              K3_PRES
#define K4_PREESED              K4_PRES  

/*********************************************************************************************
* ��������
*********************************************************************************************/
void key_init(void);                                            //������ʼ������
char get_key_status(void);
u8 KEY_Scan(u8 mode);                                           //����״̬��ѯ����
#endif /*__KEY_H_*/