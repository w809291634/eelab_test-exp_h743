#include "extend.h"
#include "dcmi.h" 
#include "ili9806.h"
#include "led.h" 
#include "ov2640.h" 
#include "stdio.h"
#include "qr_decoder_app.h"

volatile u8 ov_frame=0;                                         //֡��
volatile u8 ov_frame_cnt=0;
extern void DCMI_isr_app(void);

#if defined(__ICCARM__)
#pragma location = "g_dcmi_multi_buf"
#pragma data_alignment=4
#elif defined(__CC_ARM)
__attribute__((section(".g_dcmi_multi_buf"), zero_init))
__attribute__ ((aligned (4)))
#elif defined(__GNUC__)
__attribute__((section(".g_dcmi_multi_buf")))
__attribute__ ((aligned (4)))
#else
#error Unknown compiler
#endif

uint32_t g_dcmi_multi_buf[2][Dma_MultiBuffer_size];  /* ����DMA˫����buf���ܹ����ճ������� */

volatile uint32_t dcmi_data_len = 0;                            /* dcmi ��ǰ���ݽ��ճ��� */

DCMI_HandleTypeDef g_dcmi_handle;                               /* DCMI��� */
DMA_HandleTypeDef g_dma_dcmi_handle;                            /* DMA��� */

/*********************************************************************************************
* ����:DCMI_IRQHandler
* ����:DCMI�жϷ�����
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void DCMI_IRQHandler(void)
{
#if USE_HAL_IRQHANDLER
  HAL_DCMI_IRQHandler(&g_dcmi_handle);
#else
  uint32_t isr_value = READ_REG(g_dcmi_handle.Instance->MISR);
  /* Synchronization error interrupt management *******************************/
  if ((isr_value & DCMI_FLAG_FRAMERI) == DCMI_FLAG_FRAMERI)
  {
    __HAL_DCMI_CLEAR_FLAG(&g_dcmi_handle, DCMI_FLAG_FRAMERI);    /* ���֡�ж� */
    DCMI_Stop(1);
    /* �жϵ�LCDӦ�ó���,ʱ�価���ܶ� */
    DCMI_isr_app();
    
    /* �ָ��ɼ� */
    ov_frame_cnt++;
    DCMI_Start();
  }
#endif
}

/*********************************************************************************************
* ����:HAL_DCMI_FrameEventCallback
* ����:DCMI�жϻص�������
* ����:hdcmi:DCMI���
* ����:��
* �޸�:��
* ע��:����һ֡ͼ����
*********************************************************************************************/
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  __HAL_DCMI_CLEAR_FLAG(&g_dcmi_handle, DCMI_FLAG_FRAMERI);    /* ���֡�ж� */
  DCMI_Stop(1);
  
  /* ����LCD�Զ�������� */ 
#if USE_QR_IMG==1
  LCD_Set_Window((lcd_dev.width-QR_IMG_W)/2,
                 (lcd_dev.height-QR_IMG_H)/2-20,
                 QR_IMG_W,QR_IMG_H);
  qr_img_intercept();           //ͼ���ȡ
#else
  LCD_Set_Window((lcd_dev.width-IMAGE_WIN_WIDTH)/2,
                 (lcd_dev.height-IMAGE_WIN_HEIGHT)/2-20,
                 IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT);
#endif
  LCD_WriteRAM_Prepare();       //��ʼд��GRAM
  lcd_refresh();                //�������ͼ��ˢ�µ�LCD��
  
  /* �жϵ�LCDӦ�ó���,ʱ�価���ܶ� */
  DCMI_isr_app();
  
  /* �ָ��ɼ� */
  ov_frame_cnt++;
  DCMI_Start();
}

