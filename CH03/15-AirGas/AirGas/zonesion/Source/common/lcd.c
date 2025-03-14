/*********************************************************************************************
* 文件：lcd.c
* 作者：zonesion 2017.02.17
* 说明：LCD显示基本实验信息程序
* 修改：Chenkm 2017.02.17 添加注释
* 注释：
*
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "lcd.h"
#include "stdio.h"
#include "text.h"

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
char* experiment_name[] = {
"UART通信实验",
"IIC通信实验",
"SPI通信实验",
"LCD显示实验",
"OLED显示实验",
"触摸屏触控采集实验",
"摄像头实验",
"","","",
"温湿度采集实验",
"光照度采集实验",
"风扇控制实验",
"人体红外传感器实验",
"可燃气体传感器实验",
"步进电机控制实验",
"气压海拔传感器实验",
"门锁控制实验",
"火焰传感器实验",
"振动传感器实验",
"直流电机与测速实验",
"数码管与按键实验",
"三轴传感器实验",
"继电器控制实验",
"空气质量传感器实验",
"霍尔传感器实验",
"舵机控制实验",
"光栅传感器实验",
};

char* experiment_description[] = {
"上位机通过串口将数据发给单片机，串口调试助手软件和LCD屏幕上都会显示发送的信息。",
"通过IIC总线操作pcf8563，实时读取实时时间，通过触发K1、K2、k3按键调整设定时间的分、时、天，通过k4按键将实际时间设置为设定时间。",
"通过使用硬件SPI读Flash的ID，通过K1、K2按键进行flash的读写测试。",
"使用FSMC驱动LCD液晶屏，并在屏幕上显示文字。",
"使用IIC驱动OLED显示屏，并在屏幕上显示文字。",
"通过触摸LCD屏幕，每次采集的所有触控信息将会反馈到LCD上进行显示，支持多点触控",
"通过DCMI接口捕获OV2640摄像头视频数据，摄像头初始化成功后，点击任意按键进入摄像头测试界面。",
"","","",
"通过IIC总线读取温湿度传感器原始数据,串口每秒打印一次数据。",
"使用光照度传感器读取当前环境光照度数据,串口每秒打印一次数据。",
"通过按键K1控制风扇开关，串口打印风扇状态。",
"通过人体红外传感器检测人体活动状况,串口每秒打印一次数据。",
"通过可燃气体传感器获取当前环境燃气浓度，串口每秒打印一次数据。",    
"按下K1步进电机正转，按下K2步进电机反转。",
"通过大气压力传感器获取温度值、大气压力值与海拔值,串口每秒打印一次数据。",
"通过按键K1控制门锁开关，串口打印门锁状态。",
"通过火焰传感器采集火焰信号，并将火焰传感器的检测结果打印在PC上，1S打印一次",
"通过振动传感器检测振动，并在状态发生变化时串口打印提示信息。",
"通过K1、K2按键控制直流电机正反转，观察电机运行的实际转速，串口打印信息",
"通过按钮来控制数码管数值的增减，每次按键出现变化时打印一次信息",
"通过三轴加速度传感器对三轴重力进行采集，并将采集信息打印在PC上，1S打印一次",
"继电器周期性开合，串口打印其状态。",
"通过空气质量传感器采集空气质量信息，并将空气质量采集信息打印在PC上，1S更新一次",
"通过霍尔传感器检测磁场信号，状态发生变化时，串口打印提示信息。",
"通过K1和K2按键控制舵机运行到不同的角度，每次舵机角度变化串口打印提示信息",
"通过光栅传感器检测光栅内有无物体，当状态发生变化时，串口打印提示信息。",
};                      
                                   
/*********************************************************************************************
* 名称:lcd_init()
* 功能:LCD初始化并打印实验基本信息
* 参数:name -- 实验名称
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void lcd_init(unsigned char name)
{
  TFTLCD_Init();
  font_lib_init();
  LCD_Clear(0xFFFF);
  LCD_Fill(0, 0, LCD_WIDTH, 60,0x4596);
  LCD_Fill(0, LCD_HEIGHT-60, LCD_WIDTH, 60,0x4596);
  LCDDrawFont40_Mid(0,  10,  experiment_name[name-1],LCD_WIDTH,WHITE,0x4596);
  LCDDrawFont40_Mid(0,  LCD_HEIGHT-50, "嵌入式接口技术",LCD_WIDTH, WHITE, 0x4596);
  LCDShowFont32(8,      65,"实验描述：", LCD_WIDTH, 0x4596,WHITE);
  LCDShowFont32(8+32*2, 65+32+SPACING,experiment_description[name-1],LCD_WIDTH-(8+32*2)-20, 0x0000,WHITE);
  LCDShowFont32(8,      REF_POS,"实验现象：", LCD_WIDTH,0x4596,WHITE);
}
