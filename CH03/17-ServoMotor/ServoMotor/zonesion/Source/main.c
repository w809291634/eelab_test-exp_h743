/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "ServoMotor.h"

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
  
  ServoMotor_init(20000,200-1);                                 //伺服电机初始化,200M/200=1mhz,20msPWM波
  lcd_init(SERVOMOTOR);                                         //LCD初始化
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
  s16 angle=0;                                                  //存储当前控制角度
  s16 last_angle=-1;                                            //存储上一次控制角度
  char temp[100]={0};                                           //用于转换存储
  hardware_init();                                              //硬件初始化
  while(1){
    u8 key =  KEY_Scan(0);                                      //按键键值扫描
    if(key == K1_PRES) angle = angle+30;
    else if(key == K2_PRES) angle = angle-30;                   //通过按键控制角度
    if(angle > 180) angle=180;                                  //范围限制
    if(angle < 0) angle=0;
    if(angle!=last_angle){
      last_angle = angle;
      Servo_control(angle);                                     //控制舵机到一个角度
      // 处理显示
      sprintf(temp,"舵机角度:%d°   \r\n",angle);                //处理显示
      printf(temp);
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);
    }

    led_app(10);
    delay_ms(10);
  }
}  