/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "lis3dh.h"

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
  
  lcd_init(ACCELERATION);                                       //LCD初始化
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
  float accx,accy,accz;                                         //储存x,y,z,变量
  char buff[100];                                               //显示缓存数组
  hardware_init();
  if(lis3dh_init() == 0){                                       //三轴传感器初始化
    printf("lis3dh ok!\r\n");                                   //串口打印
    //LCD显示提示信息
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,"lis3dh ok!",LCD_WIDTH,BLACK,WHITE);
  }
  else{
    printf("lis3dh error!\r\n");                                //串口打印
    //LCD显示提示信息
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,"lis3dh error!",LCD_WIDTH,BLACK,WHITE);
  }
  while(1){
    get_lis3dhInfo(&accx,&accy,&accz);                          //从三轴传感器读取x,y,z数据
    printf("accx:%.1f N/Kg accy:%.1f N/Kg accz:%.1f N/Kg\r\n",accx,accy,accz); //串口打印
    snprintf(buff,100,"x轴：%.1f N/Kg  y轴：%.1f N/Kg   \nz轴：%.1f N/Kg  ",accx,accy,accz); 
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,buff,LCD_WIDTH,BLACK,WHITE);
    
    led_app(1000);
    delay_ms(1000);
  }
}  