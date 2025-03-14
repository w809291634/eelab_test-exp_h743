/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "Relay.h"

/*********************************************************************************************
* ���ƣ�hardware_init()
* ���ܣ�Ӳ����ʼ��
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
static void hardware_init(void)
{
  sys_cache_enable();                                           //��L1-Cache ǿ��D-Cache͸д*/
  HAL_Init();                                                   //��ʼ��HAL�� */
  SystemClock_Config();                                         //����ʱ��, 400Mhz */
  PeriphCommonClock_Config();                                   //��������ʱ�ӳ�ʼ��
  delay_init(400);                                              //��ʱ��ʼ��
  usart_init(115200);                                           //���ڳ�ʼ��
  led_init();                                                   //LED��ʼ��
  key_init();                                                   //������ʼ��
  
  relay_init();                                                 //�̵�����ʼ��
  lcd_init(RELAY);                                              //LCD��ʼ��
}

/*********************************************************************************************
* ���ƣ�main()
* ���ܣ�������
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
int main(void)
{
  unsigned char relay_flag = 0;
  hardware_init();
  while(1){
    if(relay_flag == 0){                                        //�̵���������״̬�����ı�      
      printf("RELAY ON!\r\n");                                  //���ڴ�ӡ��ʾ��Ϣ
      relay_flag = 1;                                           //���¼̵���������״̬
      relay_control(0x03);                                      //�̵����Ŀ��Ʒ����ı�
      led_setState(LED4_NUM,1);
      //LCD��ʾ�̵���״̬��Ϣ
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                    "�̵���1��  \n�̵���2��  ",LCD_WIDTH,BLACK,WHITE);
    }
    else{                                                       
      if(relay_flag == 1){                                      //�̵���������״̬�����ı�
        printf("RELAY OFF!\r\n");                               //���ڴ�ӡ��ʾ��Ϣ
        relay_flag = 0;                                         //���¼̵���������״̬
        relay_control(0x00);                                    //�̵����Ŀ��Ʒ����ı�
        led_setState(LED4_NUM,0);
        //LCD��ʾ�̵���״̬��Ϣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                      "�̵���1��  \n�̵���2��  ",LCD_WIDTH,BLACK,WHITE);
      }
    }
    
    delay_ms(1000);
  }
}  