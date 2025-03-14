/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"

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
  
  lcd_init(LCD);                                                //LCD��ʼ��
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
  hardware_init();                                              //Ӳ����ʼ��
  delay_ms(2000);
  while(1){
    //��һ��
    led_setState(LED3_NUM,0);                                   //�ر�D3 
    led_setState(LED4_NUM,0);                                   //�ر�D4
    LCD_Clear(WHITE);                                           //����
    LCDDrawFont40_Mid(0,LCD_HEIGHT/2-48,
                      "Hello IOT!",LCD_WIDTH,BLACK,WHITE); 
    POINT_COLOR = 0x07e0;
    LCD_DrawLine(0, LCD_HEIGHT/2, LCD_WIDTH, LCD_HEIGHT/2);     //����
    LCDDrawFont40_Mid(0,LCD_HEIGHT/2+8,
                      "����������ƽ̨!",LCD_WIDTH,BLACK,WHITE);
    //�ڶ���
    delay_ms(2000);
    led_setState(LED3_NUM,1);                                   //����D3
    led_setState(LED4_NUM,1);                                   //����D4
    LCD_Clear(WHITE);                                           //����
    LCDDrawFont40_Mid(0, LCD_HEIGHT/2-20, 
                      "STM32H743ZIT6",LCD_WIDTH,BLACK, WHITE);  //��ʾASCII
    delay_ms(2000); 
  }
}  
