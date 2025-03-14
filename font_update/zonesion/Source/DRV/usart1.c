#include "usart1.h"
#include "stdio.h"

/**************************user config*************************/
// �������̨���Դ��ڵ�Ӳ��
// ����1 ��������
#define USART_TX_GPIO_PORT              GPIOA
#define USART_TX_GPIO_PIN               GPIO_PIN_9
#define USART_TX_GPIO_AF                GPIO_AF7_USART1
#define USART_TX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* ��������ʱ��ʹ�� */

#define USART_RX_GPIO_PORT              GPIOA
#define USART_RX_GPIO_PIN               GPIO_PIN_10
#define USART_RX_GPIO_AF                GPIO_AF7_USART1
#define USART_RX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* ��������ʱ��ʹ�� */

#define USART_UX                        USART1
#define USART_UX_IRQn                   USART1_IRQn
#define USART_UX_IRQHandler             USART1_IRQHandler
#define USART_UX_CLK_ENABLE()           do{ __HAL_RCC_USART1_CLK_ENABLE(); }while(0)  /* USART1 ʱ��ʹ�� */

#define USARTx_RINGBUF_SIZE             (256)     
#define RXBUFFERSIZE                    1      /* �����С */
#define USART_EN_RX                     1      /* ʹ�ܣ�1��/��ֹ��0������1���� */
#define USE_UX_REGISTER                 0      // �Ƿ�ʹ�üĴ��������д������ݻ�ȡ
/**************************user config*************************/

// shell �ն�ʹ�ñ���
shellinput_t shell_1;
static char shell_ringbuf[USARTx_RINGBUF_SIZE]={0};
static volatile unsigned short Read_Index,Write_Index;

char usart1_mode=0;                         // ���ڵ������� ����ָ��  0�����ݵ���shell����̨  1�����ݵ���IAP����

// hal�⴮��ʹ��
volatile uint8_t g_rx_buffer[RXBUFFERSIZE]; /* HAL��ʹ�õĴ��ڽ��ջ��� */
UART_HandleTypeDef g_uart1_handle;          /* UART��� */

/**
 * @brief       ����X��ʼ������
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @note        ע��: ����������ȷ��ʱ��Դ, ���򴮿ڲ����ʾͻ������쳣.
 *              �����USART��ʱ��ԴSystemClock_Config()�������Ѿ����ù���.
 * @retval      ��
 */
void shell_hw_init(uint32_t baudrate)
{
    g_uart1_handle.Instance = USART_UX;                    /* USART1 */
    g_uart1_handle.Init.BaudRate = baudrate;               /* ������ */
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B;   /* �ֳ�Ϊ8λ���ݸ�ʽ */
    g_uart1_handle.Init.StopBits = UART_STOPBITS_1;        /* һ��ֹͣλ */
    g_uart1_handle.Init.Parity = UART_PARITY_NONE;         /* ����żУ��λ */
    g_uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;   /* ��Ӳ������ */
    g_uart1_handle.Init.Mode = UART_MODE_TX_RX;            /* �շ�ģʽ */
    HAL_UART_Init(&g_uart1_handle);                        /* HAL_UART_Init()��ʹ��UART1 */
    
    /* �ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ��������������� */
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
}

/**
 * @brief       UART�ײ��ʼ������
 * @param       huart: UART�������ָ��
 * @note        �˺����ᱻHAL_UART_Init()����
 *              ���ʱ��ʹ�ܣ��������ã��ж�����
 * @retval      ��
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;
    if (huart->Instance == USART_UX)                                /* ����Ǵ���1�����д���1 MSP��ʼ�� */
    {
        USART_UX_CLK_ENABLE();                                      /* USART1 ʱ��ʹ�� */
        USART_TX_GPIO_CLK_ENABLE();                                 /* ��������ʱ��ʹ�� */
        USART_RX_GPIO_CLK_ENABLE();                                 /* ��������ʱ��ʹ�� */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;                   /* TX���� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                    /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
        gpio_init_struct.Alternate = USART_TX_GPIO_AF;              /* ����ΪUSART1 */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);       /* ��ʼ���������� */

        gpio_init_struct.Pin = USART_RX_GPIO_PIN;                   /* RX���� */
        gpio_init_struct.Alternate = USART_RX_GPIO_AF;              /* ����ΪUSART1 */
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);       /* ��ʼ���������� */

