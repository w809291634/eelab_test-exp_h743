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
  
  lcd_init(UART);                                               //LCD��ʼ��
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
  u16 len;                                                      //��¼���ڽ��յĳ���
  u16 count=0;                                                  //��ѭ������
  char buf[100]={0};
  char temp[100]={0};
  hardware_init();                                              //Ӳ����ʼ��
  printf("Hello IOT! Usart is ready!\r\n");
  while(1){
    if(g_usart_rx_sta&0x8000)                                     //������ɱ�־
    {
      len=g_usart_rx_sta&0x3fff;                                  //�õ��˴ν��յ������ݳ���
      // ��λ������ʾ���͵���Ϣ
      printf("\r\n�����͵���ϢΪ:\r\n");
      HAL_UART_Transmit(&g_uart1_handle, (uint8_t *)g_usart_rx_buf, len, 1000); //�򴮿�1��������
      while(__HAL_UART_GET_FLAG(&g_uart1_handle, UART_FLAG_TC) != SET);         //�ȴ����ͽ���
      printf("\r\n\r\n");//���뻻��
      // LCD����ʾ���͵���Ϣ
      memcpy(buf,g_usart_rx_buf,len);                           //���������н��յ�����
      buf[len]=0;                                               //�γɺ��ʵ��ַ���
      snprintf(temp,100,"�����͵���ϢΪ:%s",buf);
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);    
      count =0;

      g_usart_rx_sta = 0;                                       //���ݴ�����ɣ�������ݱ��
    }
    if(count>100){                                              //����1s��������ʾ����
      count=0;
      LCD_Fill(8+32*2,REF_POS+32+SPACING,LCD_WIDTH,32*3+SPACING*2,WHITE);   
    }
    
    count++;
    led_app(10);                                                //ϵͳ����ָʾ
    delay_ms(10);                                               //��ʱ
  }
}  
