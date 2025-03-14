/*********************************************************************************************
* 文件：main.c
* 作者：Liyw 2018.1.22
* 说明：嵌入式接口技术项目实训三  
*       如今洗衣机更能多样、操作繁琐且设计冗余众多造成了极大的资源浪费。先设计一款功能
*       简洁的智能洗衣机。洗衣机拥有电子总开关，模式选择按键时间设置按键和启动按键。模
*       式选择中可以设置浸泡、洗衣、脱水和全功能。在浸泡开启时打开注水电磁阀，注水完成
*       后关闭然后浸泡。洗衣模式时电机以洗衣模式工作，清洗完成后打开放水电磁阀放水。在
*       脱水模式时放水电磁阀直接打开，电机全功能运转。全功能模式时则是按照流程浸泡、洗
*       衣、脱水逐步完成。按键在按下时蜂鸣器会鸣响提示。LCD屏幕上会显示洗衣机工作模式、
*       计时和当前工作状态。。
*       K1:洗衣机总开关
*       K2:洗衣模式选择按键
*       K3:洗衣水位设置
*       K4:洗衣启动开关
*       蜂鸣器:按键操作提示音
*       rgb灯：系统运行指示灯
*       继电器1:注水控制开关
*       继电器2:放水控制开关
*       步进电机：洗衣机电机
*       LCD屏幕:洗衣机信息显示界面
* 修改：
* 注释：系统运行时使用的是标准的1S延时，运行分钟会让实验效果不明显，可将334行的120和119更改为12和11.
*********************************************************************************************/
#include "extend.h"                                             //库函数头文件
#include "delay.h"                                              //延时函数头文件
#include "lcd.h"                                                //屏幕显示头文件
#include "led.h"                                                //led灯驱动头文件
#include "rgb.h"                                                //rgb灯驱动头文件
#include "relay.h"                                              //继电器驱动头文件
#include "buzzer.h"                                             //蜂鸣器驱动头文件
#include "key.h"                                                //按键驱动头文件
#include "stepmotor.h"                                          //步进电机驱动头文件
#include "timer.h"                                              //定时器驱动头文件
#include "screen.h"                                             //屏显头文件
#include "usart.h"                                              //串口头文件

/*********************************************************************************************
* 参数宏定义
*********************************************************************************************/
#define EFFECTIVE_NUM           100                             //按鍵循环监测有效次数
#define MODE_NUM                0x10                            //有效模式数
#define WATER_LEVEL             7                               //水位等级设置

#define FORWARD                 0x03                            //电机正转控制指令
#define RAVERSAL                0x01                            //电机反转控制指令
#define MOTOR_STOP              0x00                            //电机停止指令

#define SOAK_TIM                20                              //浸泡时长参数
#define CLEAN_TIM               10                              //清洗时长参数
#define DEHYD_TIM               5                               //脱水时长参数
    
#define SOAK_TIME               SOAK_TIM                        //浸泡时长
#define WASH_TIME               (CLEAN_TIM * 2)                 //洗衣时长
#define RINS_TIME               (CLEAN_TIM * 3)                 //漂洗时长
#define DEHY_TIME               DEHYD_TIM                       //脱水时长
//全功能耗时时长
#define TOTAL_TIME              (SOAK_TIME + WASH_TIME + RINS_TIME + DEHY_TIME * 4)

#define PUTOUT_SCREEN_DELAY     10000                           //熄屏计数阈值,单位ms

/*********************************************************************************************
* 系统标志位
*********************************************************************************************/
uint8_t rgb_twinkle;                                            //RGB灯闪烁执行标志位
uint8_t mode_twinkle;                                           //模式图标闪烁执行标志位
uint8_t tim_second;                                             //秒信号时钟执行标志位

