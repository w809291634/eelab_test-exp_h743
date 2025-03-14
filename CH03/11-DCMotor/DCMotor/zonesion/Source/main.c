/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "dcmotor.h"

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
  
  dcmotor_init();                                               //初始化电机
  lcd_init(DCMOTOR);                                            //LCD初始化
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
  u8 mode=0;                                                    //电机运行模式，0停止，1正转，2反转
  u8 count;
  char buf[100];                                                //储存LCD显示信息
  hardware_init();
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,                      //显示初始信息
                "电机停止\n电机转速：0.00000r/s   ",LCD_WIDTH,BLACK,WHITE);
  while(1){
    u8 key= KEY_Scan(0);                                        //按键扫描
    if(key==K1_PRES){                                           //按键1控制正转
      if(mode>0) mode=0;                                        //如果当前电机处于正转或者反转，停止电机
      else mode=1;
    }else if(key==K2_PRES){                                     //按键2控制反转
      if(mode>0)mode=0;
      else mode=2;
    }
    if(key){                                                    //根据模式进行电机方向控制
      if(mode==0)DCMOTOR_STOP;
      if(mode==1)DCMOTOR_FORWARD;
      if(mode==2)DCMOTOR_BACKWARD;
    }
    if(count>10){                                               //定期显示电机转速
      count=0;
      switch(mode)                                              //根据具体的模式更新显示
      {
      case 0:
        sprintf(buf,"电机停止\n电机转速：%.5fr/s   ",dcmotor_speed);
        printf("motor stop,motor speed:%.5fr/s\r\n",dcmotor_speed);
        break;
      case 1:
        sprintf(buf,"电机正转\n电机转速：%.5fr/s   ",dcmotor_speed);
        printf("motor forward,motor speed:%.5fr/s\r\n",dcmotor_speed);
        break;
      case 2:
        sprintf(buf,"电机反转\n电机转速：%.5fr/s   ",dcmotor_speed);
        printf("motor backward,motor speed:%.5fr/s\r\n",dcmotor_speed);
        break;
      }
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,buf,LCD_WIDTH,BLACK,WHITE);
    }

    count++;                                                    //周期计数
    led_app(10);                                                //系统运行提示
    delay_ms(10);                                               //延时时间
  }
}  