/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "dcmotor.h"

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
  
  dcmotor_init();                                               //��ʼ�����
  lcd_init(DCMOTOR);                                            //LCD��ʼ��
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
  u8 mode=0;                                                    //�������ģʽ��0ֹͣ��1��ת��2��ת
  u8 count;
  char buf[100];                                                //����LCD��ʾ��Ϣ
  hardware_init();
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,                      //��ʾ��ʼ��Ϣ
                "���ֹͣ\n���ת�٣�0.00000r/s   ",LCD_WIDTH,BLACK,WHITE);
  while(1){
    u8 key= KEY_Scan(0);                                        //����ɨ��
    if(key==K1_PRES){                                           //����1������ת
      if(mode>0) mode=0;                                        //�����ǰ���������ת���߷�ת��ֹͣ���
      else mode=1;
    }else if(key==K2_PRES){                                     //����2���Ʒ�ת
      if(mode>0)mode=0;
      else mode=2;
    }
    if(key){                                                    //����ģʽ���е���������
      if(mode==0)DCMOTOR_STOP;
      if(mode==1)DCMOTOR_FORWARD;
      if(mode==2)DCMOTOR_BACKWARD;
    }
    if(count>10){                                               //������ʾ���ת��
      count=0;
      switch(mode)                                              //���ݾ����ģʽ������ʾ
      {
      case 0:
        sprintf(buf,"���ֹͣ\n���ת�٣�%.5fr/s   ",dcmotor_speed);
        printf("motor stop,motor speed:%.5fr/s\r\n",dcmotor_speed);
        break;
      case 1:
        sprintf(buf,"�����ת\n���ת�٣�%.5fr/s   ",dcmotor_speed);
        printf("motor forward,motor speed:%.5fr/s\r\n",dcmotor_speed);
        break;
      case 2:
        sprintf(buf,"�����ת\n���ת�٣�%.5fr/s   ",dcmotor_speed);
        printf("motor backward,motor speed:%.5fr/s\r\n",dcmotor_speed);
        break;
      }
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,buf,LCD_WIDTH,BLACK,WHITE);
    }

    count++;                                                    //���ڼ���
    led_app(10);                                                //ϵͳ������ʾ
    delay_ms(10);                                               //��ʱʱ��
  }
}  