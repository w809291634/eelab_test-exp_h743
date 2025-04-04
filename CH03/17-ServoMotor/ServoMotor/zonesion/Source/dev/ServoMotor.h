/*********************************************************************************************
* 文件：ServoMotor.h
* 作者：Lixm 2017.10.17
* 说明：驱动代码头文件  
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 宏条件编译
*********************************************************************************************/
#ifndef __SERVOMOTOR_H__
#define __SERVOMOTOR_H__

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"

/*********************************************************************************************
* 内部原型函数
*********************************************************************************************/
void ServoMotor_init(u32 arr,u32 psc);
void Servo_control(u8 angle);

#endif /*__INFRARED_H__*/

