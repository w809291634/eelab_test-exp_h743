#include "extend.h"
#include "dcmi.h" 
#include "ili9806.h"
#include "led.h" 
#include "ov2640.h" 
#include "stdio.h"
#include "qr_decoder_app.h"

volatile u8 ov_frame=0;                                         //帧率
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

uint32_t g_dcmi_multi_buf[2][Dma_MultiBuffer_size];  /* 定义DMA双缓冲buf，能够接收超长数据 */

volatile uint32_t dcmi_data_len = 0;                            /* dcmi 当前数据接收长度 */

DCMI_HandleTypeDef g_dcmi_handle;                               /* DCMI句柄 */
DMA_HandleTypeDef g_dma_dcmi_handle;                            /* DMA句柄 */

/*********************************************************************************************
* 名称:DCMI_IRQHandler
* 功能:DCMI中断服务函数
* 参数:无
* 返回:无
* 修改:无
* 注释:无
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
* 名称:HAL_DCMI_FrameEventCallback
* 功能:DCMI中断回调服务函数
* 参数:hdcmi:DCMI句柄
* 返回:无
* 修改:无
* 注释:捕获到一帧图像处理
*********************************************************************************************/
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  __HAL_DCMI_CLEAR_FLAG(&g_dcmi_handle, DCMI_FLAG_FRAMERI);    /* 清除帧中断 */
  DCMI_Stop(1);
  
  /* 设置LCD自动填充区域 */ 
#if USE_QR_IMG==1
  LCD_Set_Window((lcd_dev.width-QR_IMG_W)/2,
                 (lcd_dev.height-QR_IMG_H)/2-20,
                 QR_IMG_W,QR_IMG_H);
  qr_img_intercept();           //图像截取
#else
  LCD_Set_Window((lcd_dev.width-IMAGE_WIN_WIDTH)/2,
                 (lcd_dev.height-IMAGE_WIN_HEIGHT)/2-20,
                 IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT);
#endif
  LCD_WriteRAM_Prepare();       //开始写入GRAM
  lcd_refresh();                //将缓存的图像刷新到LCD上
  
  /* 中断的LCD应用程序,时间尽可能短 */
  DCMI_isr_app();
  
  /* 恢复采集 */
  ov_frame_cnt++;
  DCMI_Start();
}

/*********************************************************************************************
* 名称:void dcmi_rx_callback
* 功能:DMA双缓冲数据接收回调函数
* 参数:hdcmi:DCMI句柄
* 返回:无
* 修改:无
* 注释:在DMA1_Stream1_IRQHandler中断服务函数里面被调用
*        DCMI DMA接收回调函数, 仅双缓冲模式用到, 配合中断服务函数使用
*********************************************************************************************/
static void dcmi_rx_callback(void)
{
  uint16_t i;
  uint32_t *pbuf;
  
  if(dcmi_data_len>=IMAGE_WIN_WIDTH*IMAGE_WIN_HEIGHT/2) 
    return;                                               // 防止buf溢出，指针越界
  pbuf = (uint32_t *)dcmi_buf + dcmi_data_len;            /* 偏移到有效数据末尾 */
 
  if (DMA1_Stream1->CR & (1 << 19))                       /* buf0已满,正常处理buf1 */
  {
      for (i = 0; i < Dma_MultiBuffer_size; i++)
          pbuf[i] = g_dcmi_multi_buf[0][i];               /* 读取buf0里面的数据 */

      dcmi_data_len += Dma_MultiBuffer_size;              /* 偏移 */
  }
  else                                                    /* buf1已满,正常处理buf0 */
  {
      for (i = 0; i < Dma_MultiBuffer_size; i++)
          pbuf[i] = g_dcmi_multi_buf[1][i];               /* 读取buf1里面的数据 */

      dcmi_data_len += Dma_MultiBuffer_size;              /* 偏移 */
  }
  SCB_CleanInvalidateDCache();                            /* 清除无效化DCache */
}

/*********************************************************************************************
* 名称:DMA1_Stream1_IRQHandler
* 功能:DMA1_Stream1中断服务函数(仅双缓冲模式会用到)
* 参数:无
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void DMA1_Stream1_IRQHandler(void)
{
  uint32_t isr_value = READ_REG(g_dcmi_handle.Instance->MISR);
  if (__HAL_DMA_GET_FLAG(&g_dma_dcmi_handle, DMA_FLAG_TCIF1_5) != RESET)  /* DMA传输完成 */
  {
    __HAL_DMA_CLEAR_FLAG(&g_dma_dcmi_handle, DMA_FLAG_TCIF1_5);         /* 清除DMA传输完成中断标志位 */
    dcmi_rx_callback();                                                 /* 执行摄像头接收回调函数,读取数据等操作在这里面处理 */
  } 
}

