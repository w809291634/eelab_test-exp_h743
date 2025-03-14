#ifndef __SCCB_H
#define __SCCB_H
#include "extend.h"

/* 引脚 定义 */
// SCL---PG4  SDA---PG3
#define SCCB_SCL_GPIO_PORT               GPIOG
#define SCCB_SCL_GPIO_PIN                GPIO_PIN_4
#define SCCB_SCL_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)   /* 时钟使能 */

#define SCCB_SDA_GPIO_PORT               GPIOG
#define SCCB_SDA_GPIO_PIN                GPIO_PIN_3
#define SCCB_SDA_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)   /* 时钟使能 */

/* IO操作 */
#define SCCB_SCL(x)         do{ x ? \
                              HAL_GPIO_WritePin(SCCB_SCL_GPIO_PORT, SCCB_SCL_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(SCCB_SCL_GPIO_PORT, SCCB_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)       /* SCL */

#define SCCB_SDA(x)         do{ x ? \
                              HAL_GPIO_WritePin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)       /* SDA */

#define SCCB_SDA_NUM         3
#define SCCB_ID              0X60          //OV2640的ID

//IO方向设置
#define SCCB_SDA_IN          do{SCCB_SDA_GPIO_PORT->MODER &= ~(3<<SCCB_SDA_NUM*2); SCCB_SDA_GPIO_PORT->MODER |= (0<<SCCB_SDA_NUM*2);}while(0)
#define SCCB_SDA_OUT         do{SCCB_SDA_GPIO_PORT->MODER &= ~(3<<SCCB_SDA_NUM*2); SCCB_SDA_GPIO_PORT->MODER |= (1<<SCCB_SDA_NUM*2);}while(0)

//IO操作函数
#define SCCB_SCL_L           SCCB_SCL(0)
#define SCCB_SCL_H           SCCB_SCL(1)

#define SCCB_SDA_L           SCCB_SDA(0)
#define SCCB_SDA_H           SCCB_SDA(1)

#define SCCB_SDA_R           HAL_GPIO_ReadPin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN)

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void);
u8 SCCB_WR_Reg(u8 reg,u8 data);
u8 SCCB_RD_Reg(u8 reg);
#endif
