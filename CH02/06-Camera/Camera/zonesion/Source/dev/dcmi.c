#include "extend.h"
#include "dcmi.h" 
#include "ili9806.h"
#include "led.h" 
#include "ov2640.h" 
#include "stdio.h"

volatile u8 ov_frame=0;                            //֡��
volatile u8 ov_frame_cnt=0;
extern u8 contrast_buf[30];
extern u8 saturation_buf[30];
extern u8 effect_buf[30];
extern u8 resolution_buf[30];
extern u8 key_active;
extern void DCMI_isr_app(void);

DCMI_HandleTypeDef g_dcmi_handle;       /* DCMI��� */
DMA_HandleTypeDef g_dma_dcmi_handle;    /* DMA��� */
#define USE_HAL_IRQHANDLER    1         // ʹ��hal����жϴ�����

/*********************************************************************************************
* ���ƣ�DCMI_IRQHandler
* ���ܣ�DCMI�жϷ�����
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
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
* ���ƣ�HAL_DCMI_FrameEventCallback
* ���ܣ�DCMI�жϻص�������
* ������hdcmi:DCMI���
* ���أ���
* �޸ģ���
* ע�ͣ�����һ֡ͼ����
*********************************************************************************************/
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  __HAL_DCMI_CLEAR_FLAG(&g_dcmi_handle, DCMI_FLAG_FRAMERI);    /* ���֡�ж� */
  DCMI_Stop(1);
  /* �жϵ�LCDӦ�ó���,ʱ�価���ܶ� */
  DCMI_isr_app();
  
  /* �ָ��ɼ� */
  ov_frame_cnt++;
  DCMI_Start();
}

/*********************************************************************************************
* ���ƣ�DCMI_DMA_Init
* ���ܣ�DCMI DMA����
* ������DMA_Memory0BaseAddr:�洢����ַ    ��Ҫ�洢����ͷ���ݵ��ڴ��ַ(Ҳ�����������ַ)
*           DMA_BufferSize:�洢������    0~65535
*           DMA_MemoryDataSize:�洢��λ��  
*           DMA_MemoryInc:�洢��������ʽ  
* ���أ���
* �޸ģ���
* ע�ͣ�@defgroup DMA_memory_data_size :DMA_MemoryDataSize_Byte/DMA_MemoryDataSize_HalfWord/DMA_MemoryDataSize_Word
*           @defgroup DMA_memory_incremented_mode  /** @defgroup DMA_memory_incremented_mode : DMA_MemoryInc_Enable/DMA_MemoryInc_Disable
*********************************************************************************************/
void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize,u32 DMA_MemoryInc)
{ 
  __HAL_RCC_DMA1_CLK_ENABLE();                                        /* ʹ��DMA1ʱ�� */
  __HAL_LINKDMA(&g_dcmi_handle, DMA_Handle, g_dma_dcmi_handle);       /* ��DMA��DCMI��ϵ���� */
  __HAL_DMA_DISABLE_IT(&g_dma_dcmi_handle, DMA_IT_TC);                /* �ȹر�DMA��������ж�(������ʹ��MCU����ʱ�����ֻ��������) */

  g_dma_dcmi_handle.Instance = DMA1_Stream1;                          /* DMA1������1 */
  g_dma_dcmi_handle.Init.Request = DMA_REQUEST_DCMI;                  /* DCMI��DMA���� */
  g_dma_dcmi_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;            /* ���赽�洢�� */
  g_dma_dcmi_handle.Init.PeriphInc = DMA_PINC_DISABLE;                /* ���������ģʽ */
  g_dma_dcmi_handle.Init.MemInc = DMA_MemoryInc;                      /* �洢������ģʽ */
  g_dma_dcmi_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;   /* �������ݳ���:32λ */
  g_dma_dcmi_handle.Init.MemDataAlignment = DMA_MemoryDataSize;       /* �洢�����ݳ���:8/16/32λ */
  g_dma_dcmi_handle.Init.Mode = DMA_CIRCULAR;                         /* ʹ��ѭ��ģʽ */
  g_dma_dcmi_handle.Init.Priority = DMA_PRIORITY_HIGH;                /* �����ȼ� */
  g_dma_dcmi_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;              /* ʹ��FIFO */
  g_dma_dcmi_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;     /* ʹ��ȫFIFO */
  g_dma_dcmi_handle.Init.MemBurst = DMA_MBURST_INC8;                  /* �洢��ͻ��8���Ĵ��� */
  g_dma_dcmi_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;             /* ����ͻ�����δ��� */
  HAL_DMA_DeInit(&g_dma_dcmi_handle);                                 /* �������ǰ������ */
  HAL_DMA_Init(&g_dma_dcmi_handle);                                   /* ��ʼ��DMA */

  /* �ڿ���DMA֮ǰ��ʹ��__HAL_UNLOCK()����һ��DMA,��ΪHAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
   * ����������һ��ʼҪ��ʹ��__HAL_LOCK()����DMA,������__HAL_LOCK()���жϵ�ǰ��DMA״̬�Ƿ�Ϊ����״̬�������
   * ����״̬�Ļ���ֱ�ӷ���HAL_BUSY�������ᵼ�º���HAL_DMA_Statrt()��HAL_DMAEx_MultiBufferStart()������DMA����
   * ����ֱ�ӱ�������DMAҲ�Ͳ�������������Ϊ�˱���������������������DMA֮ǰ�ȵ���__HAL_UNLOCK()�Ƚ���һ��DMA��
   */
  __HAL_UNLOCK(&g_dma_dcmi_handle);
  
  HAL_DMA_Start(&g_dma_dcmi_handle, (uint32_t)&DCMI->DR, DMA_Memory0BaseAddr, DMA_BufferSize);
} 

