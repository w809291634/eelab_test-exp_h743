/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "Flame.h"

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
  
  flame_init();                                                 //火焰传感器初始化
  lcd_init(FLAME);                                              //LCD初始化
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
  unsigned char lastState = 0;                                  //存储火焰状态变量
  unsigned char count = 0;
  hardware_init();
  if(get_flame_status())
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,"检测到火焰  ",LCD_WIDTH,BLACK,WHITE); 
  else
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,"未检测到火焰  ",LCD_WIDTH,BLACK,WHITE); 
  while(1){
    if(get_flame_status()){                                     //检测到火焰
      if(lastState == 0)
      {
        lastState = 1;
        printf("fire!\r\n");                                    //串口打印提示信息
        //LCD更新数据
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"检测到火焰  ",LCD_WIDTH,BLACK,WHITE); 
      }
    }
    else{                                                       //没有检测到火焰
      if(lastState == 1)
      {
        lastState = 0;
        printf("no fire!\r\n");                                 //串口打印提示信息
        //LCD更新数据
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"未检测到火焰  ",LCD_WIDTH,BLACK,WHITE); 
      }
    }
    if(count++ >= 10)
    {
      count = 0;
      if(lastState == 0)
        printf("no fire!\r\n");                                 //串口打印提示信息
      else
        printf("fire!\r\n");                                    //串口打印提示信息
    }
    if(lastState == 0)                                          //根据LED灯的状态控制LED灯的变化
      led_setState(LED3_NUM,0);                                 //关闭LED灯
    else                                    
      led_setState(LED3_NUM,1);                                 //点亮LED灯

    led_app(100);
    delay_ms(100);
  }
}  