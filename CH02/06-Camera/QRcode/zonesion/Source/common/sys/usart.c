#include "usart.h"
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

/**************************user config*************************/
// hal�⴮��ʹ��
/* ���ջ���, ���USART_REC_LEN���ֽ�. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  ����״̬
 *  bit15��      ������ɱ�־
 *  bit14��      ���յ�0x0d
 *  bit13~0��    ���յ�����Ч�ֽ���Ŀ
*/
uint16_t g_usart_rx_sta = 0;
volatile uint8_t g_rx_buffer[RXBUFFERSIZE]; /* HAL��ʹ�õĴ��ڽ��ջ��� */
UART_HandleTypeDef g_uart1_handle;          /* UART��� */

/*********************************************************************************************
* ����:usart_init
* ����:���ڳ�ʼ������
* ����:baudrate: ������, �����Լ���Ҫ���ò�����ֵ
* ����:��
* �޸�:��
* ע��:������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ
*        �������Ƕ�SPI1�ĳ�ʼ��
*********************************************************************************************/
void usart_init(uint32_t baudrate)
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

/*********************************************************************************************
* ����:HAL_UART_MspInit
* ����:UART�ײ��ʼ������
* ����:huart: UART�������ָ��
* ����:��
* �޸�:��
* ע��:�˺����ᱻHAL_UART_Init()����
 *       ���ʱ��ʹ�ܣ��������ã��ж�����
*********************************************************************************************/
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

/*********************************************************************************************
* ����:usart1_put
* ����:����1�ķ��ͺ���
* ����:��
* ����:��
* �޸�:��
* ע��:�����ַ�
*********************************************************************************************/
void usart1_put(const char data)
{
  while ((USART_UX->ISR & 0X40) == 0);    /* �ȴ���һ���ַ�������� */
  USART_UX->TDR = (uint8_t)data;          /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
}

/*********************************************************************************************
* ����:usart1_puts
* ����:����1�ķ��ͺ���
* ����:��
* ����:��
* �޸�:��
* ע��:�����ַ���
*********************************************************************************************/
void usart1_puts(const char * strbuf, unsigned short len)
{
  while(len--)
  {
    while ((USART_UX->ISR & 0X40) == 0);    /* �ȴ���һ���ַ�������� */
    USART_UX->TDR = (uint8_t)*strbuf;       /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    strbuf++ ;
  }
}

/*********************************************************************************************
* ����:usart1_puts
* ����:Rx����ص�����
* ����:huart: UART�������ָ��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)                             /* ����Ǵ���1 */
    {
        if ((g_usart_rx_sta & 0x8000) == 0)                    /* ����δ��� */
        {
            if (g_usart_rx_sta & 0x4000)                       /* ���յ���0x0d */
            {
                if (g_rx_buffer[0] != 0x0a) 
                {
                    g_usart_rx_sta = 0;                       /* ���մ���,���¿�ʼ */
                }
                else 
                {
                    g_usart_rx_sta |= 0x8000;                 /* ��������� */
                }
            }
            else                                              /* ��û�յ�0X0D */
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
                        g_usart_rx_sta = 0;                   /* �������ݴ���,���¿�ʼ���� */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
    }
}

/*********************************************************************************************
* ����:USART_UX_IRQHandler
* ����:����1�жϷ�����
* ����:huart: UART�������ָ��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void USART_UX_IRQHandler(void)
{ 
  HAL_UART_IRQHandler(&g_uart1_handle);     /* ����HAL���жϴ����ú��� */
}

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
  usart1_put(ch);
  return ch;
}
