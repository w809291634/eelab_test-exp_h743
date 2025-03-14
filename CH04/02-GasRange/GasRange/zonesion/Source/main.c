/*********************************************************************************************
* 文件：main.c
* 作者：Liyw 2018.1.17
* 说明：嵌入式接口技术项目实训二
*       传统的燃气灶采用机械式设计，在防火和防止煤气泄漏上都存在缺陷。现需要设计
*       一款集成灶，将火焰监测和燃气泄漏检测键入其中。该集成灶能够通过按钮触发电
*       子灶台开关，燃气阀打开，通过按钮控制火候大小火焰分为4级（小火、中火、高火、
*       大火），当灶台打开但没有检测到有火焰同时燃气泄漏时产生报警信号，同时燃气阀
*       关闭。LCD屏幕上显示集成灶开关状态，火候大小，火焰检测、燃气监测和厂家logo。
*       K4:作为燃气灶的总开关
*       蜂鸣器:按键操作提示音
*       D1\D2\D3\D4:火焰档位显示
*       RGB:燃气灶开机特效显示
*       K1:火焰档位加
*       K2:火焰档位减
*       继电器1:模拟天然气总阀
*       LCD屏幕:集成灶信息显示界面
* 修改：
* 注释：系统运行需要能够检测到明火，如无法提供明火可将main.c文件中get_flame_status()函数注释掉
*********************************************************************************************/
#include "extend.h"                                             //库函数头文件
#include "led.h"                                                //led灯头文件
#include "delay.h"                                              //系统延时头文件
#include "timer.h"                                              //定时器头文件
#include "usart.h"                                              //串口口文件
#include "rgb.h"                                                //RGB灯头文件
#include "key.h"                                                //按键头文件
#include "buzzer.h"                                             //蜂鸣器头文件
#include "relay.h"                                              //继电器头文件
#include "lcd.h"                                                //LCD驱动头文件
#include "MP-4.h"                                               //燃气监测传感器头文件
#include "Flame.h"                                              //火焰传感器头文件

/*********************************************************************************************
* 参数宏定义
*********************************************************************************************/
#define GAS_THRESHOLD           2000                            //燃气浓度阈值
#define PUTOUT_SCREEN_DELAY     5000                            //熄屏计数阈值

/*********************************************************************************************
* 系统标志位
*********************************************************************************************/
uint8_t rgb_twinkle;                                            //RGB灯闪烁执行标志位
uint8_t gas_check;                                              //燃气监测执行标志位
uint8_t fire_effect;                                            //火焰图案帧切换标志位

/*********************************************************************************************
* 屏幕显示存储数组全局变量
* 以下图片C文件使用emwin BmpCvtST.exe软件生成，打开后保存为C格式，
* 注意:
*       1、选择输出格式类型为：High color (565] red and blue swapped
*       2、需要修改输出的生成的C文件，做部分修改，每个图片数据数据数组中
*       前两个元素表示图片像素宽度和高度，需要自己手动补充。
*********************************************************************************************/
extern unsigned short _acflame_off[];                           //火焰关闭
extern unsigned short _acflame_on_1[];                          //火焰开启1
extern unsigned short _acflame_on_2[];                          //火焰开启2
extern unsigned short _acflame_on_3[];                          //火焰开启3
extern unsigned short _acflame_on_4[];                          //火焰开启4
extern unsigned short _acflame_big_on[];                        //大火开关开
extern unsigned short _acflame_big_off[];                       //大火开关关
extern unsigned short _acflame_high_on[];                       //高火开关开
extern unsigned short _acflame_high_off[];                      //高火开关关
extern unsigned short _acflame_medium_on[];                     //中火开关开
extern unsigned short _acflame_medium_off[];                    //中火开关关
extern unsigned short _acflame_small_on[];                      //小火开关开
extern unsigned short _acflame_small_off[];                     //小火开关关
extern unsigned short _actotal_switch_on[];                     //总开关开
extern unsigned short _actotal_switch_off[];                    //总开关关
extern unsigned short _acgas_leakage_t[];                       //燃气泄漏
extern unsigned short _acgas_leakage_f[];                       //燃气未泄漏
extern unsigned short _acidentification[];                      //燃气检测字符
extern unsigned short _acaux_logo[];                            //AUX logo标识

