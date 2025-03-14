#include "spi.h"
#include "stdio.h"

SPI_HandleTypeDef hspi1;                  /* SPI1句柄 */

/*********************************************************************************************
* 名称：HAL_SPI_MspInit
* 功能：SPI MSP层 MCU引脚初始化
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hspi->Instance==SPI1)
  {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;     // 使用PLL1作为SPI1的时钟输入源
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
    PB4 (NJTRST)     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

/*********************************************************************************************
* 名称：HAL_SPI_MspDeInit
* 功能：SPI MCU 引脚取消初始化
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI1)
  {
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
    PB4 (NJTRST)     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
  }
}

/*********************************************************************************************
* 名称：SPI1_Init
* 功能：SPI口初始化
* 参数：无
* 返回：无
* 修改：无
* 注释：以下是SPI模块的初始化代码，配置成主机模式
*          这里针是对SPI1的初始化
*********************************************************************************************/
void SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;                  //设置SPI工作模式:设置为主SPI
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;        //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;            //设置SPI的数据大小:SPI发送接收8位帧结构
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;          //串行同步时钟的空闲状态为高电平
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;              //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
  hspi1.Init.NSS = SPI_NSS_SOFT;                      //使用软件方式管理片选引脚
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; //定义波特率预分频的值，使用PLL1时钟源，这里的原始速度为200mhz，再进行分频
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;             //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;             /* 禁止TI模式  */
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; // 禁止CRC后，此位无效 
  hspi1.Init.CRCPolynomial = 7;                       //CRC值计算的多项式
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;        /* 禁止脉冲输出 */
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;   /* 设置FIFO大小是一个数据项 */
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  
  /* 复位配置 */
  if (HAL_SPI_DeInit(&hspi1) != HAL_OK)
  {
      Error_Handler();
  }

  /* 初始化配置 */
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
      Error_Handler();
  }  
  SPI1_ReadWriteByte(0xff);//启动传输
}

/*********************************************************************************************
* 名称：SPI1_ReadWriteByte
* 功能：SPI1 读写一个字节
* 参数：TxData:要写入的字节
* 返回：返回值:读取到的字节
* 修改：无
* 注释：无
*********************************************************************************************/
u8 SPI1_ReadWriteByte(u8 TxData)
{
  /* 查询方式传输 */    
  u8 g_spiRxBuf=0;
  if(HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&TxData, (uint8_t *)&g_spiRxBuf, 1, 1000000) != HAL_OK)
  {
    Error_Handler();
  }
  return g_spiRxBuf;
}
