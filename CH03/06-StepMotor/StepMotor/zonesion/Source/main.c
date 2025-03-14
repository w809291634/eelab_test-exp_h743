/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "stepmotor.h"

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
  
  stepmotor_init();                                             //步进电机初始化
  lcd_init(STEPMOTOR);                                          //LCD初始化
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
  unsigned char stepmotor_flag = 0;                             //储存步进电机状态变量
  static unsigned char last_stepmotor_flag;                     //储存上一次变量值
  hardware_init();
  while(1){
    u8 key=KEY_Scan(0);
    if(key==K1_PRES){
      if((stepmotor_flag & 0x01) == 0){                         //判断步进电机正转
        stepmotor_flag = 0x03;                                  //按下正转
        //LCD显示提示信息
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"步进电机正转",LCD_WIDTH,BLACK,WHITE);   
        printf("stepmotor forward!\r\n");                       //串口打印提示信息
      }
      else{
        stepmotor_flag = 0;                                     //停止
        //LCD显示提示信息
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"步进电机停止",LCD_WIDTH,BLACK,WHITE);  
        printf("stepmotor stop!\r\n");                          //串口打印提示信息
      }
    }
    if(key==K2_PRES){
      if((stepmotor_flag & 0x01) == 0){                         //判断步进电机反转
        stepmotor_flag = 0x01;                                  //按下反转
        //LCD显示提示信息
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"步进电机反转",LCD_WIDTH,BLACK,WHITE);  
        printf("stepmotor reverse!\r\n");                       //串口打印提示信息
      }
      else{
        stepmotor_flag = 0;                                     //停止
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"步进电机停止",LCD_WIDTH,BLACK,WHITE);  
        printf("stepmotor stop!\r\n");                          //串口打印提示信息
      } 
    }
    if(last_stepmotor_flag!=stepmotor_flag){
      last_stepmotor_flag=stepmotor_flag;
      stepmotor_control(stepmotor_flag);                          //根据状态变量控制步进电机
    }
    
    led_app(10);
    delay_ms(10);
  }
}  