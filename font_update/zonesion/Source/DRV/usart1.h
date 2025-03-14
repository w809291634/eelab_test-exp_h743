#ifndef __USART1_H
#define __USART1_H
#include "stdio.h"	
#include "extend.h"
#include "shell.h"

extern char usart1_mode;
extern shellinput_t shell_1;

void usart1_flush(void);
uint16_t UART_GetRemainDate(char* data, char* ringbuf, uint16_t size, uint16_t w_index, uint16_t r_index) ;
void shell_hw_init(uint32_t baudrate);
int usart1_getchar(uint8_t* data);
void usart1_put(const char data);
void usart1_puts(const char * strbuf, unsigned short len);
void shell_app_cycle(void);
#endif
