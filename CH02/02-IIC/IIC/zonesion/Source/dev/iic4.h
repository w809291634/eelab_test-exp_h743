#ifndef __I2C4_H_
#define __I2C4_H_
#include "extend.h"
#include "delay.h"
/*****  I2C4  *****/
/* 引脚 定义 */
// SCL---PC8  SDA---PC9
#define I2C4_SCL_GPIO_PORT               GPIOC
#define I2C4_SCL_GPIO_PIN                GPIO_PIN_8
#define I2C4_SCL_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* 时钟使能 */

#define I2C4_SDA_GPIO_PORT               GPIOC
#define I2C4_SDA_GPIO_PIN                GPIO_PIN_9
#define I2C4_SDA_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* 时钟使能 */

#define I2C4_SDA_NUM         9

/* IO操作 */
#define I2C4_SCL(x)         do{ x ? \
                              HAL_GPIO_WritePin(I2C4_SCL_GPIO_PORT, I2C4_SCL_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(I2C4_SCL_GPIO_PORT, I2C4_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)       /* SCL */

#define I2C4_SDA(x)         do{ x ? \
                              HAL_GPIO_WritePin(I2C4_SDA_GPIO_PORT, I2C4_SDA_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(I2C4_SDA_GPIO_PORT, I2C4_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)       /* SDA */

//IO方向设置
#define I2C4_SDA_IN          do{I2C4_SDA_GPIO_PORT->MODER &= ~(3<<I2C4_SDA_NUM*2); I2C4_SDA_GPIO_PORT->MODER |= (0<<I2C4_SDA_NUM*2);}while(0)
#define I2C4_SDA_OUT         do{I2C4_SDA_GPIO_PORT->MODER &= ~(3<<I2C4_SDA_NUM*2); I2C4_SDA_GPIO_PORT->MODER |= (1<<I2C4_SDA_NUM*2);}while(0)

//IO操作函数
#define I2C4_SCL_L           I2C4_SCL(0)
#define I2C4_SCL_H           I2C4_SCL(1)

#define I2C4_SDA_L           I2C4_SDA(0)
#define I2C4_SDA_H           I2C4_SDA(1)

#define I2C4_SDA_R           HAL_GPIO_ReadPin(I2C4_SDA_GPIO_PORT, I2C4_SDA_GPIO_PIN)

#define I2C4_Delay(delay)    delay_us(delay);

void I2C4_IO_Init(void);
void I2C4_Start(void);
void I2C4_Stop(void);
unsigned char I2C4_WaitAck(void);
void I2C4_Ack(void);
void I2C4_NAck(void);
void I2C4_WriteByte(unsigned char data);
unsigned char I2C4_ReadByte(unsigned char ack);

#endif
