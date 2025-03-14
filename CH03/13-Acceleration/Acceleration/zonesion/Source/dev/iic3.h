#ifndef _iic3_
#define _iic3_
#include "extend.h"
#include "delay.h"

//I2C
// SCL--PF9   SDA--PF10
#define I2C_SCL_CLK_ENABLE()      do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)
#define I2C_SCL_GPIO            GPIOF
#define I2C_SCL_PIN             GPIO_PIN_9

#define I2C_SDA_CLK_ENABLE()      do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)
#define I2C_SDA_GPIO            GPIOF
#define I2C_SDA_PIN             GPIO_PIN_10

#define I2C_SDA_PIN_NUM 10

#define SDA_IN          do{I2C_SDA_GPIO->MODER &= ~(3<<(I2C_SDA_PIN_NUM*2)); I2C_SDA_GPIO->MODER |= (0<<(I2C_SDA_PIN_NUM*2));}while(0)
#define SDA_OUT         do{I2C_SDA_GPIO->MODER &= ~(3<<(I2C_SDA_PIN_NUM*2)); I2C_SDA_GPIO->MODER |= (1<<(I2C_SDA_PIN_NUM*2));}while(0)

#define SCL_L           HAL_GPIO_WritePin(I2C_SCL_GPIO, I2C_SCL_PIN, GPIO_PIN_RESET)
#define SCL_H           HAL_GPIO_WritePin(I2C_SCL_GPIO, I2C_SCL_PIN, GPIO_PIN_SET)

#define SDA_L           HAL_GPIO_WritePin(I2C_SDA_GPIO, I2C_SDA_PIN, GPIO_PIN_RESET)
#define SDA_H           HAL_GPIO_WritePin(I2C_SDA_GPIO, I2C_SDA_PIN, GPIO_PIN_SET)

#define SDA_R           HAL_GPIO_ReadPin(I2C_SDA_GPIO, I2C_SDA_PIN)
#define IIC3_DelayUs(x) delay_us(x)

void I2C3_GPIOInit(void);
void I2C3_Start(void);
void I2C3_Stop(void);
void I2C3_Ack(void);
void I2C3_NoAck(void);
int I2C3_WaitAck(void);
void I2C3_WriteByte(uint8_t SendByte); //数据从高位到低位
uint8_t I2C3_ReadByte(void);

#endif