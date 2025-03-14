/*********************************************************************************************
* 文件: rgb.c
* 作者：liutong 2015.8.19
* 说明：
* 修改：
* 注释：
*********************************************************************************************/
#ifndef _RGB_H_
#define _RGB_H_
/*********************************************************************************************
*头文件
**********************************************************************************************/
#include "extend.h"

/*********************************************************************************************
* 指定本文件的引脚配置参数
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

/* LED端口定义 */
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

/* LED取反定义 */
#define LEDR_TOGGLE()    do{ HAL_GPIO_TogglePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN); }while(0)   /* LEDR = !LEDR */
#define LEDG_TOGGLE()    do{ HAL_GPIO_TogglePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN); }while(0)   /* LEDG = !LEDG */
#define LEDB_TOGGLE()    do{ HAL_GPIO_TogglePin(LEDB_GPIO_PORT, LEDB_GPIO_PIN); }while(0)   /* LEDB = !LEDB */

/*********************************************************************************************
*宏定义
**********************************************************************************************/
#define LEDR    0X10                                            //宏定义红灯数字编号
#define LEDG    0X20                                            //宏定义绿灯数字编号
#define LEDB    0X40                                            //宏定义蓝灯数字编号

/*********************************************************************************************
*函数声明
**********************************************************************************************/
void rgb_init(void);                                            //LED引脚初始化
void turn_on(unsigned char rgb);                                //开LED函数
void turn_off(unsigned char rgb);                               //关LED函数
unsigned char get_rgb_status(void);                             //获取LED当前的状态
void rgb_ctrl(uint8_t cfg);                                     //rgb控制函数
#endif 