/*********************************************************************************************
* ����:void dcmi_rx_callback
* ����:DMA˫�������ݽ��ջص�����
* ����:hdcmi:DCMI���
* ����:��
* �޸�:��
* ע��:��DMA1_Stream1_IRQHandler�жϷ��������汻����
*        DCMI DMA���ջص�����, ��˫����ģʽ�õ�, ����жϷ�����ʹ��
*********************************************************************************************/
static void dcmi_rx_callback(void)
{
  uint16_t i;
  uint32_t *pbuf;
  
  if(dcmi_data_len>=IMAGE_WIN_WIDTH*IMAGE_WIN_HEIGHT/2) 
    return;                                               // ��ֹbuf�����ָ��Խ��
  pbuf = (uint32_t *)dcmi_buf + dcmi_data_len;            /* ƫ�Ƶ���Ч����ĩβ */
 
  if (DMA1_Stream1->CR & (1 << 19))                       /* buf0����,��������buf1 */
  {
      for (i = 0; i < Dma_MultiBuffer_size; i++)
          pbuf[i] = g_dcmi_multi_buf[0][i];               /* ��ȡbuf0��������� */

      dcmi_data_len += Dma_MultiBuffer_size;              /* ƫ�� */
  }
  else                                                    /* buf1����,��������buf0 */
  {
      for (i = 0; i < Dma_MultiBuffer_size; i++)
          pbuf[i] = g_dcmi_multi_buf[1][i];               /* ��ȡbuf1��������� */

      dcmi_data_len += Dma_MultiBuffer_size;              /* ƫ�� */
  }
  SCB_CleanInvalidateDCache();                            /* �����Ч��DCache */
}

/*********************************************************************************************
* ����:DMA1_Stream1_IRQHandler
* ����:DMA1_Stream1�жϷ�����(��˫����ģʽ���õ�)
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void DMA1_Stream1_IRQHandler(void)
{
  uint32_t isr_value = READ_REG(g_dcmi_handle.Instance->MISR);
  if (__HAL_DMA_GET_FLAG(&g_dma_dcmi_handle, DMA_FLAG_TCIF1_5) != RESET)  /* DMA������� */
  {
    __HAL_DMA_CLEAR_FLAG(&g_dma_dcmi_handle, DMA_FLAG_TCIF1_5);         /* ���DMA��������жϱ�־λ */
    dcmi_rx_callback();                                                 /* ִ������ͷ���ջص�����,��ȡ���ݵȲ����������洦�� */
  } 
}

/*********************************************************************************************
* ����:DCMI_DMA_Init
* ����:DCMI DMA����
* ����:mem0addr:�洢��������1��ַ
*        mem1addr:�洢��������2��ַ�����ʹ��DMA����������Ͳ���Ҫ�õ�
*        memsize:���ô��䳤�ȣ���λ���ݳ��ȣ���ʵ������仯��    0~65535
*        memblen:�洢��λ��  
*        meminc:�洢���Ƿ��Զ�����
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void DCMI_DMA_Init(uint32_t mem0addr, uint32_t mem1addr, uint16_t memsize, uint32_t memblen, uint32_t meminc)
{ 
  __HAL_RCC_DMA1_CLK_ENABLE();                                        /* ʹ��DMA1ʱ�� */
  __HAL_LINKDMA(&g_dcmi_handle, DMA_Handle, g_dma_dcmi_handle);       /* ��DMA��DCMI��ϵ���� */
  __HAL_DMA_DISABLE_IT(&g_dma_dcmi_handle, DMA_IT_TC);                /* �ȹر�DMA��������ж�(������ʹ��MCU����ʱ�����ֻ��������) */

  g_dma_dcmi_handle.Instance = DMA1_Stream1;                          /* DMA1������1 */
  g_dma_dcmi_handle.Init.Request = DMA_REQUEST_DCMI;                  /* DCMI��DMA���� */
  g_dma_dcmi_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;            /* ���赽�洢�� */
  g_dma_dcmi_handle.Init.PeriphInc = DMA_PINC_DISABLE;                /* ���������ģʽ */
  g_dma_dcmi_handle.Init.MemInc = meminc;                             /* �洢������ģʽ */
  g_dma_dcmi_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;   /* �������ݳ���:32λ */
  g_dma_dcmi_handle.Init.MemDataAlignment = memblen;                  /* �洢�����ݳ���:8/16/32λ */
  g_dma_dcmi_handle.Init.Mode = DMA_CIRCULAR;                         /* ʹ��ѭ��ģʽ */
  g_dma_dcmi_handle.Init.Priority = DMA_PRIORITY_HIGH;                /* �����ȼ� */
  g_dma_dcmi_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;              /* ʹ��FIFO */
  g_dma_dcmi_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL; /* ʹ��1/2��FIFO */
  g_dma_dcmi_handle.Init.MemBurst = DMA_PBURST_SINGLE;                /* �洢��ͻ������ */
  g_dma_dcmi_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;             /* ����ͻ�����δ��� */
  HAL_DMA_DeInit(&g_dma_dcmi_handle);                                 /* �������ǰ������ */
  HAL_DMA_Init(&g_dma_dcmi_handle);                                   /* ��ʼ��DMA */

  /* �ڿ���DMA֮ǰ��ʹ��__HAL_UNLOCK()����һ��DMA,��ΪHAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
   * ����������һ��ʼҪ��ʹ��__HAL_LOCK()����DMA,������__HAL_LOCK()���жϵ�ǰ��DMA״̬�Ƿ�Ϊ����״̬�������
   * ����״̬�Ļ���ֱ�ӷ���HAL_BUSY�������ᵼ�º���HAL_DMA_Statrt()��HAL_DMAEx_MultiBufferStart()������DMA����
   * ����ֱ�ӱ�������DMAҲ�Ͳ�������������Ϊ�˱���������������������DMA֮ǰ�ȵ���__HAL_UNLOCK()�Ƚ���һ��DMA��
   */
  __HAL_UNLOCK(&g_dma_dcmi_handle);
  
  if (mem1addr == 0)                                                                                  /* ����DMA����ʹ��˫���� */
  {
    HAL_DMA_Start(&g_dma_dcmi_handle, (uint32_t)&DCMI->DR, mem0addr, memsize);
  }
  else                                                                                                /* ʹ��˫���� */
  {
    HAL_DMAEx_MultiBufferStart(&g_dma_dcmi_handle, (uint32_t)&DCMI->DR, mem0addr, mem1addr, memsize); /* ����˫���� */
    __HAL_DMA_ENABLE_IT(&g_dma_dcmi_handle, DMA_IT_TC);                                               /* ������������ж� */
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, DMA_IRQ_PRO, 0);                                          /* DMA�ж����ȼ� */
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  }
} 

