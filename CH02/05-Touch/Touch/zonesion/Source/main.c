/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "gt911.h"

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
  
  GT911_init();                                                 //触摸屏初始化
  lcd_init(TOUCH);                                              //LCD初始化
}

/*********************************************************************************************
* 名称：clean_lcd_info()
* 功能：清理LCD上显示的触摸点信息
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
static void clean_lcd_info(void)
{
  static char last_num=0xff;                                    //存放信息
  if(last_num!=GT911_pointNum){
    last_num = GT911_pointNum;
    switch(GT911_pointNum){
      /* 清理所有信息 */
      case 0:LCD_Fill(8+32*2,REF_POS+32+5,LCD_WIDTH,32*3+5*2,WHITE);break;
      /* 清理第二、三行信息和处理第一行的显示 */
      case 1:
      LCD_Fill(LCD_WIDTH/2+32,REF_POS+32+5,LCD_WIDTH/2,32,WHITE);
      LCD_Fill(8+32*2,REF_POS+32*2+5*2,LCD_WIDTH,32*2+5*1,WHITE);break;
      case 2:
      LCD_Fill(8+32*2,REF_POS+32*2+5*2,LCD_WIDTH,32*2+5*1,WHITE);break;
      /* 清理第三行信息和处理第二行的显示 */
      case 3:
      LCD_Fill(LCD_WIDTH/2+32,REF_POS+32*2+5*2,LCD_WIDTH/2,32,WHITE);
      LCD_Fill(8+32*2,REF_POS+32*3+5*3,LCD_WIDTH,32*1+5,WHITE);break;
      case 4:
      LCD_Fill(8+32*2,REF_POS+32*3+5*3,LCD_WIDTH,32*1+5,WHITE);break;
      /* 不进行处理 */
      case 5:break;
    }
  }
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
  u8 count=0;
  u8 ledon=0;
  char buf[100];
  hardware_init();                                              //初始化
  while(1){
    GT911_scan();
    /* 处理LCD显示信息 */
    for(int i=0;i<GT911_pointNum;i++){                          //遍历每个触摸点的信息
      snprintf(buf,100,"ID%d x:%3d y:%3d size:%2d\r\n",
               GT911Point[i].id,GT911Point[i].x,
               GT911Point[i].y,GT911Point[i].size);
      // printf(buf);                                           //打印信息
      if(i==0)                                                  //显示ID为0的触摸点
        LCDShowFont32(8+32*2,REF_POS+32+5,buf,LCD_WIDTH,BLACK,WHITE);
      else if(i==1)                                             //显示ID为1的触摸点
        LCDShowFont32(LCD_WIDTH/2+32,REF_POS+32+5,buf,LCD_WIDTH,BLACK,WHITE);
      else if(i==2)                                             //显示ID为2的触摸点
        LCDShowFont32(8+32*2,REF_POS+32*2+5*2,buf,LCD_WIDTH,BLACK,WHITE);
      else if(i==3)                                             //显示ID为3的触摸点
        LCDShowFont32(LCD_WIDTH/2+32,REF_POS+32*2+5*2,buf,LCD_WIDTH,BLACK,WHITE);
      else if(i==4)                                             //显示ID为4的触摸点
        LCDShowFont32(8+32*2,REF_POS+32*3+5*3,buf,LCD_WIDTH,BLACK,WHITE);
    }
    /* 清理之前LCD上显示的信息 */
    clean_lcd_info();
    
    count++;
    if(count%10==0){                                            //系统运行指示灯
      ledon=!ledon;
      if(ledon) led_setState(LED4_NUM,1);
      else led_setState(LED4_NUM,0);
    }
    delay_ms(10);                                               //延时10毫秒
  }
}  