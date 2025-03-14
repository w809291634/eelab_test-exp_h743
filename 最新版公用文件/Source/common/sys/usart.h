#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "extend.h"
#include "string.h"

/**************************user config*************************/
#define RXBUFFERSIZE                    1      /* �����С */
#define USART_REC_LEN                   200    /* �����������ֽ��� 200 */
#define USART_EN_RX                     1      /* ʹ�ܣ�1��/��ֹ��0������1���� */

extern uint8_t g_usart_rx_buf[USART_REC_LEN];
extern uint16_t g_usart_rx_sta ;
extern UART_HandleTypeDef g_uart1_handle;          /* UART��� */

void usart_init(uint32_t baudrate);
void usart1_put(const char data);
void usart1_puts(const char * strbuf, unsigned short len);
void shell_app_cycle(void);
#endif
