/*********************************************************************************************
* 文件：main.c
* 作者：Liyw 2018.1.23
* 说明：嵌入式接口技术项目实训一
*       图书馆是一个阅读和汲取新知识的地方，但如果图书馆光线不好会对读者的眼睛造成伤害，
*       因此为了给读者提供良好的阅读环境，需要对图书馆光照强度进行有效的监测，同时还需要
*       对光照进行调节，当光照强度大于某值时，关闭部分照明灯以降低光照强度，当光强较弱是
*       需要打开一些照明灯补光。图书馆照明调节系统分为三个部分，分别是控制系统、光强调节
*       系统和光照采集系统。通过这套系统可以为图书馆提供稳定的光源环境。LCD屏幕上显示光照
*       强度，阈值上下限和设备开关状态。。
*       光照度传感器：用于对图书馆内的光照强度进行监测和采集
*       D1/D2/D3/D4：用于表示图书馆内灯光的亮度控制级
*       rgb：用于对系统正常工作的状态指示
*       LCD屏幕：用于显示系统的相关信息
*       key1:用于对光强控制模式的设定
*       key2:用于手动控制中增加灯光的照明亮度
*       key3:用于手动控制中减弱灯光的照明亮度
* 修改：
* 注释：
*********************************************************************************************/
#include "extend.h"                                             //系统库函数头文件
#include "delay.h"                                              //延时函数头文件
#include "led.h"                                                //LED灯控制头文件
#include "rgb.h"                                                //rgb灯头文件
#include "BH1750.h"                                             //光照度传感器头文件
#include "usart.h"                                              //串口控制相关头文件
#include "lcd.h"                                                //LCD控制头文件
#include "screen.h"                                             //LCD屏幕显示头文件
#include "key.h"                                                //按键头文件
#include "timer.h"                                              //定时器头文件
#include "buzzer.h"                                             //蜂鸣器头文件

/*********************************************************************************************
* 参数宏定义
*********************************************************************************************/
#define EFFECTIVE_NUM           1000                            //按键循环检测有效册数

#define LUMI_THRESHLOD_MAX      5000                            //光照干预控制上限
#define LUMI_THRESHLOD_MIN      0                               //光照干预控制下限
#define LUMI_LEVEL_SET          4                               //光照控制级别
//光强分级控制参数宽度
#define LUMI_PARAGRAPH          ((LUMI_THRESHLOD_MAX - LUMI_THRESHLOD_MIN) / (LUMI_LEVEL_SET + 1))
//相对光强参数
#define LUMI_RELATIVE(a)        (a - LUMI_THRESHLOD_MIN)
//光强检测参数分级
#define LUMI_COEFFICIENT(a)     (LUMI_RELATIVE(a) / LUMI_PARAGRAPH)

/*********************************************************************************************
* 系统标志位
*********************************************************************************************/
uint8_t rgb_twinkle;                                            //rgb灯循环闪烁执行标志位
uint8_t get_lumination;                                         //光强检测执行标志位

/*********************************************************************************************
* 系统通用数组
*********************************************************************************************/
uint8_t led_level[] = {0x00, 0x01, 0x03, 0x07, 0x0f};           //灯光整体控制参数数组

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
  timer_init();                                                 //初始化定时器
  buzzer_init();                                                //初始化蜂鸣器
  rgb_init();                                                   //初始化rgb灯
  bh1750_init();                                                //初始化光照度传感器
  lcd_init(NO_NAME);                                            //LCD初始化
}

