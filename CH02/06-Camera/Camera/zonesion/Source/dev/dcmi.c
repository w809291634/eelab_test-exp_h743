#include "extend.h"
#include "dcmi.h" 
#include "ili9806.h"
#include "led.h" 
#include "ov2640.h" 
#include "stdio.h"

volatile u8 ov_frame=0;                            //帧率
volatile u8 ov_frame_cnt=0;
extern u8 contrast_buf[30];
extern u8 saturation_buf[30];
extern u8 effect_buf[30];
extern u8 resolution_buf[30];
extern u8 key_active;
extern void DCMI_isr_app(void);

DCMI_HandleTypeDef g_dcmi_handle;       /* DCMI句柄 */
DMA_HandleTypeDef g_dma_dcmi_handle;    /* DMA句柄 */
#define USE_HAL_IRQHANDLER    1         // 使用hal库的中断处理函数

/*********************************************************************************************
* 名称：DCMI_IRQHandler
* 功能：DCMI中断服务函数
* 参数：无
* 返回：无
* 修改：无
* 注释：无
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
    __HAL_DCMI_CLEAR_FLAG(&g_dcmi_handle, DCMI_FLAG_FRAMERI);    /* 清除帧中断 */
    DCMI_Stop(1);
    /* 中断的LCD应用程序,时间尽可能短 */
    DCMI_isr_app();
    
    /* 恢复采集 */
    ov_frame_cnt++;
    DCMI_Start();
  }
#endif
}

/*********************************************************************************************
* 名称：HAL_DCMI_FrameEventCallback
* 功能：DCMI中断回调服务函数
* 参数：hdcmi:DCMI句柄
* 返回：无
* 修改：无
* 注释：捕获到一帧图像处理
*********************************************************************************************/
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  __HAL_DCMI_CLEAR_FLAG(&g_dcmi_handle, DCMI_FLAG_FRAMERI);    /* 清除帧中断 */
  DCMI_Stop(1);
  /* 中断的LCD应用程序,时间尽可能短 */
  DCMI_isr_app();
  
  /* 恢复采集 */
  ov_frame_cnt++;
  DCMI_Start();
}

