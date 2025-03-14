/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "key.h"
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
  key_init();                                                   //按键初始化
  rgb_init();                                                   //RGB灯初始化
  led_init();                                                   //LED灯初始化
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
  char status = 0;                                              //声明一个表示状态的变量
  hardware_init();
  while(1){
    u8 key=KEY_Scan(0);                                         //按键键值扫描
    if(key){
      status++;                                                 //状态变量加一
      if (status>3)                                             //状态变量最大为3
        status=0;                                               //状态变量置零
    }

    switch(status){    
      case 0: 
        turn_off(LEDR);                                          //关闭LEDR
        turn_off(LEDG);                                          //关闭LEDG
        turn_off(LEDB);                                          //关闭LEDB
        led_turn_off(LED1_NUM);                                  //关闭LED1
        led_turn_off(LED2_NUM);                                  //关闭LED2
        led_turn_off(LED3_NUM);                                  //关闭LED3
        break;
      case 1:turn_on(LEDR);led_turn_on(LED1_NUM);break;         //点亮LEDR和LED1
      case 2:turn_on(LEDG);led_turn_on(LED2_NUM);break;         //点亮LEDG和LED2
      case 3:turn_on(LEDB);led_turn_on(LED3_NUM);break;         //点亮LEDB和LED3
      default:status=0;break;                                   //status清零
    }
  }
}  
