/*********************************************************************************************
* 文件：MP-4.c
* 作者：Lixm 2017.10.17
* 说明：可燃气体驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "MP-4.h"

ADC_HandleTypeDef g_adc_handle;           /* ADC句柄 */
/*********************************************************************************************
* 名称：combustiblegas_init()
* 功能：可燃气体传感器初始化  PC0-->ADC123通道10 连接 燃气传感器
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void combustiblegas_init(void)
{
  ADC_MultiModeTypeDef multimode = {0};

  g_adc_handle.Instance = ADC_ADCX;
  g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;            /* 2分频，ADCCLK=PER_CK/2=64/2=32MHZ */
  g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;                      /* 12位模式 */
  g_adc_handle.Init.ScanConvMode = DISABLE;                               /* 非扫描模式 */
  g_adc_handle.Init.ContinuousConvMode = DISABLE;                         /* 关闭连续转换 */
  g_adc_handle.Init.DiscontinuousConvMode = DISABLE;                      /* 禁止不连续采样模式 */
  g_adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                   /* 关闭EOC中断 */
  g_adc_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                   /* 有新的数据的死后直接覆盖掉旧数据 */
  g_adc_handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;                 /* 数据不进行左移 */
  g_adc_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;     /* 规则通道的数据仅仅保存在DR寄存器里面 */
  g_adc_handle.Init.LowPowerAutoWait = DISABLE;                           /* 自动低功耗关闭 */
  
  g_adc_handle.Init.NbrOfConversion = 1;                                  /* 1个转换在规则序列中 也就是只转换规则序列1 */
  g_adc_handle.Init.NbrOfDiscConversion = 0;                              /* 不连续采样通道数为0 */
  g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
  g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 使用软件触发 */
  g_adc_handle.Init.OversamplingMode = DISABLE;                           /* 过采样关闭 */
  if (HAL_ADC_Init(&g_adc_handle) != HAL_OK)                              /* 初始化  */
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;                                  /* ADC独立模式  */
  if (HAL_ADCEx_MultiModeConfigChannel(&g_adc_handle, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_ADCEx_Calibration_Start(&g_adc_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED); /* ADC校准 */
}

/*********************************************************************************************
* 名称：HAL_ADC_MspInit()
* 功能：ADC底层硬件初始化
* 参数：adcHandle ADC句柄
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
    /** 选择ADC的外设时钟源
    */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_CLKP; /* ADC选择时钟源为 PER_CK */
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    ADC_ADCX_CHY_CLK_ENABLE();
    ADC_ADCX_CHY_GPIO_CLK_ENABLE();                             /* 时钟使能 */

    GPIO_InitStruct.Pin = ADC_ADCX_CHY_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &GPIO_InitStruct);    /* 初始化引脚 */
  }
}

/*********************************************************************************************
* 名称：HAL_ADC_MspDeInit()
* 功能：ADC底层硬件卸载
* 参数：adcHandle ADC句柄
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
  if(adcHandle->Instance==ADC1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_ADC12_CLK_DISABLE();
    HAL_GPIO_DeInit(ADC_ADCX_CHY_GPIO_PORT, ADC_ADCX_CHY_GPIO_PIN);
  }
}

/*********************************************************************************************
* 名称：get_combustiblegas_status(void)
* 功能：获取可燃气体传感器状态
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned int get_combustiblegas_data(void)
{ 
  ADC_ChannelConfTypeDef adc_ch_conf;

  adc_ch_conf.Channel = ADC_ADCX_CHY;                     /* 通道 */
  adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                  /* 在规则通道内排号为1 */
  adc_ch_conf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* 采样时间，设置最大采样周期: 810.5个ADC周期 */
  adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED;              /* 单边采集 */
  adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;
  adc_ch_conf.Offset = 0;   
  HAL_ADC_ConfigChannel(&g_adc_handle, &adc_ch_conf);     /* 通道配置 */

  HAL_ADC_Start(&g_adc_handle);                           /* 开启ADC */

  HAL_ADC_PollForConversion(&g_adc_handle, 10);           /* 轮询转换 */
  return HAL_ADC_GetValue(&g_adc_handle);                 /* 返回最近一次ADC1规则组的转换结果 */
}