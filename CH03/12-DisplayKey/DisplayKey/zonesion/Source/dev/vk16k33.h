/*********************************************************************************************
* 文件：vk16k33.h
* 作者：zonesion
* 说明：vk16k33头文件
* 修改：Chenkm 2017.01.10 修改代码格式，增加代码注释和文件说明
* 注释：
*********************************************************************************************/
#ifndef __VK16K33_H__
#define __VK16K33_H__
#include "extend.h"

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define VK16K33_ADDRESS 0x70

#define VK16K33_CMD_OSC_OFF 0x20
#define VK16K33_CMD_OSC_ON 0x21
#define VK16K33_CMD_DISPLAY_OFF 0x80
#define VK16K33_CMD_DISPLAY_ON 0x81
#define VK16K33_INT_LOW 0xA1
#define VK16K33_CMD_BRIGHT(x) (0xE0|(x&0x0f))

#define IICKEY_UP                   0x01		                    //上
#define IICKEY_LEFT                 0x02		                    //左
#define IICKEY_DOWN                 0x03		                    //下
#define IICKEY_RIGHT                0x04		                    //右
#define IICKEY_CENTER               0x05		                    //中

/*********************************************************************************************
* 外部原型函数
*********************************************************************************************/
extern char onoff;
void vk16k33_init(void);
void segment_display(unsigned int num);
void display_off(void);
void display_on(void);
unsigned char vk16k33_get_keyval(void);
#endif //__ZLG7290_H__