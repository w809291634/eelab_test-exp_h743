/*********************************************************************************************
* �ļ�: rgb.c
* ���ߣ�liutong 2015.8.19
* ˵����
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#ifndef _RGB_H_
#define _RGB_H_
/*********************************************************************************************
*ͷ�ļ�
**********************************************************************************************/
#include "extend.h"

/*********************************************************************************************
* ָ�����ļ����������ò���
*********************************************************************************************/
// RGB_R--PG8  RGB_G--PG7  RGB_B--PG6  
#define LEDR_GPIO_PORT                  GPIOG
#define LEDR_GPIO_PIN                   GPIO_PIN_8
#define LEDR_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)

#define LEDG_GPIO_PORT                  GPIOG
#define LEDG_GPIO_PIN                   GPIO_PIN_7
#define LEDG_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)

#define LEDB_GPIO_PORT                  GPIOG
#define LEDB_GPIO_PIN                   GPIO_PIN_6
#define LEDB_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)

/* LED�˿ڶ��� */
#define _LEDR(x)   do{ x ? \
                      HAL_GPIO_WritePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)    

#define _LEDG(x)   do{ x ? \
                      HAL_GPIO_WritePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)  

#define _LEDB(x)   do{ x ? \
                      HAL_GPIO_WritePin(LEDB_GPIO_PORT, LEDB_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LEDB_GPIO_PORT, LEDB_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)  

/* LEDȡ������ */
#define LEDR_TOGGLE()    do{ HAL_GPIO_TogglePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN); }while(0)   /* LEDR = !LEDR */
#define LEDG_TOGGLE()    do{ HAL_GPIO_TogglePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN); }while(0)   /* LEDG = !LEDG */
#define LEDB_TOGGLE()    do{ HAL_GPIO_TogglePin(LEDB_GPIO_PORT, LEDB_GPIO_PIN); }while(0)   /* LEDB = !LEDB */

/*********************************************************************************************
*�궨��
**********************************************************************************************/
#define LEDR    0X10                                            //�궨�������ֱ��
#define LEDG    0X20                                            //�궨���̵����ֱ��
#define LEDB    0X40                                            //�궨���������ֱ��

/*********************************************************************************************
*��������
**********************************************************************************************/
void rgb_init(void);                                            //LED���ų�ʼ��
void turn_on(unsigned char rgb);                                //��LED����
void turn_off(unsigned char rgb);                               //��LED����
unsigned char get_rgb_status(void);                             //��ȡLED��ǰ��״̬
void rgb_ctrl(uint8_t cfg);                                     //rgb���ƺ���
#endif 