/*********************************************************************************************
* ���ƣ�HAL_DCMI_MspInit
* ���ܣ�DCMI�ײ��������������ã�ʱ��ʹ�ܣ��ж�����
* ������hdcmi:DCMI���
* ���أ���
* �޸ģ���
* ע�ͣ��˺����ᱻHAL_DCMI_Init()����
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
* ���ƣ�DCMI_init
* ���ܣ�DCMI ��ʼ��
* ������hdcmi:DCMI���
* ���أ���
* �޸ģ���
* ע�ͣ�IO��Ӧ��ϵ����:
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
  HAL_NVIC_SetPriority(DCMI_IRQn, 0, 0);                      /* ��ռ���ȼ�0�������ȼ�0 */
  HAL_NVIC_EnableIRQ(DCMI_IRQn);                              /* ʹ��DCMI�ж� */
}

/*********************************************************************************************
* ���ƣ�DCMI_Start
* ���ܣ�DCMI,��������
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
void DCMI_Start(void)
{
  /* ����LCD�Զ�������� */ 
  LCD_Set_Window((lcd_dev.height-IMAGE_WIN_HEIGHT)/2,
                 (lcd_dev.height-IMAGE_WIN_HEIGHT)/2,
                 IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT);
  LCD_WriteRAM_Prepare();       //��ʼд��GRAM
  
  /* �����־λ����������DCMI�������� */
//  DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | 
//                DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);  //��ֹDMAʹ�ܲ���
  __HAL_DMA_ENABLE(&g_dma_dcmi_handle);   /* ʹ��DMA */
  DCMI->CR |= DCMI_CR_CAPTURE;            /* DCMI����ʹ�� */  
  /* ����ʹ��֡�ж�,��ΪHAL_DCMI_IRQHandler()������ر�֡�ж� */
  __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);    //����֡�ж� 
}

//DCMI,�رմ���
void DCMI_Stop(u8 isr)
{
  __HAL_DCMI_DISABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);   //ʧ��֡�ж� 
  DCMI->CR &= ~(DCMI_CR_CAPTURE);   /* DCMI����ر� */
  if(!isr)
    while (DCMI->CR & 0X01);       //��DCMI��֡�ж��в���Ҫ�ȴ�
  __HAL_DMA_DISABLE(&g_dma_dcmi_handle);  /* �ر�DMA */
}
