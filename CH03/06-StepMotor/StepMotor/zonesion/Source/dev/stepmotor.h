/*********************************************************************************************
* 文件：stepmotor.h
* 作者：Lixm 2017.10.17
* 说明：步进电机驱动代码头文件  
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 宏条件编译
*********************************************************************************************/
#ifndef __STEPMOTOR_H__
#define __STEPMOTOR_H__

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"

/*********************************************************************************************
* 内部原型函数
*********************************************************************************************/
void stepmotor_init(void);                                      //步进电机传感器初始化
void stepmotor_control(unsigned char cmd);                     //步进电机传感器控制

#endif /*__STEPMOTOR_H__*/

