/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "lock.h"

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
  
  lock_init();                                                  //��ʼ������
  lcd_init(LOCK);                                               //LCD��ʼ��
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
  char open=0;
  hardware_init();                                              //Ӳ����ʼ��
  while(1){
    u8 key = KEY_Scan(0);                                       //��ֵɨ��
    if(key==K1_PRES){
      open = !open;                                             //��־λȡ��
      if(open){
        LOCK_OPEN;                                              //������
        printf("������\r\n");
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"������",LCD_WIDTH,BLACK,WHITE); 
      }else{
        LOCK_CLOSE;                                             //�ر�����
        printf("�����ر�\r\n");
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"�����ر�",LCD_WIDTH,BLACK,WHITE); 
      }
    }
    
    led_app(10);
    delay_ms(10);
  }
}  