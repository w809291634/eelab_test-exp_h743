/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "key.h"
#include "rgb.h"
#include "iwdg.h"
#include "led.h"

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
  key_init();                                                   //按键初始化
  led_init();                                                   //led灯初始化
  rgb_init();                                                   //RGB灯初始化
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
  hardware_init();
  delay_count(20000);                                           //延时
  wdg_init(IWDG_PRESCALER_32,1000);                             //由LSI（默认32KHZ）驱动，预分频数32，重载值为1000，溢出时间为1S
  turn_on(LEDR);                                                //R灯开
  while(1){
    if(get_key_status()==K3_PREESED){                           //判断K3按键是否被按下 
      wdg_feed();                                               //喂狗程序
    }
    delay_count(500);                                           //延时
    turn_on(LEDG);                                              //G灯开
    delay_count(500);                                           //延时
    turn_off(LEDG);                                             //G灯关
  }
}  