/*********************************************************************************************
* 名称：DCMI_DMA_Init
* 功能：DCMI DMA配置
* 参数：DMA_Memory0BaseAddr:存储器地址    将要存储摄像头数据的内存地址(也可以是外设地址)
*           DMA_BufferSize:存储器长度    0~65535
*           DMA_MemoryDataSize:存储器位宽  
*           DMA_MemoryInc:存储器增长方式  
* 返回：无
* 修改：无
* 注释：@defgroup DMA_memory_data_size :DMA_MemoryDataSize_Byte/DMA_MemoryDataSize_HalfWord/DMA_MemoryDataSize_Word
*           @defgroup DMA_memory_incremented_mode  /** @defgroup DMA_memory_incremented_mode : DMA_MemoryInc_Enable/DMA_MemoryInc_Disable
*********************************************************************************************/
void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize,u32 DMA_MemoryInc)
{ 
  __HAL_RCC_DMA1_CLK_ENABLE();                                        /* 使能DMA1时钟 */
  __HAL_LINKDMA(&g_dcmi_handle, DMA_Handle, g_dma_dcmi_handle);       /* 将DMA与DCMI联系起来 */
  __HAL_DMA_DISABLE_IT(&g_dma_dcmi_handle, DMA_IT_TC);                /* 先关闭DMA传输完成中断(否则在使用MCU屏的时候会出现花屏的情况) */

  g_dma_dcmi_handle.Instance = DMA1_Stream1;                          /* DMA1数据流1 */
  g_dma_dcmi_handle.Init.Request = DMA_REQUEST_DCMI;                  /* DCMI的DMA请求 */
  g_dma_dcmi_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;            /* 外设到存储器 */
  g_dma_dcmi_handle.Init.PeriphInc = DMA_PINC_DISABLE;                /* 外设非增量模式 */
  g_dma_dcmi_handle.Init.MemInc = DMA_MemoryInc;                      /* 存储器增量模式 */
  g_dma_dcmi_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;   /* 外设数据长度:32位 */
  g_dma_dcmi_handle.Init.MemDataAlignment = DMA_MemoryDataSize;       /* 存储器数据长度:8/16/32位 */
  g_dma_dcmi_handle.Init.Mode = DMA_CIRCULAR;                         /* 使用循环模式 */
  g_dma_dcmi_handle.Init.Priority = DMA_PRIORITY_HIGH;                /* 高优先级 */
  g_dma_dcmi_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;              /* 使能FIFO */
  g_dma_dcmi_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;     /* 使用全FIFO */
  g_dma_dcmi_handle.Init.MemBurst = DMA_MBURST_INC8;                  /* 存储器突发8节拍传输 */
  g_dma_dcmi_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;             /* 外设突发单次传输 */
  HAL_DMA_DeInit(&g_dma_dcmi_handle);                                 /* 先清除以前的设置 */
  HAL_DMA_Init(&g_dma_dcmi_handle);                                   /* 初始化DMA */

  /* 在开启DMA之前先使用__HAL_UNLOCK()解锁一次DMA,因为HAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
   * 这两个函数一开始要先使用__HAL_LOCK()锁定DMA,而函数__HAL_LOCK()会判断当前的DMA状态是否为锁定状态，如果是
   * 锁定状态的话就直接返回HAL_BUSY，这样会导致函数HAL_DMA_Statrt()和HAL_DMAEx_MultiBufferStart()后续的DMA配置
   * 程序直接被跳过！DMA也就不能正常工作，为了避免这种现象，所以在启动DMA之前先调用__HAL_UNLOCK()先解锁一次DMA。
   */
  __HAL_UNLOCK(&g_dma_dcmi_handle);
  
  HAL_DMA_Start(&g_dma_dcmi_handle, (uint32_t)&DCMI->DR, DMA_Memory0BaseAddr, DMA_BufferSize);
} 

/*********************************************************************************************
* 名称：HAL_DCMI_MspInit
* 功能：DCMI底层驱动，引脚配置，时钟使能，中断配置
* 参数：hdcmi:DCMI句柄
* 返回：无
* 修改：无
* 注释：此函数会被HAL_DCMI_Init()调用
*********************************************************************************************/
void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{
  GPIO_InitTypeDef gpio_init_struct;
 
  __HAL_RCC_DCMI_CLK_ENABLE();                        /* 使能DCMI时钟 */
  __HAL_RCC_GPIOA_CLK_ENABLE();                       /* 使能GPIOA时钟 */
  __HAL_RCC_GPIOB_CLK_ENABLE();                       /* 使能GPIOB时钟 */
  __HAL_RCC_GPIOC_CLK_ENABLE();                       /* 使能GPIOC时钟 */
  __HAL_RCC_GPIOE_CLK_ENABLE();                       /* 使能GPIOE时钟 */
  /* PA4/6初始化设置 PA4--DCMI_HSYNC  PA6--DCMI_PCLK */
  gpio_init_struct.Pin = GPIO_PIN_4|GPIO_PIN_6;
  gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* 推挽复用 */
  gpio_init_struct.Pull = GPIO_PULLUP;                /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /* 高速 */
  gpio_init_struct.Alternate = GPIO_AF13_DCMI;        /* 复用为DCMI */
  HAL_GPIO_Init(GPIOA, &gpio_init_struct);            /* 初始化PA6引脚 */

  /* PB6/7初始化设置 PB6--DCMI_D5  PB7--DCMI_VSYNC */
  gpio_init_struct.Pin = GPIO_PIN_7|GPIO_PIN_6;
  HAL_GPIO_Init(GPIOB, &gpio_init_struct);            /* 初始化引脚 */

  /* PC6/7 初始化设置 DCMI_D0-D1 */
  gpio_init_struct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  HAL_GPIO_Init(GPIOC, &gpio_init_struct);            /* 初始化引脚 */

  /* PE0/1/4/5/6 初始化设置 DCMI_D2/3/4/6/7 */
  gpio_init_struct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6; 
  HAL_GPIO_Init(GPIOE, &gpio_init_struct);            /* 初始化引脚 */
}

