/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "pcf8563.h"
#include "string.h"

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
  
  PCF8563_init();
  lcd_init(IIC);                                                //LCD初始化
}

/*********************************************************************************************
* 名称：check_time_vaild()
* 功能：检查时间的有效性
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
void check_time_vaild(calendar_t* time){
  if(time->second>59){
    time->second=0;
    time->minute++;
  }
  if(time->minute>59){
    time->minute=0;
    time->hour++;
  }
  if(time->hour>23){
    time->hour=0;
    time->day++;
  }
  if(time->day>31){
    time->day=1;
    time->month++;
  }
  if(time->month>12){
    time->month=1;
    time->year++;
  }
}

/*********************************************************************************************
* 名称：main()
* 功能：主函数
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
calendar_t test_time={.year=23,
                  .month=11,
                  .day=17,
                  .hour=10,
                  .minute=4,
                  .second=23};                                  //初始的设置时间测试数组
int main(void)
{
  char count=100;                                               //用于形成1S的循环周期
  char state=0;                                                 //状态栏是否刷新
  char init=1;                                                  //用于LCD显示首次的设定时间
  char temp[100]={0};
  u8 key_value=0;                                               //储存按键键值
  calendar_t time={0};
  hardware_init();                                              //硬件初始化
  while(1){
    key_value=KEY_Scan(0);
    /* 设置RTC设定时间 */
    if(key_value || init){
      init=0;
      switch(key_value){
        case K1_PRES:test_time.minute++;break;                  //按键1控制设定时间分针增1
        case K2_PRES:test_time.hour++;break;                    //按键2控制设定时间时针增1
        case K3_PRES:test_time.day++;break;                     //按键3控制设定时间天数增1
        case K4_PRES:{                                          //按键4触发RTC设定时间
          if(PCF8563_settime(&test_time)){
            snprintf(temp,100,"%s","PCF8563_settime success!   \r\n");
            printf(temp);
            LCDShowFont32(8+32*2,REF_POS+32*3+SPACING*2+5,      //LCD显示设置时间成功
                  temp,LCD_WIDTH,BLACK,WHITE); 
          }else{
            snprintf(temp,100,"%s","PCF8563_settime fail!     \r\n");
            printf(temp);
            LCDShowFont32(8+32*2,REF_POS+32*3+SPACING*2+5,      //LCD显示设置时间失败
                          temp,LCD_WIDTH,BLACK,WHITE); 
          }
          state=1;                                              //标记状态栏已经刷新，后面需要清除
        }break;
      }
      /* 获取设定时间 */
      check_time_vaild(&test_time);                             //检查设定时间
      snprintf(temp,100,"设定时间:%04d/%02d/%02d %02d:%02d:%02d",//控制LCD的显示文本
             test_time.year+2000,test_time.month,test_time.day,test_time.hour,test_time.minute,test_time.second);
      LCDShowFont32(8+32*2,REF_POS+32+5,                        //LCD显示文本
                    temp,LCD_WIDTH,BLACK,WHITE); 
    }

    /* 显示当前RTC实时时间 和 状态栏 */
    if(count>=100 && PCF8563_gettime(&time)){
      count=0;
      /* 清除状态栏LCD文本 */
      if(state){
        state++;
        if(state>3){
          state=0;
          LCDShowFont32(8+32*2,REF_POS+32*3+SPACING*2+5,        //LCD状态栏清理文本
                    "                         ",LCD_WIDTH,BLACK,WHITE); 
        }
      }
      /* 获取实际时间 */
      snprintf(temp,100,"实际时间:%04d/%02d/%02d %02d:%02d:%02d",//控制LCD的显示文本
             time.year+2000,time.month,time.day,time.hour,time.minute,time.second);

      time.week = CountWeek(time.year+2000, time.month, time.day);
      switch(time.week){
        case 0:strcat(temp," Sunday    \r\n");break;
        case 1:strcat(temp," Monday    \r\n");break;
        case 2:strcat(temp," Tuesday    \r\n");break;
        case 3:strcat(temp," Wednesday    \r\n");break;
        case 4:strcat(temp," Thursday    \r\n");break;
        case 5:strcat(temp," Friday    \r\n");break;
        case 6:strcat(temp," Saturday    \r\n");break;
        default:strcat(temp," error    \r\n");break;
      }
      LCDShowFont32(8+32*2,REF_POS+32*2+SPACING+5,              //LCD显示文本
                    temp,LCD_WIDTH,BLACK,WHITE); 
      printf(temp);
    }
    
    count++;
    led_app(10);
    delay_ms(10);                                               //延时
  }
}  
