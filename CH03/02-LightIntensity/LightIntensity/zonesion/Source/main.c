/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "BH1750.h"

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
  
  bh1750_init();                                                //光照度传感器初始化
  lcd_init(LIGHTINTENSITY);                                     //LCD初始化
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
  float light_data = 0;                                         //存储光照度数据变量
  char temp[100]={0};
  hardware_init();
  while(1){
    light_data = bh1750_get_data();                             //获取光照数据
    printf("light_data:%.2f\r\n",light_data);                   //将光照度数据通过串口打印出来
    sprintf(temp,"光照：%.2fLux    ",light_data);               //将光照度数据通过LCD屏显示出来
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);    
    
    led_app(1000);
    delay_ms(1000);
  }
}  