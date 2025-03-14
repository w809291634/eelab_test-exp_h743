/*********************************************************************************************
* 文件: LED.H
* 作者：liutong 2015.8.19
* 说明：
* 修改：
* 注释：
*********************************************************************************************/
#ifndef _LED_H_
#define _LED_H_
/*********************************************************************************************
*头文件
**********************************************************************************************/
#include "extend.h"
/*********************************************************************************************
*宏定义
**********************************************************************************************/
/******************************************************************************************/
/* 引脚 定义 */
#define LED1_GPIO_PORT                  GPIOG
#define LED1_GPIO_PIN                   GPIO_PIN_0
#define LED1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)

#define LED2_GPIO_PORT                  GPIOF
#define LED2_GPIO_PIN                   GPIO_PIN_15
#define LED2_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)

#define LED3_GPIO_PORT                  GPIOF
#define LED3_GPIO_PIN                   GPIO_PIN_14
#define LED3_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)

#define LED4_GPIO_PORT                  GPIOF
#define LED4_GPIO_PIN                   GPIO_PIN_13
#define LED4_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)

/* LED端口定义 */
#define LED1(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)    

#define LED2(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)  

#define LED3(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)  

#define LED4(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)  
/* LED取反定义 */
#define LED1_TOGGLE()    do{ HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN); }while(0)   /* LED1 = !LED1 */
#define LED2_TOGGLE()    do{ HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_GPIO_PIN); }while(0)   /* LED2 = !LED2 */
#define LED3_TOGGLE()    do{ HAL_GPIO_TogglePin(LED3_GPIO_PORT, LED3_GPIO_PIN); }while(0)   /* LED3 = !LED3 */
#define LED4_TOGGLE()    do{ HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_GPIO_PIN); }while(0)   /* LED4 = !LED4 */

#define LED1_NUM              0x001
#define LED2_NUM              0x002
#define LED3_NUM              0x004
#define LED4_NUM              0x008

/*********************************************************************************************
*函数声明
**********************************************************************************************/
void led_init(void);                                            //LED引脚初始化
void led_turn_on(unsigned char rgb);                            //开LED函数
void led_turn_off(unsigned char rgb);                           //关LED函数
void led_ctrl(uint16_t cmd);
#define led_control(x)        led_ctrl(x)                       //函数的另外一种名称
void led_setState(uint16_t cmd, unsigned char state);
unsigned char get_led_status(void);                             //获取LED当前的状态
void led_app(u16 cycle);
void led_app_ex(u32 cycle);
#endif 

