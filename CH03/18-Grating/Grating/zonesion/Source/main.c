/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "grating.h"

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
  
  grating_init();                                               //红外光栅传感器初始化
  lcd_init(GRATING);                                            //LCD初始化
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
  unsigned char lastState = 0;                                  // 光栅上一次状态
  unsigned char num = 0;
  hardware_init();
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                "未检测到遮挡  \nD3：关  ",LCD_WIDTH,BLACK,WHITE);
  while(1){
    if(get_grating_status() == 1){                              //检测到遮挡
      if(lastState == 0)
      {
        led_setState(LED3_NUM,1);                               //点亮LED灯
        printf("Grating!\r\n");                                 //串口打印提示信息
        //LCD显示检测信息 
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                      "检测到遮挡  \nD3：开  ",LCD_WIDTH,BLACK,WHITE);
        lastState = 1;                                          //记录状态
        num = 0;                                                //计数清0
      }
    }
    else{                                                       //没有检测到遮挡
      num ++;                                                   //计数自增
      if(num >= 30)                                             //检测遮挡
      {
        num = 0;                                                //计数清0
        printf("no Grating!\r\n");                              //串口打印提示信息
      }
      if(lastState == 1)
      {
        //LCD显示检测信息
        led_setState(LED3_NUM,0);                               //熄灭LED灯
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                      "未检测到遮挡  \nD3：关  ",LCD_WIDTH,BLACK,WHITE);
        printf("no Grating!\r\n");                              //串口打印提示信息
        lastState = 0;
      }
    }
    
    led_app(100);
    delay_ms(100);                                              //延时100毫秒
  }
}  