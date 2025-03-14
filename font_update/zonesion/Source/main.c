#include "extend.h"
#include "delay.h"
#include "user_cmd.h"
#include "usart1.h"
#include "download.h"
#include "w25qxx.h"
#include "led.h"
#include "key.h"
#include "font_config.h"

font_info_t font_info;

// 硬件初始化
static void system_init(void)
{
  sys_cache_enable();                     //打开L1-Cache 强制D-Cache透写*/
  HAL_Init();                             //初始化HAL库 */
  SystemClock_Config();                   //设置时钟, 400Mhz */
  delay_init(400);                        //延时初始化
  /* 初始化 shell 控制台 */
  shell_hw_init(115200);                  // 初始化 控制台串口硬件
  shell_init("shell >" ,usart1_puts);     // 初始化 控制台输出
  shell_input_init(&shell_1,usart1_puts); // 初始化 交互
  welcome_gets(&shell_1,0,0);             // 主动显示 welcome'
  cmdline_gets(&shell_1,"\r",1);          // 一次换行
}

// 硬件初始化
static void hardware_init(void)
{
  led_init();                             //LED初始化
  key_init();                             //按键初始化
  W25QXX_Init();
  READ_FON_INFO;
  debug_info("font_flag:%08x\r\n",font_info.font_flag);
}

// 应用初始化
static void app_init(void)
{
  register_user_cmd();
}

// 主函数
int main(void)
{
  system_init();
  hardware_init();
  app_init();
  while(1){
    led_app(1000000);
    if(usart1_mode==0) 
      shell_app_cycle();
    else font_download(); 
  }
}  