/*********************************************************************************************
* 定义图标位置
*********************************************************************************************/
#define TOTAL_SWITCH_X  ((int)(LCD_WIDTH*0.05f))                //总开关图标起始位置
#define TOTAL_SWITCH_Y  ((int)(LCD_HEIGHT*0.05f))

#define AUX_LOGO_X      ((int)(LCD_WIDTH*0.8f))                 //AUX图标起始位置
#define AUX_LOGO_Y      ((int)(LCD_HEIGHT*0.05f))

#define FLAME_X         ((int)(LCD_WIDTH*0.31f))                //火焰图标起始位置
#define FLAME_Y         ((int)(LCD_HEIGHT*0.17f))

#define FLAME_S_X       ((int)(LCD_WIDTH*0.31f))                //火焰"小"字符图标起始位置
#define FLAME_S_Y       ((int)(LCD_HEIGHT*0.57f))
#define FLAME_M_X       ((int)(LCD_WIDTH*0.41f))                //火焰"中"字符图标起始位置
#define FLAME_M_Y       ((int)(LCD_HEIGHT*0.57f))
#define FLAME_H_X       ((int)(LCD_WIDTH*0.51f))                //火焰"高"字符图标起始位置
#define FLAME_H_Y       ((int)(LCD_HEIGHT*0.57f))
#define FLAME_B_X       ((int)(LCD_WIDTH*0.61f))                //火焰"大"字符图标起始位置
#define FLAME_B_Y       ((int)(LCD_HEIGHT*0.57f))

#define IDENTIF_X       ((int)(LCD_WIDTH*0.05f))                //中文标识图标起始位置
#define IDENTIF_Y       ((int)(LCD_HEIGHT*0.825f))

#define GAS_LEAKAGE_X   ((int)(LCD_WIDTH*0.26f))                //燃气泄露标识图标起始位置
#define GAS_LEAKAGE_Y   ((int)(LCD_HEIGHT*0.833f))

#define MAGNIFICATION   2                                       //图标的放大倍数

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
  rgb_init();                                                   //RGB灯初始化
  timer_init();                                                 //定时器时间片功能初始化
  buzzer_init();                                                //蜂鸣器初始化
  relay_init();                                                 //继电器初始化
  combustiblegas_init();                                        //燃气传感器初始化
  flame_init();                                                 //火焰传感器初始化

  lcd_init(NO_NAME);                                            //LCD初始化
}

