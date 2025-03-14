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

// Ӳ����ʼ��
static void system_init(void)
{
  sys_cache_enable();                     //��L1-Cache ǿ��D-Cache͸д*/
  HAL_Init();                             //��ʼ��HAL�� */
  SystemClock_Config();                   //����ʱ��, 400Mhz */
  delay_init(400);                        //��ʱ��ʼ��
  /* ��ʼ�� shell ����̨ */
  shell_hw_init(115200);                  // ��ʼ�� ����̨����Ӳ��
  shell_init("shell >" ,usart1_puts);     // ��ʼ�� ����̨���
  shell_input_init(&shell_1,usart1_puts); // ��ʼ�� ����
  welcome_gets(&shell_1,0,0);             // ������ʾ welcome'
  cmdline_gets(&shell_1,"\r",1);          // һ�λ���
}

// Ӳ����ʼ��
static void hardware_init(void)
{
  led_init();                             //LED��ʼ��
  key_init();                             //������ʼ��
  W25QXX_Init();
  READ_FON_INFO;
  debug_info("font_flag:%08x\r\n",font_info.font_flag);
}

// Ӧ�ó�ʼ��
static void app_init(void)
{
  register_user_cmd();
}

// ������
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
