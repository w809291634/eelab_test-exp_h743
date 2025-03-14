/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "fbm320.h"

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
  
  if(fbm320_init() == 1)                                        //查询fbm320状态
    printf("fbm320 ok!\r\n");
  lcd_init(AIRPRESSURE);                                        //LCD初始化
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
  float temperature = 0;                                        //存储温度数据变量
  long pressure = 0;                                            //存储压力数据变量
  float altitude = 0.0;                                         //存储海拔高度变量
  char buff[100];                                               //显示缓存数组
  hardware_init();
  while(1){
    fbm320_data_get(&temperature,&pressure);                    //获得温度，大气压力数据
    altitude =  (101325-pressure)*(100.0f/(101325 - 100131));   //获得海拔高度数据
    //将温度数据通过串口打印出来
    printf("temperature:%.1f℃\r\npressure:%0.1fkPa\r\n", temperature,pressure/1000.0f);                  
    printf("altitude:%0.1f m\r\n",altitude);                    //将海拔高度数据通过串口打印出来
    sprintf(buff,"温  度：%.1f℃   \n大气压：%.1fkPa   \n海  拔：%.1f m   ",
            temperature,pressure/1000.0f,altitude);             //LCD上显示温度 
    LCDShowFont32(8+32*2,REF_POS+32+5,buff,LCD_WIDTH,BLACK,WHITE); 

    led_app(1000);
    delay_ms(1000);
  }
}  