/*********************************************************************************************
* 系统通用数组 1：注水，2：放水，3：浸泡，4：洗涤，5：脱水
*********************************************************************************************/
uint8_t soak_program[] = {1, 3, 2};                             //浸泡功能流程设置
uint8_t wash_program[] = {1, 4, 4, 2};                          //洗衣功能流程设置
uint8_t rins_program[] = {1, 4 ,2, 1, 4 ,2, 1, 4 ,2};           //清洗功能流程设置
uint8_t dehy_program[] = {2, 5};                                //脱水功能流程设置
//全功能洗衣流程设置
uint8_t total_program[]= {1, 3, 4, 4, 2, 5, 1, 4, 2, 5, 1, 4, 2, 5, 1, 4 ,2, 5};

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
  
  timer_init();                                                 //系统时钟节拍初始化
  led_init();                                                   //LED初始化
  rgb_init();                                                   //rgb灯初始化
  key_init();                                                   //按键初始化
  buzzer_init();                                                //蜂鸣器初始化
  relay_init();                                                 //继电器初始化
  stepmotor_init();                                             //步进电机初始化
  
  lcd_init(NO_NAME);                                            //LCD初始化
}

/*********************************************************************************************
* 名称：main()
* 功能：实训三项目逻辑主体
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int main(void)
{
  uint16_t total_count = 0;                                     //总开关循环监测计数参数
  uint8_t total_switch = 0;                                     //总开关状态标志位
  uint8_t total_flag = 0;                                       //总开关切换标志位
    
  uint16_t mode_count = 0;                                      //模式功能按键循环监测计数参数
  uint8_t mode_select = 0;                                      //模式选择标志位
  uint8_t mode_status = 0;                                      //模式图标状态标志位
  uint8_t mode_temp = 0;                                        //选择模式缓存参数
  uint8_t mode_flag = 0;                                        //模式主动配置切换标志位
  uint8_t *mode_confirm;                                        //洗衣机功能执行指针
  uint8_t mode_step = 0;                                        //洗衣机功能执行动作切换标志位
  uint8_t diffe_step = 0;                                       //洗衣机当前执行动作状态指示标志位
  
  uint16_t water_count = 0;                                     //水位配置按钮循环监测计数参数
  uint8_t water_level = 0;                                      //水温配置选择标志位
  uint8_t level_flag = 0;                                       //水位配置切换标志位
  
  uint16_t startup_count = 0;                                   //启动按钮循环检测计数参数
  uint8_t startup_switch = 0;                                   //启动按钮状态标志位
  uint8_t startup_flag = 0;                                     //启动按钮状态切换标志位
  
  uint8_t rgb_level = 0;                                        //rgb灯状态切换标志位
  
  uint8_t sec_twinkle = 0;                                      //秒图标栓说标志位
  
  uint16_t total_time = 0;                                      //程序总时间参数
  uint16_t diffe_time = 0;                                      //洗衣机功能内动作时间记录参数
  uint16_t timing_flag = 0;                                     //洗衣机动作切换标志位
  
  uint8_t work_flag = 0;                                        //洗衣机工作执行标志位

  uint8_t end_flag = 0;                                         //洗衣机工作完成标志位
  
  uint8_t add_water_flag = 0;                                   //洗衣机注水标志位
  uint8_t sub_water_flag = 0;                                   //洗衣机放水标志位
  uint8_t soaking_flag = 0;                                     //浸泡标志位
  uint8_t washing_flag = 0;                                     //清洗标志位
  uint8_t dehydration_flag = 0;                                 //脱水标志位
  
  uint8_t motor_dir = 0;                                        //电机正反转控制标志位
  
  uint32_t shutdown_st = 0;                                     //熄屏起始时间

  /*设备硬件初始化*/
  hardware_init();                                              //硬件初始化
  
  /*设备初始状态配置*/
  RELAY1_CTRL(OFF);                                             //关闭注水继电器
  RELAY2_CTRL(OFF);                                             //关闭放水继电器
  stepmotor_control(MOTOR_STOP);                                //关闭电机
  
  /*屏显信息初始化*/
  show_logo();                                                  //打印设备logo图标
  show_switch(OFF);                                             //打印系统总开关图标
  show_soak(ON);                                                //打印浸泡功能图标
  show_wash(ON);                                                //打印洗衣功能图标
  show_rinse(ON);                                               //打印清洗功能图标
  show_dehyd(ON);                                               //打印脱水功能图标
  show_all_mode(ON);                                            //打印全功能图标
  show_timer(0, 00);                                            //打印系统执行时间信息
  show_level(0);                                                //打印水位值信息
  show_point(ON);                                               //打印秒点图标

  /*程序主函数*/
  while(1){                                                     //主循环
    /*总开关按键状态检测及开关状态赋值*/
    if(key_status(K1) == 0) total_count ++;                     //循环检测按键按下状态并计数
    if(key_status(K1) && (total_count > EFFECTIVE_NUM)){        //当按键抬起，检测按键按下时长并检测有效性
      if(total_switch == 0) total_switch = 1;                   //如果总开关为关闭则开启
      else total_switch = 0;                                    //否则总开关打开
      total_count = 0;                                          //总开关按键循环检测参数清零
      buzzer_tweet();                                           //蜂鸣器鸣叫
      total_flag = 1;                                           //总开关按键状态切换标志位置位
    }

    /*模式选择按键状态检测及模式赋值*/
    if(total_switch && !work_flag){                             //总开关是否打开、洗衣程序是否开始执行
      if(key_status(K2) == 0) mode_count ++;                    //循环检测模式选择按键按下状态并计数
      if(key_status(K2) && (mode_count > EFFECTIVE_NUM)){       //如果按键抬起，检测按键按下时长并检测有效性
        //如果模式小于设定模式数，或者模式状态为0，那么模式置一
        if((mode_select >= MODE_NUM) || (mode_select == 0)) mode_select = 1;
        else mode_select <<= 1;                                 //否则模式左移一位
        mode_count = 0;                                         //开关循环检测清零
        buzzer_tweet();                                         //蜂鸣器鸣响
        mode_flag = 1;                                          //模式设置切换标志位置位
      }
    }
    /*水位配置开关状态检测及模式赋值*/
    if(total_switch && !work_flag){                             //总开关是否开启，洗衣程序是否启动
      if(key_status(K3) == 0) water_count ++;                   //循环检测水位设置按键是否按下并计数
      if(key_status(K3) && (water_count > EFFECTIVE_NUM)){      //如果按键抬起，检测按键按下时长并检测有效性
        if(water_level > WATER_LEVEL) water_level = 1;          //如果水位标志位大于设定水位，则水位状态标志位置位
        else water_level ++;                                    //否则水位值加一
        water_count = 0;                                        //水位配置按键循环检测参数清零
        buzzer_tweet();                                         //蜂鸣器鸣叫
        level_flag = 1;                                         //水位状态切换标志位置位
        show_level(water_level * 5);                            //屏幕显示设置水位信息
      }
    }
    /*洗衣程序启动开关检测及模式赋值*/
    if(total_switch){                                           //总开关是否打开
      if(key_status(K4) == 0) startup_count ++;                 //如果启动开关是否按下并计数
      if(key_status(K4) && (startup_count > EFFECTIVE_NUM)){    //如果按键抬起，检测按键按下实诚并检测有效性
        if(startup_switch == 0) startup_switch = 1;             //如果程序启动状态为0，则将状态置位     
        else startup_switch = 0;                                //否则程序启动状态复位
        startup_count = 0;                                      //计数值清零
        buzzer_tweet();                                         //蜂鸣器鸣叫
        startup_flag = 1;                                       //程序启动状态切换标志位置位
      }
    }
    /*熄屏控制*/
    if(total_switch){                                           //如果总开关打开
      if(total_flag){                                           //如果总开关状态切换标志位置位
        LCD_BL(1);                                              //则屏幕打开
      }
      shutdown_st = xtime();
    }else{
      if(xtime()-shutdown_st>PUTOUT_SCREEN_DELAY){
        shutdown_st = xtime();
        LCD_BL(0);                                              //如果计数值达到了设定的延时时间测熄屏
      }
    }
    /*总开关状态判断及系统控制*/
    if(total_switch && !end_flag){                              //系统总开关是否开启且洗衣结束标志位复位    
      if(total_flag){                                           //如果总开关状态切换标志位置位
        mode_select = 0;                                        //模式选择复位
        water_level = 0;                                        //水位配置复位
        startup_switch = 0;                                     //程序启动复位
        show_switch(ON);                                        //总开关图标点亮
      }                         
    }else{                                      
      if(total_flag || end_flag){                               //如果总开关闭，或程序结束标志位置位
        mode_select = 0;                                        //模式选择复位
        water_level = 0;                                        //水位设置复位
        startup_switch = 0;                                     //启动开关复位
        total_time = 0;                                         //程序总时间清零
        work_flag = 0;                                          //工作标志位复位
        rgb_ctrl(0);                                            //RGB灯熄灭
        RELAY1_CTRL(OFF);                                       //注水电磁阀关闭
        RELAY2_CTRL(OFF);                                       //放水电磁阀关闭
        stepmotor_control(MOTOR_STOP);                          //步进电机控制关闭
        show_timer(0, 00);                                      //时间显示清零
        show_level(0);                                          //水位显示清零
        show_point(ON);                                         //秒点显示
        show_soak(ON);                                          //浸泡图标常亮
        show_wash(ON);                                          //洗衣标志常亮
        show_rinse(ON);                                         //清洗标志常亮
        show_dehyd(ON);                                         //脱水标志常亮
        show_all_mode(ON);                                      //全功能标志常亮
        if(end_flag){                                           //如果程序执行结束标志置位
          for(char i=0; i<8; i++){                              
            buzzer_tweet();                                     //蜂鸣器鸣响8次
            delay_ms(1000);                                     //延时1S
          }
          end_flag = 0;                                         //程序结束标志位复位
        }else show_switch(OFF);                                 //否则系统总开关图标显示关闭
      }
    }   
    /*系统运行RGB指示灯控制*/
    if(total_switch){                                           //控开关是否开启
      if(rgb_twinkle){                                          //RGB灯闪烁控执行标志位是否置位
        rgb_ctrl(rgb_level % 7 + 1);                            //RGB灯闪烁状态控制标志位
        rgb_level ++;                                           //RGB灯闪烁控制标志位加一
        rgb_twinkle = 0;                                        //RGB灯闪烁执行标志位清空   
      }
    }
    /*获取洗衣程序执行时间和获取模式配置标志*/
    if(!work_flag){                                             //如果程序没有执行
      if(mode_flag || level_flag){                              //如果模式切换标志位或水位切换标志位置位
        switch(mode_select){                                    //判断当前设置的模式
        case 0x01:                                              //如果为浸泡模式
          total_time = water_level * 2 + SOAK_TIME;             //计算浸泡模式执行总时间
          mode_confirm = soak_program;                          //获取浸泡模式动作步骤
          break;                                                //跳出
        case 0x02:                                              //如果为模式洗衣
          total_time = water_level * 2 + WASH_TIME;             //计算洗衣模式总时间
          mode_confirm = wash_program;                          //获取洗衣模式执行步骤
          break;                                                //跳出
        case 0x04:                                              //如果为清洗模式
          total_time = water_level * 6 + RINS_TIME;             //计算清洗模式总时间
          mode_confirm = rins_program;                          //获取清洗模式步骤
          break;                                                //跳出
        case 0x08:                                              //如果为脱水模式
          total_time = water_level + DEHY_TIME;                 //计算脱水模式总时间
          mode_confirm = dehy_program;                          //获取脱水模式步骤
          break;                                                //跳出
        case 0x10:                                              //如果为全功能模式
          total_time = water_level * 8 + TOTAL_TIME;            //计算全功能模式总时间
          mode_confirm = total_program;                         //获取全功能模式步骤
          break;                                                //跳出
        }
      }
    }
    /*洗衣机功能图标显示控制*/
    if(mode_twinkle && mode_select){                            //图标闪烁执行标志位是否置位，模式是否被设置
      mode_twinkle = 0;                                         //图标闪烁标志位复位
      if(!mode_status){                                         //如果模式及图标状态标志位复位
        mode_temp = mode_select;                                //存储当前洗衣机功能模式
        mode_status = 1;                                        //图标状态标志位置位
        show_timer((total_time / 60), (total_time % 60));       //更新显示时间        
      }else mode_status = 0;                                    //图标状态标志位复位
      switch(mode_temp){                                        //模式缓存状态
      case 0x01:                                                //如果为浸泡模式
        show_soak(mode_status);                                 //浸泡模式图标执行闪烁
        break;                                                  //跳出
      case 0x02:                                                //如果为洗衣模式
        show_wash(mode_status);                                 //洗衣模式图标执行闪烁
        break;                                                  //跳出
      case 0x04:                                                //如果为清洗模式
        show_rinse(mode_status);                                //清洗模式图标执行闪烁
        break;                                                  //跳出
      case 0x08:                                                //如果为脱水模式
        show_dehyd(mode_status);                                //脱水模式图标执行闪烁
        break;                                                  //跳出
      case 0x10:                                                //如果为全功能模式
        show_all_mode(mode_status);                             //全功能模式图标执行闪烁
        break;                                                  //跳出
      }
    }
    /*洗衣程序启动判断和标志位设置*/
    if(startup_switch){                                         //启动开关是否打开
      if(startup_flag){                                         //启动开关状态标志位是否置位
        if(water_level && mode_select){                         //水位和模式选择是否不为零
          work_flag = 1;                                        //工作执行标志位置位
          timing_flag = 1;                                      //时间跳变标志位置位
        }else work_flag = 0;                                    //否则工作执行标志位复位
      }
    }else{
      if(startup_flag){                                         //如果启动开关状态标志位置位
        work_flag = 0;                                          //工作执行标志位复位
        RELAY1_CTRL(OFF);                                       //注水电磁阀关闭
        RELAY1_CTRL(OFF);                                       //放水电磁阀关闭
        stepmotor_control(MOTOR_STOP);                          //步进电机控制关闭
      }
    }
    /*洗衣机秒信号动作执行*/
    if(tim_second){                                             //秒信号执行标志位是否置位
      if(work_flag){                                            //工作标志位是否置位
        tim_second = 0;                                         //秒信号执行标志位复位
        show_point(ON);                                         //秒点图标点亮
        sec_twinkle ++;                                         //秒闪烁参数加一
        if((sec_twinkle % 2) == 1){                             //是否达到一秒
          if((sec_twinkle % 120) == 119){                       //是否达到一分钟
            if(total_time == 0){                                //执行总时间是否执行完成
              work_flag = 0;                                    //工作标志位复位
              end_flag = 1;                                     //工作结束标志位置位
            }else{
              total_time -= 1;                                  //工作时间减一
              diffe_time += 1;                                  //洗衣动作时间加一
              timing_flag = 1;                                  //时钟跳变标志位置位
            }
            sec_twinkle = 0;                                    //秒闪参数清零
          }
          show_point(OFF);                                      //秒点图标熄灭
        }
      }
      else show_point(ON);                                      //否则秒点图标点亮
    }
    /*洗衣机洗衣步骤执行操作*/
    if(work_flag){                                              //如果工作标志位置位
      if(timing_flag){                                          //如果时间跳变标志位置位
        do{     
          mode_step = 0;                                        //模式步骤标志位复位
          switch(*(mode_confirm + diffe_step)){                 //岸段当前洗衣机要执行的动作
          case 1:                                               //如果为注水动作
            if(diffe_time < water_level){                       //动作执行时间是否达到设定动作时间
              add_water_flag = 1;                               //注水标志位置位
            }else{
              add_water_flag = 0;                               //注水标志位复位
              diffe_time = 0;                                   //当前动作计时结束
              diffe_step += 1;                                  //模式执行动作加一
              mode_step = 1;                                    //模式动作置位
            }
            break;                                              //跳出
          case 2:                                               //如果为放水动作
            if(diffe_time < water_level){                       //动作执行时间是否达到设定动作时间
              sub_water_flag = 1;                               //放水标志位置位
            }else{
              sub_water_flag = 0;                               //放水标志位复位
              diffe_time = 0;                                   //当前动作计时结束
              diffe_step += 1;                                  //模式执行动作加一
              mode_step = 1;                                    //模式动作置位
            }
            break;                                              //跳出
          case 3:                                               //如果为浸泡动作
            if(diffe_time < SOAK_TIM){                          //动作执行时间是否达到设定动作时间
              soaking_flag = 1;                                 //浸泡标准位置位
            }else{
              soaking_flag = 0;                                 //浸泡标志位复位
              diffe_time = 0;                                   //当前动作计时结束
              diffe_step += 1;                                  //模式执行动作加一
              mode_step = 1;                                    //模式动作置位
            }
            break;                                              //跳出
          case 4:                                               //如果为洗衣动作
            if(diffe_time < CLEAN_TIM){                         //动作执行时间是否达到设定动作时间
              washing_flag = 1;                                 //洗衣标志位置位
            }else{
              washing_flag = 0;                                 //洗衣标志位复位
              diffe_time = 0;                                   //当前动作计时结束
              diffe_step += 1;                                  //模式执行动作加一
              mode_step = 1;                                    //模式动作置位
            }
            break;                                              //跳出
          case 5:                                               //如果为脱水动作
            if(diffe_time < DEHYD_TIM){                         //动作执行时间是否达到设定动作时间
              dehydration_flag = 1;                             //脱水动作标志位置位
            }else{
              dehydration_flag = 0;                             //脱水动作标志位复位
              diffe_time = 0;                                   //当前动作计时结束
              diffe_step += 1;                                  //模式执行动作加一
              mode_step = 1;                                    //模式动作置位
            }
            break;                                              //跳出
          }
        }while(mode_step);                                      //模式执行标志位置位执行
      }
    }
    /*洗衣机相关硬件设备控制*/
    if(work_flag){                                              //如果工作标志位置位
      if(timing_flag){                                          //如果秒跳变标志位置位
        if(diffe_time == 0){                                    //如果当前执行动作执行完成
          RELAY1_CTRL(OFF);                                     //关闭注水继电器
          RELAY2_CTRL(OFF);                                     //关闭放水继电器
          stepmotor_control(MOTOR_STOP);                        //关闭步进电机
        }

        if(add_water_flag) RELAY1_CTRL(ON);                     //如果注水，打开注水继电器
        
        if(sub_water_flag) RELAY2_CTRL(ON);                     //如果放水，打开放水继电器
        
        if(soaking_flag) stepmotor_control(MOTOR_STOP);         //如果浸泡，关闭电机
        
        if(washing_flag){                                       //如果洗衣
          if(motor_dir){                                        //如果标志位置位
            stepmotor_control(FORWARD);                         //电机正转
            motor_dir = 0;                                      //标志位复位
          }else{
            stepmotor_control(RAVERSAL);                        //否则反转
            motor_dir = 1;                                      //标志位置位
          }
        }
        
        if(dehydration_flag){                                   //如果脱水
          stepmotor_control(FORWARD);                           //放水电机打开
          RELAY2_CTRL(ON);
        }
      }
    }
    
    total_flag = 0;                                             //总开关标志位复位
    mode_flag = 0;                                              //模式切换标志位复位
    level_flag = 0;                                             //水位切换标志位复位
    startup_flag = 0;                                           //启动动作标志位复位
    timing_flag = 0;                                            //秒时钟跳变标志位复位
  }
}
