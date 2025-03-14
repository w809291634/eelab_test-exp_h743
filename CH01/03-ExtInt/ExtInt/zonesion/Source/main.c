/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "exti.h"
#include "rgb.h"
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
  rgb_init();                                                   //RGB灯初始化
  led_init();                                                   //led灯初始化
  exti_init();                                                  //初始化按键检测管脚
}

/*********************************************************************************************
* 名称：main()
* 功能：主函数
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
char rgb_status = 0;                                            //声明一个表示RGB状态的变量
int main(void)
{
  hardware_init();
  while(1){
    if(rgb_status == 0){                                        //RGB灯为状态0
      turn_on(LEDR);                                            //R灯开
      led_turn_on(LED1_NUM);                                    //LED1灯开
      turn_off(LEDG);                                           //B灯关
      led_turn_off(LED2_NUM);                                   //LED2灯关
    }
    else{                                                       //RGB灯为状态1
      turn_off(LEDR);                                           //R灯关
      led_turn_off(LED1_NUM);                                   //LED1灯关
      turn_on(LEDG);                                            //B灯开
      led_turn_on(LED2_NUM);                                    //LED2灯开
    }
  }
}  