/*********************************************************************************************
* ����:HAL_DCMI_MspInit
* ����:DCMI�ײ��������������ã�ʱ��ʹ�ܣ��ж�����
* ����:��
* ����:��
* �޸�:��
* ע��:�˺����ᱻHAL_DCMI_Init()����
*********************************************************************************************/
void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{
  GPIO_InitTypeDef gpio_init_struct;
 
  __HAL_RCC_DCMI_CLK_ENABLE();                        /* ʹ��DCMIʱ�� */
  __HAL_RCC_GPIOA_CLK_ENABLE();                       /* ʹ��GPIOAʱ�� */
  __HAL_RCC_GPIOB_CLK_ENABLE();                       /* ʹ��GPIOBʱ�� */
  __HAL_RCC_GPIOC_CLK_ENABLE();                       /* ʹ��GPIOCʱ�� */
  __HAL_RCC_GPIOE_CLK_ENABLE();                       /* ʹ��GPIOEʱ�� */
  /* PA4/6��ʼ������ PA4--DCMI_HSYNC  PA6--DCMI_PCLK */
  gpio_init_struct.Pin = GPIO_PIN_4|GPIO_PIN_6;
  gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* ���츴�� */
  gpio_init_struct.Pull = GPIO_PULLUP;                /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /* ���� */
  gpio_init_struct.Alternate = GPIO_AF13_DCMI;        /* ����ΪDCMI */
  HAL_GPIO_Init(GPIOA, &gpio_init_struct);            /* ��ʼ��PA6���� */

  /* PB6/7��ʼ������ PB6--DCMI_D5  PB7--DCMI_VSYNC */
  gpio_init_struct.Pin = GPIO_PIN_7|GPIO_PIN_6;
  HAL_GPIO_Init(GPIOB, &gpio_init_struct);            /* ��ʼ������ */

  /* PC6/7 ��ʼ������ DCMI_D0-D1 */
  gpio_init_struct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  HAL_GPIO_Init(GPIOC, &gpio_init_struct);            /* ��ʼ������ */

  /* PE0/1/4/5/6 ��ʼ������ DCMI_D2/3/4/6/7 */
  gpio_init_struct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6; 
  HAL_GPIO_Init(GPIOE, &gpio_init_struct);            /* ��ʼ������ */
}

