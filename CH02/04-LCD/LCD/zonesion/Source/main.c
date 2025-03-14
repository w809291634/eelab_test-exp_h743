/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"

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
  
  lcd_init(LCD);                                                //LCD初始化
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
  hardware_init();                                              //硬件初始化
  delay_ms(2000);
  while(1){
    //第一屏
    led_setState(LED3_NUM,0);                                   //关闭D3 
    led_setState(LED4_NUM,0);                                   //关闭D4
    LCD_Clear(WHITE);                                           //清屏
    LCDDrawFont40_Mid(0,LCD_HEIGHT/2-48,
                      "Hello IOT!",LCD_WIDTH,BLACK,WHITE); 
    POINT_COLOR = 0x07e0;
    LCD_DrawLine(0, LCD_HEIGHT/2, LCD_WIDTH, LCD_HEIGHT/2);     //画线
    LCDDrawFont40_Mid(0,LCD_HEIGHT/2+8,
                      "物联网开放平台!",LCD_WIDTH,BLACK,WHITE);
    //第二屏
    delay_ms(2000);
    led_setState(LED3_NUM,1);                                   //点亮D3
    led_setState(LED4_NUM,1);                                   //点亮D4
    LCD_Clear(WHITE);                                           //清屏
    LCDDrawFont40_Mid(0, LCD_HEIGHT/2-20, 
                      "STM32H743ZIT6",LCD_WIDTH,BLACK, WHITE);  //显示ASCII
    delay_ms(2000); 
  }
}  
