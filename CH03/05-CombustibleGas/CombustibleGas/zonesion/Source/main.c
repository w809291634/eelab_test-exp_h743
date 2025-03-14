/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "MP-4.h"

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
  
  combustiblegas_init();                                        //人体红外传感器初始化
  lcd_init(COMBUSTIBLEGAS);                                     //LCD初始化
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
  unsigned int combustiblegas = 0;                           	  //存储可燃气体状态变量
  char temp[100]={0};
  hardware_init();
  while(1){
    combustiblegas = get_combustiblegas_data();                 //获取可燃气体状态
    printf("combustiblegas:%d\r\n",combustiblegas);             //串口打印提示信息
    sprintf(temp,"可燃气体浓度:%5d   ",combustiblegas);         //添加可燃气体状态数据字符串到串口缓存
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);    
    
    led_app(1000);
    delay_ms(1000);
  }
}  