/*********************************************************************************************
* 名称：main()
* 功能：实训二项目逻辑主体
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int main(void)
{
  //配置系统各参数标志位
  uint8_t total_switch = 0;                                     //燃气灶总开关标志位
  uint8_t last_total_switch = 0;                                //储存上一次总开关标志位
  uint8_t key4_switch = 0;                                      //K4按键开关
  uint16_t count = 0;                                           //熄屏计数参数
  uint8_t fire_level = 0;                                       //火焰大小标志位
  uint8_t fire_set_last = 0;                                    //火焰大小标志位存储参数
  uint32_t gas_concentration = 0;                               //燃气浓度存储参数
  uint8_t fire_monitor = 0;                                     //火焰监测结果存储参数
  uint8_t safety_lock = 0;                                      //系统安全锁标志位
  uint8_t fire_film = 0;                                        //火焰图案帧控制标志位
  uint8_t rgb_level = 0;                                        //RGB灯闪烁状态控制标志位
  //初始化系统各个设备
  hardware_init();
  //屏幕初始状态加载
  LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y, _actotal_switch_off[0], _actotal_switch_off[1], _actotal_switch_off,MAGNIFICATION);//加载总开关关闭效果
  LCD_FillData_ex(AUX_LOGO_X, AUX_LOGO_Y, _acaux_logo[0], _acaux_logo[1], _acaux_logo,MAGNIFICATION);//加载AUX图标位置
  LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_off[0], _acflame_off[1], _acflame_off,MAGNIFICATION); //加载火焰关闭效果
  LCD_FillData_ex(FLAME_S_X, FLAME_S_Y ,_acflame_small_off[0], _acflame_small_off[1], _acflame_small_off,MAGNIFICATION);//加载下火关闭效果
  LCD_FillData_ex(FLAME_M_X, FLAME_M_Y ,_acflame_medium_off[0], _acflame_medium_off[1], _acflame_medium_off,MAGNIFICATION);//加载中火关闭效果
  LCD_FillData_ex(FLAME_H_X, FLAME_H_Y, _acflame_high_off[0], _acflame_high_off[1], _acflame_high_off,MAGNIFICATION);//加载高火关闭效果
  LCD_FillData_ex(FLAME_B_X, FLAME_B_Y, _acflame_big_off[0], _acflame_big_off[1], _acflame_big_off,MAGNIFICATION);//加载大火关闭效果
  LCD_FillData_ex(IDENTIF_X, IDENTIF_Y, _acidentification[0], _acidentification[1], _acidentification,MAGNIFICATION);//加载"燃气监测"标识
  
  //系统主循环后台程序
  while(1){                                                     //主循环
    //检测安全锁可总开关状态并配置相关参数显示总开关效果
    if(safety_lock){                                            //检测安全锁是否正常
      if(!key_status(K4)){
        delay_ms(50);
        if(!key_status(K4)){
          key4_switch = 1 - key4_switch;
          while(!key_status(K4));
        }
      }
      if(key4_switch){                                          //检测K4开关是否为开启
        if(total_switch != 1){                                  //检测系统总开关是否置位
          buzzer_tweet();                                       //蜂鸣器鸣响
          total_switch = 1;                                     //总开关开
          RELAY1_CTRL(ON);                                      //继电器（燃气阀）打开
          LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y, _actotal_switch_on[0], _actotal_switch_on[1], _actotal_switch_on,MAGNIFICATION);//加载总开关开效果
        }
      }else{                                                    //否则
        if(total_switch == 1){                                  //检测总开关是否关闭
          rgb_ctrl(0);                                          //RGB灯关闭
          buzzer_tweet();                                       //蜂鸣器鸣响
          total_switch = 0;                                     //总开关关闭
          fire_level = 0;                                       //火焰控制关闭
          RELAY1_CTRL(OFF);                                     //继电器（燃气阀）关闭
          LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y, _actotal_switch_off[0], _actotal_switch_off[1], _actotal_switch_off,MAGNIFICATION);//加载总开关关闭效果
        }
      }
    }
    
    //检测总开关打开后燃气灶是否正常点火并配置相关参数
    if(total_switch /*&& get_flame_status()*/){                 //判断总开关是否打开，是否正常点火
      if(fire_monitor == 0) fire_level = 1;                     //判断火焰检测是否被置位并设置火焰为1级
      fire_monitor = 1;                                         //设置火焰监测标志位为正常
      last_total_switch=total_switch;                           //记录当前总开关状态
    }else{                                                      //否则
      if(last_total_switch!=total_switch){                      //画面刷新为总开关关闭状态
        last_total_switch=total_switch;
        LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_off[0], _acflame_off[1], _acflame_off,MAGNIFICATION);//加载火焰关闭效果
        LCD_FillData_ex(FLAME_S_X, FLAME_S_Y, _acflame_small_off[0], _acflame_small_off[1], _acflame_small_off,MAGNIFICATION);//加载下火关闭效果
        LCD_FillData_ex(FLAME_M_X, FLAME_M_Y, _acflame_medium_off[0], _acflame_medium_off[1], _acflame_medium_off,MAGNIFICATION);//加载中火关闭效果
        LCD_FillData_ex(FLAME_H_X, FLAME_H_Y, _acflame_high_off[0], _acflame_high_off[1], _acflame_high_off,MAGNIFICATION);//加载高火关闭效果
        LCD_FillData_ex(FLAME_B_X, FLAME_B_Y, _acflame_big_off[0], _acflame_big_off[1], _acflame_big_off,MAGNIFICATION);//加载大火关闭效果
      }
      fire_level = 0;                                           //火焰等级设置为0关闭
      fire_monitor = 0;                                         //火焰信号检测异常
    }
    
    //屏幕火焰效果控制
    if(fire_monitor && fire_effect){                            //火焰监测是否正常，火焰图案帧切换是否置位
      switch(fire_film % 4){                                    //火焰帧标志位取余
      case 0:                                                   //当余0时
        LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_on_1[0], _acflame_on_1[1], _acflame_on_1,MAGNIFICATION);//更新火焰开1
        break;                                                  //跳出
      case 1:                                                   //当余1时
        LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_on_2[0], _acflame_on_2[1], _acflame_on_2,MAGNIFICATION);//更新火焰开2
        break;                                                  //跳出
      case 2:                                                   //当余2时
        LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_on_3[0], _acflame_on_3[1], _acflame_on_3,MAGNIFICATION);//更新火焰开3
        break;                                                  //跳出
      case 3:                                                   //当余3时
        LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_on_4[0], _acflame_on_4[1], _acflame_on_4,MAGNIFICATION);//更新火焰开4
        break;                                                  //跳出
      }    
      fire_effect = 0;                                          //清空火焰帧切换标志位
      fire_film ++;                                             //火焰帧标志位加一
    }
    
    //RGB灯闪烁效果控制
    if(total_switch && rgb_twinkle){                            //系统总开关是否开启。RGB灯闪烁控执行标志位是否置位
      rgb_ctrl(rgb_level % 7 + 1);                              //RGB灯闪烁状态控制标志位
      rgb_level ++;                                             //RGB灯闪烁控制标志位加一
      rgb_twinkle = 0;                                          //RGB灯闪烁执行标志位清空        
    }
    
    //熄屏亮屏控制
    if(total_switch || !safety_lock){                           //总开关是否开启，安全锁是否异常
      LCD_BL(1);                                                //屏幕点亮
      count = 0;                                                //熄屏延时参数清空
    }else{                                                      //否则
      if(count == PUTOUT_SCREEN_DELAY){                         //熄屏延时参数是否与熄屏延时阈值一致
        LCD_BL(0);                                              //屏幕熄灭
      }else count ++;                                           //否则熄屏延时参数加一
    }
    
    //火焰等级配置及参数配置
    if(total_switch && safety_lock && fire_monitor){            //总开关，安全开关，火焰监测是否均正常
      if(!key_status(K1)){                                      //检测按键1是否被按下
        buzzer_tweet();                                         //蜂鸣器鸣叫
        if(!key_status(K1)){                                    //检测按键是否确实被按下
          while(!key_status(K1));                               //等待按键抬起
          delay_ms(10);                                         //延时10ms
          if(!(fire_level & 0x08)){                             //检测火焰级别是否设置满
            fire_level = (fire_level << 1) + 1;                 //火焰等级加一
          }
        }
      }
      if(!key_status(K2)){                                      //检测按键2是否被按下
        buzzer_tweet();                                         //蜂鸣器鸣响
        if(!key_status(K2)){                                    //检测按键2是否确实被按下
          while(!key_status(K2));                               //等待按键抬起
          delay_ms(10);                                         //延时10ms
          if(fire_level & 0x02){                                //检测火焰级别是否被设置空
            fire_level = (fire_level >> 1);                     //火焰等级减一
          }
        }
      }
    }
    
    //执行火焰等级的控制效果
    if(fire_level != fire_set_last){                            //查看此次火焰设置值是否与前一次相同
      
      led_control(fire_level);                                  //控制火焰等级显示灯
      
      if((fire_level & 0x01) && (fire_level == 0x01)){          //判断火焰等级是否为小火
        LCD_FillData_ex(FLAME_S_X, FLAME_S_Y ,_acflame_small_on[0], _acflame_small_on[1], _acflame_small_on,MAGNIFICATION);//执行小火按钮点亮
      }else LCD_FillData_ex(FLAME_S_X, FLAME_S_Y ,_acflame_small_off[0], _acflame_small_off[1], _acflame_small_off,MAGNIFICATION);//执行小火按钮熄灭
      
      if((fire_level & 0x02) && (fire_level == 0x03)){          //判断火焰等级是否为中火
        LCD_FillData_ex(FLAME_M_X, FLAME_M_Y, _acflame_medium_on[0], _acflame_medium_on[1], _acflame_medium_on,MAGNIFICATION);//执行中火钮点亮
      }else LCD_FillData_ex(FLAME_M_X, FLAME_M_Y, _acflame_medium_off[0], _acflame_medium_off[1], _acflame_medium_off,MAGNIFICATION);//执行中火按钮熄灭
      
      if((fire_level & 0x04) && (fire_level == 0x07)){          //判断火焰等级是否为高火   
        LCD_FillData_ex(FLAME_H_X, FLAME_H_Y, _acflame_high_on[0], _acflame_high_on[1], _acflame_high_on,MAGNIFICATION);//执行高火按钮点亮
      }else LCD_FillData_ex(FLAME_H_X, FLAME_H_Y, _acflame_high_off[0], _acflame_high_off[1], _acflame_high_off,MAGNIFICATION);//执行高火按钮熄灭
      
      if((fire_level & 0x08) && (fire_level == 0x0f)){          //判断火焰等级是否为大火   
        LCD_FillData_ex(FLAME_B_X, FLAME_B_Y, _acflame_big_on[0], _acflame_big_on[1], _acflame_big_on,MAGNIFICATION);//执行大火按钮点亮
      }else LCD_FillData_ex(FLAME_B_X, FLAME_B_Y, _acflame_big_off[0], _acflame_big_off[1], _acflame_big_off,MAGNIFICATION);//执行大火按钮熄灭
      
      fire_set_last = fire_level;                               //存储此次的火焰控制等级
    }
    
    //燃气监测和参数获取
    if(gas_check){                                              //燃气监测执行标志位是否置位
      gas_concentration = get_combustiblegas_data();            //获取燃气浓度检测值        
      printf("gas_concentration:%d\r\n",gas_concentration);
      gas_check = 0;                                            //清空燃气监测执行标志位
    }
    
    //执行燃气泄漏状况监测
    if(gas_concentration > GAS_THRESHOLD){                      //燃气浓度值是否超过燃气浓度阈值
      if(safety_lock != 0){                                     //检测安全锁是否复位
        safety_lock = 0;                                        //安全锁复位
        total_switch = 0;                                       //总开关关闭
        fire_level = 0;                                         //火焰级值0
        rgb_ctrl(0);                                            //RGB灯关闭
        RELAY1_CTRL(OFF);                                       //继电器（燃气阀）关闭
        LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y, _actotal_switch_off[0], _actotal_switch_off[1], _actotal_switch_off,MAGNIFICATION);//加载总开关关闭效果 
      }
      buzzer_tweet();                                           //蜂鸣器鸣响
      LCD_FillData_ex(GAS_LEAKAGE_X, GAS_LEAKAGE_Y, _acgas_leakage_t[0], _acgas_leakage_t[1], _acgas_leakage_t,MAGNIFICATION);//燃气泄漏标识点亮
      delay_ms(50);                                             //延时50MS
      LCD_FillData_ex(GAS_LEAKAGE_X, GAS_LEAKAGE_Y, _acgas_leakage_f[0], _acgas_leakage_f[1], _acgas_leakage_f,MAGNIFICATION);//燃气泄漏标识熄灭
    }else{
      if(safety_lock != 1){                                     //检测安全锁是否置位
        safety_lock = 1;                                        //安全锁置位
        LCD_FillData_ex(GAS_LEAKAGE_X, GAS_LEAKAGE_Y, _acgas_leakage_f[0], _acgas_leakage_f[1], _acgas_leakage_f,MAGNIFICATION);//燃气泄漏标识熄灭
      }
    }
    //循环总延时
    delay_ms(1);                                                //延时1MS
  }
}
