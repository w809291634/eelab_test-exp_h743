/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "vk16k33.h"

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
  
  vk16k33_init();                                               //初始化vk16k33
  lcd_init(DISPLAYKEY);                                         //LCD初始化
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
  s32 num=0;
  u32 last_num=0;
  char buf[100]={0};
  hardware_init();
  while(1){
    u8 key=vk16k33_get_keyval();                                //获取按钮键值
    switch(key){                                                //不同按键触发不同效果
      case IICKEY_UP:num++;break;
      case IICKEY_LEFT:num=num+100;break;
      case IICKEY_DOWN:num--;break;
      case IICKEY_RIGHT:num=num-100;break;
      case IICKEY_CENTER:{                                      //中间按键控制数码管的显示
        if(onoff)display_off();
        else display_on();
      }break;
    }
    if(num>9999)num=9999;                                       //数值范围控制
    if(num<=0)num=0;
    
    if(last_num!=num || key){                                   //处理显示
      if(last_num!=num)
        segment_display(num);                                   //数值变化更新显示，按键变化不更新
      last_num=num;
      //LCD显示信息
      snprintf(buf,100,"数码管数值:%04d     \n按键键值:%d  \r\n",num,key);
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,buf,LCD_WIDTH,BLACK,WHITE);
      printf(buf);                                              //串口打印提示信息
    }

    led_app(30);
    delay_ms(30);
  }
}  