/*********************************************************************************************
* ����:DCMI_init
* ����:DCMI ��ʼ��
* ����:��
* ����:��
* �޸�:��
* ע��:IO��Ӧ��ϵ����:
*              ����ͷģ�� ------------  STM32������
*               OV_D0~D7  ------------  PC6/PC7/PE0/PE1/PE4/PB6/PE5/PE6
*               OV_SCL    ------------  PG4
*               OV_SDA    ------------  PG3
*               OV_VSYNC  ------------  PB7
*               OV_HREF   ------------  PA4
*               OV_PCLK   ------------  PA6
*               OV_RESET  ------------  PA11
*              ����������ʼ��OV_D0~D7/OV_VSYNC/OV_HREF/OV_PCLK���ź�(11��).
*********************************************************************************************/
void DCMI_init(void)
{
  g_dcmi_handle.Instance = DCMI;
  g_dcmi_handle.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;     /* Ӳ��ͬ��HSYNC,VSYNC */
  g_dcmi_handle.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;   /* PCLK  ��������Ч */
  g_dcmi_handle.Init.VSPolarity = DCMI_VSPOLARITY_LOW;        /* VSYNC �͵�ƽ��Ч */
  g_dcmi_handle.Init.HSPolarity = DCMI_HSPOLARITY_LOW;        /* HSYNC �͵�ƽ��Ч */
  g_dcmi_handle.Init.CaptureRate = DCMI_CR_ALL_FRAME;         /* ȫ֡���� */
  g_dcmi_handle.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;  /* 8λ���ݸ�ʽ */
  HAL_DCMI_Init(&g_dcmi_handle);                              /* ��ʼ��DCMI���˺����Ὺ��֡�ж� */ 

  __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);        /* ������ͷ��׽�ж� */
  __HAL_DCMI_ENABLE(&g_dcmi_handle);

  /* �ر����жϡ�VSYNC�жϡ�ͬ�������жϺ�����ж� */
  __HAL_DCMI_DISABLE_IT(&g_dcmi_handle, DCMI_IT_LINE | DCMI_IT_VSYNC | DCMI_IT_ERR | DCMI_IT_OVR);
  HAL_NVIC_SetPriority(DCMI_IRQn, DCMI_IRQ_PRO, 0);           /* ��ռ���ȼ�1�������ȼ�0 */
  HAL_NVIC_EnableIRQ(DCMI_IRQn);                              /* ʹ��DCMI�ж� */
}

/*********************************************************************************************
* ����:lcd_refresh
* ����:������ͷ���ݻ�����µ�LCD��
* ����:��
* ����:��
* �޸�:��
* ע��:IO��Ӧ��ϵ����:
*********************************************************************************************/
void lcd_refresh(void)
{
  u16* p=(u16*)dcmi_buf;
#if USE_QR_IMG==1
  for(int i=0;i<QR_IMG_W*QR_IMG_H;i++){
    *(__IO u16 *) (Bank1_LCD_D)=p[i];
  }
#else
  for(int i=0;i<ARRAY_SIZE(dcmi_buf)*2;i++){
    *(__IO u16 *) (Bank1_LCD_D)=p[i];
  }
#endif
}

/*********************************************************************************************
* ����:DCMI_Start
* ����:DCMI,��������
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void DCMI_Start(void)
{ 
  /* �����־λ����������DCMI�������� */
  __HAL_DMA_CLEAR_FLAG(&g_dma_dcmi_handle,DMA_FLAG_FEIF1_5 | DMA_FLAG_DMEIF1_5 | 
                DMA_FLAG_TEIF1_5 | DMA_FLAG_HTIF1_5 | DMA_FLAG_TCIF1_5);    //��ֹDMAʹ�ܲ���
  __HAL_UNLOCK(&g_dma_dcmi_handle);
  __HAL_DMA_ENABLE(&g_dma_dcmi_handle);   /* ʹ��DMA */
  
  DCMI->CR |= DCMI_CR_CAPTURE;            /* DCMI����ʹ�� */  
  /* ����ʹ��֡�ж�,��ΪHAL_DCMI_IRQHandler()������ر�֡�ж� */
  __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);    //����֡�ж� 
}

/*********************************************************************************************
* ����:DCMI_Stop
* ����:DCMI,�رմ���
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void DCMI_Stop(u8 isr)
{
  __HAL_DCMI_DISABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);   //ʧ��֡�ж� 
  DCMI->CR &= ~(DCMI_CR_CAPTURE);                 /* DCMI����ر� */
  if(!isr)
    while (DCMI->CR & 0X01);                      //��DCMI��֡�ж��в���Ҫ�ȴ�
  __HAL_DMA_DISABLE(&g_dma_dcmi_handle);          /* �ر�DMA */
}
