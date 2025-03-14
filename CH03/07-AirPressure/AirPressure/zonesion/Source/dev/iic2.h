/*********************************************************************************************
* 文件：iic.h
* 作者：zonesion
* 说明：iic头文件
* 修改：
* 注释：
*********************************************************************************************/
#ifndef _IIC2_H_
#define _IIC2_H_
#include "extend.h"
#include "delay.h"

/*********************************************************************************************
* 外部原型函数
*********************************************************************************************/
void iic_delay_us(unsigned int i);
void iic_init(void);
void iic_start(void);
void iic_stop(void);
void iic_send_ack(char ack);
unsigned char iic_recv_ack(void);
unsigned char iic_write_byte(unsigned char data);
unsigned char iic_read_byte(unsigned char ack);
void delay(unsigned int t);

#endif 