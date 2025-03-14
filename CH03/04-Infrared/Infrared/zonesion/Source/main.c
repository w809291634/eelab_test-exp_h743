/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "Infrared.h"

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
  
  infrared_init();                                              //人体红外传感器初始化
  lcd_init(INFRARED);                                           //LCD初始化
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
  unsigned char last_status = 0;                                //存储人体红外状态变量
  unsigned count1 = 0, count2 = 0;                              //count1：系统时间计数；count2：连续无人时间计数
  char temp[100]={0};
  hardware_init();
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,"无人体活动  \nD3：关  ",LCD_WIDTH,BLACK,WHITE);
  while(1){
    if(get_infrared_status() == 1) {
      if(last_status == 0) {
        led_setState(LED3_NUM,1);                               //点亮LED灯                                
        printf("human!\r\n");                                   //串口打印提示信息
        //LCD显示人体红外传感器检测的信息
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"有人体活动  \nD3：开  ",LCD_WIDTH,BLACK,WHITE);
      }
      count2 = 0;
      last_status = 1;
    } else {
      if(last_status == 1) {
        count2 = 0;
        last_status = 2;
      } else if(last_status == 2 && count2 >= 50) {             //连续5秒没检测到人认为离开
        led_setState(LED3_NUM,0);                               //熄灭LED灯
        printf("no human!\r\n");                                //串口打印提示信息
        //LCD显示人体红外传感器检测的信息
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"无人体活动  \nD3：关  ",LCD_WIDTH,BLACK,WHITE);
        last_status = 0;
      }
      count2++;
    }
    if(count1++ >= 10) {
      count1 = 0;
      if(last_status == 0) {
        printf("no human!\r\n");
      } else {
        printf("human!\r\n");
      }
    }
    led_app(100);
    delay_ms(100);
  }
}  