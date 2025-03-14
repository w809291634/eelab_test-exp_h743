/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"

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
  led_init();                                                   //LED初始化
  key_init();                                                   //按键初始化
  
  lcd_init(UART);                                               //LCD初始化
}

/*********************************************************************************************
* 名称：main()
* 功能：主函数
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
int main(void)
{
  u16 len;                                                      //记录串口接收的长度
  u16 count=0;                                                  //大循环计数
  char buf[100]={0};
  char temp[100]={0};
  hardware_init();                                              //硬件初始化
  printf("Hello IOT! Usart is ready!\r\n");
  while(1){
    if(g_usart_rx_sta&0x8000)                                     //接收完成标志
    {
      len=g_usart_rx_sta&0x3fff;                                  //得到此次接收到的数据长度
      // 上位机端显示发送的消息
      printf("\r\n您发送的消息为:\r\n");
      HAL_UART_Transmit(&g_uart1_handle, (uint8_t *)g_usart_rx_buf, len, 1000); //向串口1发送数据
      while(__HAL_UART_GET_FLAG(&g_uart1_handle, UART_FLAG_TC) != SET);         //等待发送结束
      printf("\r\n\r\n");//插入换行
      // LCD端显示发送的消息
      memcpy(buf,g_usart_rx_buf,len);                           //拷贝串口中接收的数据
      buf[len]=0;                                               //形成合适的字符串
      snprintf(temp,100,"您发送的消息为:%s",buf);
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);    
      count =0;

      g_usart_rx_sta = 0;                                       //数据处理完成，清除数据标记
    }
    if(count>100){                                              //超过1s后，清理显示区域
      count=0;
      LCD_Fill(8+32*2,REF_POS+32+SPACING,LCD_WIDTH,32*3+SPACING*2,WHITE);   
    }
    
    count++;
    led_app(10);                                                //系统运行指示
    delay_ms(10);                                               //延时
  }
}  