#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);                          /* ʹ��USART1�ж�ͨ�� */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 1, 0);                  /* ��ռ���ȼ�1�������ȼ�0 */
#endif
    }
}

// �������
void usart1_flush(void)
{
  Read_Index=Write_Index;
}

// ����1�ķ��ͺ���
// �����ַ�
void usart1_put(const char data)
{
  while ((USART_UX->ISR & 0X40) == 0);    /* �ȴ���һ���ַ�������� */
  USART_UX->TDR = (uint8_t)data;          /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
}

// ����1�ķ��ͺ���
// �����ַ���
void usart1_puts(const char * strbuf, unsigned short len)
{
  while(len--)
  {
    while ((USART_UX->ISR & 0X40) == 0);    /* �ȴ���һ���ַ�������� */
    USART_UX->TDR = (uint8_t)*strbuf;       /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    strbuf++ ;
  }
}

/* ��ȡ��������ʣ��δ��ȡ���� */
// data ��ȡʣ������
// ringbuf ��ǰ���λ����� size ��������С 
// w_index r_index ��ǰ�������Ķ�д����
// ���� ��ȡ����
uint16_t UART_GetRemainDate(char* data, char* ringbuf, uint16_t size, uint16_t w_index, uint16_t r_index) 
{
  //��ȡʣ������
  if(w_index >= r_index) { 
    int len1 = w_index - r_index;
    memcpy(data,ringbuf + r_index,len1);
    return len1;
  } else {
    int len1=size-r_index;          // ����ĩβ������
    int len2=w_index;
    memcpy(data,ringbuf + r_index,len1);
    memcpy(data+len1,ringbuf,len2);
    return len1+len2;
  }
}

// �ӻ�������ȡһ���ֽ�
// ע�⣺���ݴ�������shell_app_cycleֻ��ѡ����һ
int usart1_getchar(uint8_t* data)
{
  if(Write_Index!=Read_Index){
    *data=shell_ringbuf[Read_Index];
    Read_Index = (Read_Index+1)% USARTx_RINGBUF_SIZE;   // ��ȡ������1
    return 1;
  }else return 0;
}

// shell����̨��ȡ��������
void shell_app_cycle()
{
#define SHELL_DATA_PROCESS          {shell_input(&shell_1, temp, data_len);\
                                      Read_Index = (Read_Index+data_len)% USARTx_RINGBUF_SIZE;}
  if(Write_Index!=Read_Index){
    /* ȡ���λ�����ʣ������ */
    char temp[USARTx_RINGBUF_SIZE]={0};
    char flag=0x1B;             // ������������ַ�
    uint16_t data_len=UART_GetRemainDate(temp,shell_ringbuf,USARTx_RINGBUF_SIZE,Write_Index,Read_Index);

    /* ���ݴ��� */
    if(strchr(temp,flag)){
      if(strlen(temp)>2)SHELL_DATA_PROCESS;
    }
    else SHELL_DATA_PROCESS
  }
}

/**
 * @brief       Rx����ص�����
 * @param       huart: UART�������ָ��
 * @retval      ��
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)                             /* ����Ǵ���1 */
  {
    shell_ringbuf[Write_Index] = (char)g_rx_buffer[0] ;
    Write_Index++;
    Write_Index = Write_Index % USARTx_RINGBUF_SIZE;
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
  }
}

// ʹ�üĴ����汾�Ĵ��ڽ��ճ���
void USART_UX_USER_INT(void)
{
  uint8_t rxdata;
  if (USART_UX->ISR & (1 << 5))               /* ���յ����� */
  {
    rxdata = USART_UX->RDR;
    shell_ringbuf[Write_Index] = (char)rxdata ;
    Write_Index++;
    Write_Index = Write_Index % USARTx_RINGBUF_SIZE;
  }
  USART_UX->ICR |= 1 << 3;                    /* ����������, ������ܻῨ���ڴ����жϷ��������� */
}

/**
 * @brief       ����1�жϷ�����
 * @param       ��
 * @retval      ��
 */
void USART_UX_IRQHandler(void)
{ 
#if USE_UX_REGISTER
  HAL_UART_IRQHandler(&g_uart1_handle);     /* ����HAL���жϴ����ú��� */
#else
  USART_UX_USER_INT();
#endif
}

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
  usart1_put(ch);
  return ch;
}
