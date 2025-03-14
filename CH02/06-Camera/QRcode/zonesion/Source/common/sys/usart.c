#include "usart.h"
#include "stdio.h"

/**************************user config*************************/
// 定义控制台调试串口的硬件
// 串口1 引脚配置
#define USART_TX_GPIO_PORT              GPIOA
#define USART_TX_GPIO_PIN               GPIO_PIN_9
#define USART_TX_GPIO_AF                GPIO_AF7_USART1
#define USART_TX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* 发送引脚时钟使能 */

#define USART_RX_GPIO_PORT              GPIOA
#define USART_RX_GPIO_PIN               GPIO_PIN_10
#define USART_RX_GPIO_AF                GPIO_AF7_USART1
#define USART_RX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* 接收引脚时钟使能 */

#define USART_UX                        USART1
#define USART_UX_IRQn                   USART1_IRQn
#define USART_UX_IRQHandler             USART1_IRQHandler
#define USART_UX_CLK_ENABLE()           do{ __HAL_RCC_USART1_CLK_ENABLE(); }while(0)  /* USART1 时钟使能 */

/**************************user config*************************/
// hal库串口使用
/* 接收缓冲, 最大USART_REC_LEN个字节. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
uint16_t g_usart_rx_sta = 0;
volatile uint8_t g_rx_buffer[RXBUFFERSIZE]; /* HAL库使用的串口接收缓冲 */
UART_HandleTypeDef g_uart1_handle;          /* UART句柄 */

/*********************************************************************************************
* 名称:usart_init
* 功能:串口初始化函数
* 参数:baudrate: 波特率, 根据自己需要设置波特率值
* 返回:无
* 修改:无
* 注释:以下是SPI模块的初始化代码，配置成主机模式
*        这里针是对SPI1的初始化
*********************************************************************************************/
void usart_init(uint32_t baudrate)
{
    g_uart1_handle.Instance = USART_UX;                    /* USART1 */
    g_uart1_handle.Init.BaudRate = baudrate;               /* 波特率 */
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B;   /* 字长为8位数据格式 */
    g_uart1_handle.Init.StopBits = UART_STOPBITS_1;        /* 一个停止位 */
    g_uart1_handle.Init.Parity = UART_PARITY_NONE;         /* 无奇偶校验位 */
    g_uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;   /* 无硬件流控 */
    g_uart1_handle.Init.Mode = UART_MODE_TX_RX;            /* 收发模式 */
    HAL_UART_Init(&g_uart1_handle);                        /* HAL_UART_Init()会使能UART1 */
    
    /* 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量 */
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
}

/*********************************************************************************************
* 名称:HAL_UART_MspInit
* 功能:UART底层初始化函数
* 参数:huart: UART句柄类型指针
* 返回:无
* 修改:无
* 注释:此函数会被HAL_UART_Init()调用
 *       完成时钟使能，引脚配置，中断配置
*********************************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;
    if (huart->Instance == USART_UX)                                /* 如果是串口1，进行串口1 MSP初始化 */
    {
        USART_UX_CLK_ENABLE();                                      /* USART1 时钟使能 */
        USART_TX_GPIO_CLK_ENABLE();                                 /* 发送引脚时钟使能 */
        USART_RX_GPIO_CLK_ENABLE();                                 /* 接收引脚时钟使能 */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;                   /* TX引脚 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                    /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
        gpio_init_struct.Alternate = USART_TX_GPIO_AF;              /* 复用为USART1 */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);       /* 初始化发送引脚 */

        gpio_init_struct.Pin = USART_RX_GPIO_PIN;                   /* RX引脚 */
        gpio_init_struct.Alternate = USART_RX_GPIO_AF;              /* 复用为USART1 */
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);       /* 初始化接收引脚 */

#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);                          /* 使能USART1中断通道 */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 1, 0);                  /* 抢占优先级1，子优先级0 */
#endif
    }
}

/*********************************************************************************************
* 名称:usart1_put
* 功能:串口1的发送函数
* 参数:无
* 返回:无
* 修改:无
* 注释:发送字符
*********************************************************************************************/
void usart1_put(const char data)
{
  while ((USART_UX->ISR & 0X40) == 0);    /* 等待上一个字符发送完成 */
  USART_UX->TDR = (uint8_t)data;          /* 将要发送的字符 ch 写入到DR寄存器 */
}

/*********************************************************************************************
* 名称:usart1_puts
* 功能:串口1的发送函数
* 参数:无
* 返回:无
* 修改:无
* 注释:发送字符串
*********************************************************************************************/
void usart1_puts(const char * strbuf, unsigned short len)
{
  while(len--)
  {
    while ((USART_UX->ISR & 0X40) == 0);    /* 等待上一个字符发送完成 */
    USART_UX->TDR = (uint8_t)*strbuf;       /* 将要发送的字符 ch 写入到DR寄存器 */
    strbuf++ ;
  }
}

/*********************************************************************************************
* 名称:usart1_puts
* 功能:Rx传输回调函数
* 参数:huart: UART句柄类型指针
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)                             /* 如果是串口1 */
    {
        if ((g_usart_rx_sta & 0x8000) == 0)                    /* 接收未完成 */
        {
            if (g_usart_rx_sta & 0x4000)                       /* 接收到了0x0d */
            {
                if (g_rx_buffer[0] != 0x0a) 
                {
                    g_usart_rx_sta = 0;                       /* 接收错误,重新开始 */
                }
                else 
                {
                    g_usart_rx_sta |= 0x8000;                 /* 接收完成了 */
                }
            }
            else                                              /* 还没收到0X0D */
            {
                if(g_rx_buffer[0] == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = g_rx_buffer[0] ;
                    g_usart_rx_sta++;
                    if(g_usart_rx_sta > (USART_REC_LEN - 1))
                    {
                        g_usart_rx_sta = 0;                   /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
    }
}

/*********************************************************************************************
* 名称:USART_UX_IRQHandler
* 功能:串口1中断服务函数
* 参数:huart: UART句柄类型指针
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void USART_UX_IRQHandler(void)
{ 
  HAL_UART_IRQHandler(&g_uart1_handle);     /* 调用HAL库中断处理公用函数 */
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
  usart1_put(ch);
  return ch;
}
