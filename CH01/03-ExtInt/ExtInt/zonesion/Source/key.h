#ifndef __KEY_H__
#define __KEY_H__
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define KEY1_GPIO_PORT                  GPIOB
#define KEY1_GPIO_PIN                   GPIO_PIN_12
#define KEY1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* 时钟使能 */

#define KEY2_GPIO_PORT                  GPIOB
#define KEY2_GPIO_PIN                   GPIO_PIN_13
#define KEY2_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* 时钟使能 */

#define KEY3_GPIO_PORT                  GPIOB
#define KEY3_GPIO_PIN                   GPIO_PIN_14
#define KEY3_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define KEY4_GPIO_PORT                  GPIOB
#define KEY4_GPIO_PIN                   GPIO_PIN_15
#define KEY4_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN)                     /* 读取KEY1引脚 */
#define KEY2        HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN)                     /* 读取KEY2引脚 */
#define KEY3        HAL_GPIO_ReadPin(KEY3_GPIO_PORT, KEY3_GPIO_PIN)                     /* 读取KEY3引脚 */
#define KEY4        HAL_GPIO_ReadPin(KEY4_GPIO_PORT, KEY4_GPIO_PIN)                     /* 读取KEY4引脚 */

#define K1_PRES                 0x01                            //宏定义K1数字编号
#define K2_PRES                 0x02                            //宏定义K2数字编号
#define K3_PRES                 0x04                            //宏定义K3数字编号
#define K4_PRES                 0x08                            //宏定义K4数字编号

#define K1_PREESED              K1_PRES 
#define K2_PREESED              K2_PRES 
#define K3_PREESED              K3_PRES
#define K4_PREESED              K4_PRES  

/*********************************************************************************************
* 函数声明
*********************************************************************************************/
void key_init(void);                                            //按键初始化函数
char get_key_status(void);
u8 KEY_Scan(u8 mode);                                           //按键状态查询函数
#endif /*__KEY_H_*/