/*********************************************************************************************
* 名称:DCMI_DMA_Init
* 功能:DCMI DMA配置
* 参数:mem0addr:存储器缓冲区1地址
*        mem1addr:存储器缓冲区2地址，如果使用DMA单缓存这里就不需要用到
*        memsize:设置传输长度，单位数据长度（以实际情况变化）    0~65535
*        memblen:存储器位宽  
*        meminc:存储器是否自动递增
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void DCMI_DMA_Init(uint32_t mem0addr, uint32_t mem1addr, uint16_t memsize, uint32_t memblen, uint32_t meminc)
{ 
  __HAL_RCC_DMA1_CLK_ENABLE();                                        /* 使能DMA1时钟 */
  __HAL_LINKDMA(&g_dcmi_handle, DMA_Handle, g_dma_dcmi_handle);       /* 将DMA与DCMI联系起来 */
  __HAL_DMA_DISABLE_IT(&g_dma_dcmi_handle, DMA_IT_TC);                /* 先关闭DMA传输完成中断(否则在使用MCU屏的时候会出现花屏的情况) */

  g_dma_dcmi_handle.Instance = DMA1_Stream1;                          /* DMA1数据流1 */
  g_dma_dcmi_handle.Init.Request = DMA_REQUEST_DCMI;                  /* DCMI的DMA请求 */
  g_dma_dcmi_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;            /* 外设到存储器 */
  g_dma_dcmi_handle.Init.PeriphInc = DMA_PINC_DISABLE;                /* 外设非增量模式 */
  g_dma_dcmi_handle.Init.MemInc = meminc;                             /* 存储器增量模式 */
  g_dma_dcmi_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;   /* 外设数据长度:32位 */
  g_dma_dcmi_handle.Init.MemDataAlignment = memblen;                  /* 存储器数据长度:8/16/32位 */
  g_dma_dcmi_handle.Init.Mode = DMA_CIRCULAR;                         /* 使用循环模式 */
  g_dma_dcmi_handle.Init.Priority = DMA_PRIORITY_HIGH;                /* 高优先级 */
  g_dma_dcmi_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;              /* 使能FIFO */
  g_dma_dcmi_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL; /* 使用1/2的FIFO */
  g_dma_dcmi_handle.Init.MemBurst = DMA_PBURST_SINGLE;                /* 存储器突发传输 */
  g_dma_dcmi_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;             /* 外设突发单次传输 */
  HAL_DMA_DeInit(&g_dma_dcmi_handle);                                 /* 先清除以前的设置 */
  HAL_DMA_Init(&g_dma_dcmi_handle);                                   /* 初始化DMA */

  /* 在开启DMA之前先使用__HAL_UNLOCK()解锁一次DMA,因为HAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
   * 这两个函数一开始要先使用__HAL_LOCK()锁定DMA,而函数__HAL_LOCK()会判断当前的DMA状态是否为锁定状态，如果是
   * 锁定状态的话就直接返回HAL_BUSY，这样会导致函数HAL_DMA_Statrt()和HAL_DMAEx_MultiBufferStart()后续的DMA配置
   * 程序直接被跳过！DMA也就不能正常工作，为了避免这种现象，所以在启动DMA之前先调用__HAL_UNLOCK()先解锁一次DMA。
   */
  __HAL_UNLOCK(&g_dma_dcmi_handle);
  
  if (mem1addr == 0)                                                                                  /* 开启DMA，不使用双缓冲 */
  {
    HAL_DMA_Start(&g_dma_dcmi_handle, (uint32_t)&DCMI->DR, mem0addr, memsize);
  }
  else                                                                                                /* 使用双缓冲 */
  {
    HAL_DMAEx_MultiBufferStart(&g_dma_dcmi_handle, (uint32_t)&DCMI->DR, mem0addr, mem1addr, memsize); /* 开启双缓冲 */
    __HAL_DMA_ENABLE_IT(&g_dma_dcmi_handle, DMA_IT_TC);                                               /* 开启传输完成中断 */
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, DMA_IRQ_PRO, 0);                                          /* DMA中断优先级 */
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  }
} 

/*********************************************************************************************
* 名称:HAL_DCMI_MspInit
* 功能:DCMI底层驱动，引脚配置，时钟使能，中断配置
* 参数:无
* 返回:无
* 修改:无
* 注释:此函数会被HAL_DCMI_Init()调用
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
* 名称:DCMI_init
* 功能:DCMI 初始化
* 参数:无
* 返回:无
* 修改:无
* 注释:IO对应关系如下:
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
  HAL_NVIC_SetPriority(DCMI_IRQn, DCMI_IRQ_PRO, 0);           /* 抢占优先级1，子优先级0 */
  HAL_NVIC_EnableIRQ(DCMI_IRQn);                              /* 使能DCMI中断 */
}

/*********************************************************************************************
* 名称:lcd_refresh
* 功能:将摄像头数据缓存更新到LCD上
* 参数:无
* 返回:无
* 修改:无
* 注释:IO对应关系如下:
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
* 名称:DCMI_Start
* 功能:DCMI,启动传输
* 参数:无
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void DCMI_Start(void)
{ 
  /* 清理标志位并重新启动DCMI捕获数据 */
  __HAL_DMA_CLEAR_FLAG(&g_dma_dcmi_handle,DMA_FLAG_FEIF1_5 | DMA_FLAG_DMEIF1_5 | 
                DMA_FLAG_TEIF1_5 | DMA_FLAG_HTIF1_5 | DMA_FLAG_TCIF1_5);    //防止DMA使能不了
  __HAL_UNLOCK(&g_dma_dcmi_handle);
  __HAL_DMA_ENABLE(&g_dma_dcmi_handle);   /* 使能DMA */
  
  DCMI->CR |= DCMI_CR_CAPTURE;            /* DCMI捕获使能 */  
  /* 重新使能帧中断,因为HAL_DCMI_IRQHandler()函数会关闭帧中断 */
  __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);    //开启帧中断 
}

/*********************************************************************************************
* 名称:DCMI_Stop
* 功能:DCMI,关闭传输
* 参数:无
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void DCMI_Stop(u8 isr)
{
  __HAL_DCMI_DISABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);   //失能帧中断 
  DCMI->CR &= ~(DCMI_CR_CAPTURE);                 /* DCMI捕获关闭 */
  if(!isr)
    while (DCMI->CR & 0X01);                      //在DCMI的帧中断中不需要等待
  __HAL_DMA_DISABLE(&g_dma_dcmi_handle);          /* 关闭DMA */
}
