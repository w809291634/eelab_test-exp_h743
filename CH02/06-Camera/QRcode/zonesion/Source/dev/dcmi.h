#ifndef _DCMI_H
#define _DCMI_H
#include "extend.h"   									
#include "gui.h"

#define Dma_MultiBuffer_size  4 * 1024                          /* 定义DMA接收数据时,双缓存区每个缓存区的大小 */
#define USE_HAL_IRQHANDLER    1                                 // 使用hal库的中断处理函数

/* 优先级设定 */
#define DMA_IRQ_PRO           0
#define DCMI_IRQ_PRO          1

extern volatile u8 ov_frame;                                    //帧率
extern volatile u8 ov_frame_cnt;

extern DCMI_HandleTypeDef g_dcmi_handle;                        /* DCMI句柄 */
extern DMA_HandleTypeDef g_dma_dcmi_handle;                     /* DMA句柄 */

extern uint32_t g_dcmi_multi_buf[2][Dma_MultiBuffer_size];      /* 定义DMA双缓冲buf，能够接收超长数据 */
extern volatile uint32_t dcmi_data_len;                         /* dcmi 当前数据接收长度 */

void DCMI_init(void);
void DCMI_DMA_Init(uint32_t mem0addr, uint32_t mem1addr, uint16_t memsize, uint32_t memblen, uint32_t meminc);
void DCMI_Start(void);
void DCMI_Stop(u8 isr);
  
void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync);
void lcd_refresh(void);
#endif
