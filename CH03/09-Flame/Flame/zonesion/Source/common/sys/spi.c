#include "spi.h"
#include "stdio.h"

SPI_HandleTypeDef hspi1;                  /* SPI1��� */

/*********************************************************************************************
* ���ƣ�HAL_SPI_MspInit
* ���ܣ�SPI MSP�� MCU���ų�ʼ��
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hspi->Instance==SPI1)
  {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;     // ʹ��PLL1��ΪSPI1��ʱ������Դ
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
* ���ƣ�HAL_SPI_MspDeInit
* ���ܣ�SPI MCU ����ȡ����ʼ��
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
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
* ���ƣ�SPI1_Init
* ���ܣ�SPI�ڳ�ʼ��
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ�������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ
*          �������Ƕ�SPI1�ĳ�ʼ��
*********************************************************************************************/
void SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;                  //����SPI����ģʽ:����Ϊ��SPI
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;        //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;            //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;          //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;              //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
  hspi1.Init.NSS = SPI_NSS_SOFT;                      //ʹ�������ʽ����Ƭѡ����
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; //���岨����Ԥ��Ƶ��ֵ��ʹ��PLL1ʱ��Դ�������ԭʼ�ٶ�Ϊ200mhz���ٽ��з�Ƶ
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;             //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;             /* ��ֹTIģʽ  */
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; // ��ֹCRC�󣬴�λ��Ч 
  hspi1.Init.CRCPolynomial = 7;                       //CRCֵ����Ķ���ʽ
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;        /* ��ֹ������� */
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;   /* ����FIFO��С��һ�������� */
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  
  /* ��λ���� */
  if (HAL_SPI_DeInit(&hspi1) != HAL_OK)
  {
      Error_Handler();
  }

  /* ��ʼ������ */
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
      Error_Handler();
  }  
  SPI1_ReadWriteByte(0xff);//��������
}

/*********************************************************************************************
* ���ƣ�SPI1_ReadWriteByte
* ���ܣ�SPI1 ��дһ���ֽ�
* ������TxData:Ҫд����ֽ�
* ���أ�����ֵ:��ȡ�����ֽ�
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
u8 SPI1_ReadWriteByte(u8 TxData)
{
  /* ��ѯ��ʽ���� */    
  u8 g_spiRxBuf=0;
  if(HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&TxData, (uint8_t *)&g_spiRxBuf, 1, 1000000) != HAL_OK)
  {
    Error_Handler();
  }
  return g_spiRxBuf;
}
