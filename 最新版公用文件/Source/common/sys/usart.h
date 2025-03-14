#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "extend.h"
#include "string.h"

/**************************user config*************************/
#define RXBUFFERSIZE                    1      /* 缓存大小 */
#define USART_REC_LEN                   200    /* 定义最大接收字节数 200 */
#define USART_EN_RX                     1      /* 使能（1）/禁止（0）串口1接收 */

extern uint8_t g_usart_rx_buf[USART_REC_LEN];
extern uint16_t g_usart_rx_sta ;
extern UART_HandleTypeDef g_uart1_handle;          /* UART句柄 */

void usart_init(uint32_t baudrate);
void usart1_put(const char data);
void usart1_puts(const char * strbuf, unsigned short len);
void shell_app_cycle(void);
#endif
