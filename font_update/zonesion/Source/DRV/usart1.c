#include "usart1.h"
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

#define USARTx_RINGBUF_SIZE             (256)     
#define RXBUFFERSIZE                    1      /* 缓存大小 */
#define USART_EN_RX                     1      /* 使能（1）/禁止（0）串口1接收 */
#define USE_UX_REGISTER                 0      // 是否使用寄存器来进行串口数据获取
/**************************user config*************************/

// shell 终端使用变量
shellinput_t shell_1;
static char shell_ringbuf[USARTx_RINGBUF_SIZE]={0};
static volatile unsigned short Read_Index,Write_Index;

char usart1_mode=0;                         // 串口的数据流 工作指向  0：数据到达shell控制台  1：数据到达IAP更新

// hal库串口使用
volatile uint8_t g_rx_buffer[RXBUFFERSIZE]; /* HAL库使用的串口接收缓冲 */
UART_HandleTypeDef g_uart1_handle;          /* UART句柄 */

/**
 * @brief       串口X初始化函数
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @note        注意: 必须设置正确的时钟源, 否则串口波特率就会设置异常.
 *              这里的USART的时钟源SystemClock_Config()函数中已经设置过了.
 * @retval      无
 */
void shell_hw_init(uint32_t baudrate)
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

/**
 * @brief       UART底层初始化函数
 * @param       huart: UART句柄类型指针
 * @note        此函数会被HAL_UART_Init()调用
 *              完成时钟使能，引脚配置，中断配置
 * @retval      无
 */
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

// 清空数据
void usart1_flush(void)
{
  Read_Index=Write_Index;
}

// 串口1的发送函数
// 发送字符
void usart1_put(const char data)
{
  while ((USART_UX->ISR & 0X40) == 0);    /* 等待上一个字符发送完成 */
  USART_UX->TDR = (uint8_t)data;          /* 将要发送的字符 ch 写入到DR寄存器 */
}

// 串口1的发送函数
// 发送字符串
void usart1_puts(const char * strbuf, unsigned short len)
{
  while(len--)
  {
    while ((USART_UX->ISR & 0X40) == 0);    /* 等待上一个字符发送完成 */
    USART_UX->TDR = (uint8_t)*strbuf;       /* 将要发送的字符 ch 写入到DR寄存器 */
    strbuf++ ;
  }
}

/* 获取缓冲区中剩余未读取数据 */
// data 读取剩余数据
// ringbuf 当前环形缓存区 size 缓存区大小 
// w_index r_index 当前缓存区的读写索引
// 返回 读取长度
uint16_t UART_GetRemainDate(char* data, char* ringbuf, uint16_t size, uint16_t w_index, uint16_t r_index) 
{
  //获取剩余数据
  if(w_index >= r_index) { 
    int len1 = w_index - r_index;
    memcpy(data,ringbuf + r_index,len1);
    return len1;
  } else {
    int len1=size-r_index;          // 环形末尾读长度
    int len2=w_index;
    memcpy(data,ringbuf + r_index,len1);
    memcpy(data+len1,ringbuf,len2);
    return len1+len2;
  }
}

// 从缓存区读取一个字节
// 注意：数据处理结果和shell_app_cycle只能选择其一
int usart1_getchar(uint8_t* data)
{
  if(Write_Index!=Read_Index){
    *data=shell_ringbuf[Read_Index];
    Read_Index = (Read_Index+1)% USARTx_RINGBUF_SIZE;   // 读取索引加1
    return 1;
  }else return 0;
}

// shell控制台获取输入数据
void shell_app_cycle()
{
#define SHELL_DATA_PROCESS          {shell_input(&shell_1, temp, data_len);\
                                      Read_Index = (Read_Index+data_len)% USARTx_RINGBUF_SIZE;}
  if(Write_Index!=Read_Index){
    /* 取环形缓存区剩余数据 */
    char temp[USARTx_RINGBUF_SIZE]={0};
    char flag=0x1B;             // 方向键的特殊字符
    uint16_t data_len=UART_GetRemainDate(temp,shell_ringbuf,USARTx_RINGBUF_SIZE,Write_Index,Read_Index);

    /* 数据处理 */
    if(strchr(temp,flag)){
      if(strlen(temp)>2)SHELL_DATA_PROCESS;
    }
    else SHELL_DATA_PROCESS
  }
}

/**
 * @brief       Rx传输回调函数
 * @param       huart: UART句柄类型指针
 * @retval      无
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)                             /* 如果是串口1 */
  {
    shell_ringbuf[Write_Index] = (char)g_rx_buffer[0] ;
    Write_Index++;
    Write_Index = Write_Index % USARTx_RINGBUF_SIZE;
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
  }
}

// 使用寄存器版本的串口接收程序
void USART_UX_USER_INT(void)
{
  uint8_t rxdata;
  if (USART_UX->ISR & (1 << 5))               /* 接收到数据 */
  {
    rxdata = USART_UX->RDR;
    shell_ringbuf[Write_Index] = (char)rxdata ;
    Write_Index++;
    Write_Index = Write_Index % USARTx_RINGBUF_SIZE;
  }
  USART_UX->ICR |= 1 << 3;                    /* 清除溢出错误, 否则可能会卡死在串口中断服务函数里面 */
}

/**
 * @brief       串口1中断服务函数
 * @param       无
 * @retval      无
 */
void USART_UX_IRQHandler(void)
{ 
#if USE_UX_REGISTER
  HAL_UART_IRQHandler(&g_uart1_handle);     /* 调用HAL库中断处理公用函数 */
#else
  USART_UX_USER_INT();
#endif
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
  usart1_put(ch);
  return ch;
}