/*********************************************************************************************
* 名称：main()
* 功能：实训一项目逻辑主体
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int main(void)
{
  /*定义系统需求参数*/
  uint16_t mode_count = 0;                                      //模式选择按键循环检测参数
  uint8_t mode_select = 0;                                      //当前模式状态参数
  uint16_t add_count = 0;                                       //光强加按键循环检测参数
  uint16_t sub_count = 0;                                       //光强减按键循环检测参数
  uint8_t fill_light = 0;                                       //灯光补光控制等级
  uint8_t set_flag = 0;                                         //补光控制等级设置变更标志位
  uint8_t rgb_level = 0;                                        //RGB灯闪烁状态控制标志位
  uint16_t lumi_value = 0;                                      //光强检测值临时存储参数
  
  /*系统硬件初始化*/
  hardware_init();

  /*屏幕显示内容初始化*/
  show_logo();                                                  //显示屏幕logo
  show_fill_light(fill_light);                                  //显示光强控制图标
  show_value(0);                                                //显示初始化光强值
  show_mode(mode_select);                                       //显示当前控制模式
  while(1){                                                     //主循环 
    /*模式控制i按键检测及相关参数赋值*/
    if(key_status(K1) == 0) mode_count ++;                      //循环记录按键按下次数
    if(key_status(K1) && (mode_count > EFFECTIVE_NUM)){         //如果按键抬起且按下动作有效
      if(mode_select == 0) mode_select = 1;                     //设置为手动模式
      else mode_select = 0;                                     //设置为自动模式
      mode_count = 0;                                           //循环计数值清零
      buzzer_tweet();                                           //蜂鸣器鸣响
      show_mode(mode_select);                                   //模式状态图标发生切换
    }
    
    /*光强加控制按键检测及相关参数赋值*/
    if(mode_select){                                            //如果当前为手动模式
      if(key_status(K2) == 0) add_count ++;                     //循环记录按键按下次数
      if(key_status(K2) && (add_count > EFFECTIVE_NUM)){        //如果按键抬起且按下动作有效
        //如果光强设置值未溢出则置最大值
        if(fill_light >= LUMI_LEVEL_SET) fill_light = LUMI_LEVEL_SET;
        else fill_light ++;                                     //补光级数加一
        set_flag = 1;                                           //设置变更标志位置位
        add_count = 0;                                          //循环计数值清零
        buzzer_tweet();                                         //蜂鸣器鸣响
      }
    }
    
    /*光强减控制按键检测及相关参数赋值*/
    if(mode_select){                                            //如果当前为手动模式
      if(key_status(K3) == 0) sub_count ++;                     //循环记录按键按下次数
      if(key_status(K3) && (sub_count > EFFECTIVE_NUM)){        //如果按键抬起且按下动作有效
        if(fill_light <= 0) fill_light = 0;                     //如果光强设置值未溢出则置最小值
        else fill_light --;                                     //补光计数减一
        set_flag = 1;                                           //设置变更标志位置位
        sub_count = 0;                                          //循环计数值清零
        buzzer_tweet();                                         //蜂鸣器鸣响
      }
    }
    
    /*RGB灯闪烁控制*/
    if(rgb_twinkle){                                            //如果RGB灯闪烁执行标志位置位
      rgb_ctrl(rgb_level % 7 + 1);                              //RGB灯闪烁状态控制标志位
      rgb_level ++;                                             //RGB灯闪烁控制标志位加一
      rgb_twinkle = 0;                                          //RGB灯闪烁执行标志位清空
    }
    
    /*采集光强值，计算合理的光强控制级数*/
    if(get_lumination){                                         //如果光强检测标志位置位
      lumi_value = (uint16_t)(bh1750_get_data());               //获取当前光照强度
      show_value(lumi_value);                                   //屏幕显示当前光强数值
      if(!mode_select){                                         //如果为自动模式
        //如果光强小于干预光强的最小值
        if(LUMI_RELATIVE(lumi_value) < 0) fill_light = LUMI_LEVEL_SET;
        else if(LUMI_COEFFICIENT(lumi_value) >= LUMI_LEVEL_SET){//如果光强级数大于了设定控制级数
          fill_light = 0;                                       //那么设定光强控制计数为0
        }else{
          //获取光强控制级数
          fill_light = LUMI_LEVEL_SET - LUMI_COEFFICIENT(lumi_value);
        }
        set_flag = 1;                                           //光强控制级数变更标志位置位
      }
      get_lumination = 0;                                       //光强传感器采集执行标志位复位
    }
    
    /*执行LCD上的数值显示和led控制*/
    if(set_flag){                                               //如果光强控制级数变更标志位置位
      led_turn_on(led_level[fill_light]);                       //打开相应led指示灯
      led_turn_off((~led_level[fill_light]) & 0x0f);            //关闭相应led指示灯
      show_fill_light(fill_light);                              //屏幕显示光强控制级数
    }
    
    set_flag = 0;                                               //光强控制级数变更标志位复位
  }
}