/*********************************************************************************************
* 名称：DCMI_init
* 功能：DCMI 初始化
* 参数：hdcmi:DCMI句柄
* 返回：无
* 修改：无
* 注释：IO对应关系如下:
*              摄像头模块 ------------  STM32开发板
*               OV_D0~D7  ------------  PC6/PC7/PE0/PE1/PE4/PB6/PE5/PE6
*               OV_SCL    ------------  PG4
*               OV_SDA    ------------  PG3
*               OV_VSYNC  ------------  PB7
*               OV_HREF   ------------  PA4
*               OV_PCLK   ------------  PA6
*               OV_RESET  ------------  PA11
*              本函数仅初始化OV_D0~D7/OV_VSYNC/OV_HREF/OV_PCLK等信号(11个).
*********************************************************************************************/
void DCMI_init(void)
{
  g_dcmi_handle.Instance = DCMI;
  g_dcmi_handle.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;     /* 硬件同步HSYNC,VSYNC */
  g_dcmi_handle.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;   /* PCLK  上升沿有效 */
  g_dcmi_handle.Init.VSPolarity = DCMI_VSPOLARITY_LOW;        /* VSYNC 低电平有效 */
  g_dcmi_handle.Init.HSPolarity = DCMI_HSPOLARITY_LOW;        /* HSYNC 低电平有效 */
  g_dcmi_handle.Init.CaptureRate = DCMI_CR_ALL_FRAME;         /* 全帧捕获 */
  g_dcmi_handle.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;  /* 8位数据格式 */
  HAL_DCMI_Init(&g_dcmi_handle);                              /* 初始化DCMI，此函数会开启帧中断 */ 

  __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);        /* 开启镜头捕捉中断 */
  __HAL_DCMI_ENABLE(&g_dcmi_handle);

  /* 关闭行中断、VSYNC中断、同步错误中断和溢出中断 */
  __HAL_DCMI_DISABLE_IT(&g_dcmi_handle, DCMI_IT_LINE | DCMI_IT_VSYNC | DCMI_IT_ERR | DCMI_IT_OVR);
  HAL_NVIC_SetPriority(DCMI_IRQn, 0, 0);                      /* 抢占优先级0，子优先级0 */
  HAL_NVIC_EnableIRQ(DCMI_IRQn);                              /* 使能DCMI中断 */
}

/*********************************************************************************************
* 名称：DCMI_Start
* 功能：DCMI,启动传输
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void DCMI_Start(void)
{
  /* 设置LCD自动填充区域 */ 
  LCD_Set_Window((lcd_dev.height-IMAGE_WIN_HEIGHT)/2,
                 (lcd_dev.height-IMAGE_WIN_HEIGHT)/2,
                 IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT);
  LCD_WriteRAM_Prepare();       //开始写入GRAM
  
  /* 清理标志位并重新启动DCMI捕获数据 */
//  DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | 
//                DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);  //防止DMA使能不了
  __HAL_DMA_ENABLE(&g_dma_dcmi_handle);   /* 使能DMA */
  DCMI->CR |= DCMI_CR_CAPTURE;            /* DCMI捕获使能 */  
  /* 重新使能帧中断,因为HAL_DCMI_IRQHandler()函数会关闭帧中断 */
  __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);    //开启帧中断 
}

//DCMI,关闭传输
void DCMI_Stop(u8 isr)
{
  __HAL_DCMI_DISABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);   //失能帧中断 
  DCMI->CR &= ~(DCMI_CR_CAPTURE);   /* DCMI捕获关闭 */
  if(!isr)
    while (DCMI->CR & 0X01);       //在DCMI的帧中断中不需要等待
  __HAL_DMA_DISABLE(&g_dma_dcmi_handle);  /* 关闭DMA */
}
