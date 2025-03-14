/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "ServoMotor.h"

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
  
  ServoMotor_init(20000,200-1);                                 //�ŷ������ʼ��,200M/200=1mhz,20msPWM��
  lcd_init(SERVOMOTOR);                                         //LCD��ʼ��
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
  s16 angle=0;                                                  //�洢��ǰ���ƽǶ�
  s16 last_angle=-1;                                            //�洢��һ�ο��ƽǶ�
  char temp[100]={0};                                           //����ת���洢
  hardware_init();                                              //Ӳ����ʼ��
  while(1){
    u8 key =  KEY_Scan(0);                                      //������ֵɨ��
    if(key == K1_PRES) angle = angle+30;
    else if(key == K2_PRES) angle = angle-30;                   //ͨ���������ƽǶ�
    if(angle > 180) angle=180;                                  //��Χ����
    if(angle < 0) angle=0;
    if(angle!=last_angle){
      last_angle = angle;
      Servo_control(angle);                                     //���ƶ����һ���Ƕ�
      // ������ʾ
      sprintf(temp,"����Ƕ�:%d��   \r\n",angle);                //������ʾ
      printf(temp);
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);
    }

    led_app(10);
    delay_ms(10);
  }
}  