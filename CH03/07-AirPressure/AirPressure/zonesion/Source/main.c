/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "fbm320.h"

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
  
  if(fbm320_init() == 1)                                        //��ѯfbm320״̬
    printf("fbm320 ok!\r\n");
  lcd_init(AIRPRESSURE);                                        //LCD��ʼ��
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
  float temperature = 0;                                        //�洢�¶����ݱ���
  long pressure = 0;                                            //�洢ѹ�����ݱ���
  float altitude = 0.0;                                         //�洢���θ߶ȱ���
  char buff[100];                                               //��ʾ��������
  hardware_init();
  while(1){
    fbm320_data_get(&temperature,&pressure);                    //����¶ȣ�����ѹ������
    altitude =  (101325-pressure)*(100.0f/(101325 - 100131));   //��ú��θ߶�����
    //���¶�����ͨ�����ڴ�ӡ����
    printf("temperature:%.1f��\r\npressure:%0.1fkPa\r\n", temperature,pressure/1000.0f);                  
    printf("altitude:%0.1f m\r\n",altitude);                    //�����θ߶�����ͨ�����ڴ�ӡ����
    sprintf(buff,"��  �ȣ�%.1f��   \n����ѹ��%.1fkPa   \n��  �Σ�%.1f m   ",
            temperature,pressure/1000.0f,altitude);             //LCD����ʾ�¶� 
    LCDShowFont32(8+32*2,REF_POS+32+5,buff,LCD_WIDTH,BLACK,WHITE); 

    led_app(1000);
    delay_ms(1000);
  }
}  