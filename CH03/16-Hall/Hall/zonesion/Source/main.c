/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "hall.h"

/*********************************************************************************************
* 名称：hardware_init()
* 功能：硬件初始化
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
static void hardware_init(void)
{
  sys_cache_enable();                                           //打开L1-Cache 强制D-Cache透写*/
  HAL_Init();                                                   //初始化HAL库 */
  SystemClock_Config();                                         //设置时钟, 400Mhz */
  PeriphCommonClock_Config();                                   //外设特殊时钟初始化
  delay_init(400);                                              //延时初始化
  usart_init(115200);                                           //串口初始化
  led_init();                                                   //LED初始化
  key_init();                                                   //按键初始化
  
  hall_init();                                                  //霍尔传感器初始化
  lcd_init(HALL);                                               //LCD初始化
}

/*********************************************************************************************
* 名称：main()
* 功能：主函数
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
int main(void)
{
  unsigned char hall_status = 0;                                //存储霍尔状态变量
  hardware_init();
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                "未检测到磁场  \nD3：关  ",LCD_WIDTH,BLACK,WHITE);
  while(1){
    if(get_hall_status() == 1){                                 //检测到磁场
      if(hall_status == 0){                                     //霍尔传感器状态发生改变
        led_setState(LED3_NUM,1);;                              //点亮LED灯
        printf("hall!\r\n");                                    //串口打印提示信息
        hall_status = 1;                                        //更新振动传感器状态
        //LCD显示信息
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                      "检测到磁场  \nD3：开  ",LCD_WIDTH,BLACK,WHITE);
      }
    }
    else{                                                       //没有检测到磁场
      if(hall_status == 1){                                     //关闭LED灯
        led_setState(LED3_NUM,0);;                              //熄灭LED灯
        printf("no hall!\r\n");                                 //串口打印提示信息
        hall_status = 0;                                        //更新霍尔传感器状态
        //LCD显示信息
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                      "未检测到磁场  \nD3：关  ",LCD_WIDTH,BLACK,WHITE);
      }
    }
    
    led_app_ex(10);
    delay_us(10);                                               //延时10微秒
  }
}  