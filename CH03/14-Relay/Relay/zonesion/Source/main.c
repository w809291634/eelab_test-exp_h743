/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "Relay.h"

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
  
  relay_init();                                                 //继电器初始化
  lcd_init(RELAY);                                              //LCD初始化
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
  unsigned char relay_flag = 0;
  hardware_init();
  while(1){
    if(relay_flag == 0){                                        //继电器传感器状态发生改变      
      printf("RELAY ON!\r\n");                                  //串口打印提示信息
      relay_flag = 1;                                           //更新继电器传感器状态
      relay_control(0x03);                                      //继电器的控制发生改变
      led_setState(LED4_NUM,1);
      //LCD显示继电器状态信息
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                    "继电器1开  \n继电器2开  ",LCD_WIDTH,BLACK,WHITE);
    }
    else{                                                       
      if(relay_flag == 1){                                      //继电器传感器状态发生改变
        printf("RELAY OFF!\r\n");                               //串口打印提示信息
        relay_flag = 0;                                         //更新继电器传感器状态
        relay_control(0x00);                                    //继电器的控制发生改变
        led_setState(LED4_NUM,0);
        //LCD显示继电器状态信息
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                      "继电器1关  \n继电器2关  ",LCD_WIDTH,BLACK,WHITE);
      }
    }
    
    delay_ms(1000);
  }
}  