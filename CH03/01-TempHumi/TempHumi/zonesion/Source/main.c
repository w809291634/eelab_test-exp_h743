/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "htu21d.h"

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
  
  htu21d_init();                                                //传感器初始化
  lcd_init(TEMPHUMI);                                           //LCD初始化
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
  int Original_Temp = 0;
  int Original_Humi = 0;
  float Temp = 0.0;
  float Humi = 0.0;
  char temp[100]={0};
  hardware_init();
  while(1){
    Original_Temp=htu21d_read_temp();                           //获取当前温度原始数据
    Original_Humi=htu21d_read_humi();                           //获取当前湿度原始数据
    Temp = (float)Original_Temp/1000;
    Humi = (float)Original_Humi/1000;
    printf("原始温度:%.2f℃\r\n",Temp);                          //串口打印温度原始数据
    printf("原始湿度:%.2f\r\n",Humi);                            //串口打印湿度原始数据
    sprintf(temp,"温度:%.2f℃   \n湿度:%.2f%%rh  ",Temp,Humi);   //将温度湿度原始数据转化为字符串存储在temp数组中
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);    
    
    led_app(1000);
    delay_ms(1000);
  }
}  
