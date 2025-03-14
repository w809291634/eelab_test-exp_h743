/*********************************************************************************************
* 文件：ADC.c
* 作者：chennian 2017.10.25
* 说明：延时函数     
* 修改：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "ADC.h"

ADC_HandleTypeDef g_adc_handle;           /* ADC句柄 */

/**
 * @brief       ADC底层驱动，引脚配置，时钟使能
 * @param       此函数会被HAL_ADC_Init()调用
 * @param       hadc:ADC句柄
 * @retval      无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hadc->Instance==ADC3)
  {
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_CLKP;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_ADC3_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**ADC3 GPIO Configuration
    PC3_C     ------> ADC3_INP1
    */
    HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC3, SYSCFG_SWITCH_PC3_OPEN);
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_ADC3_CLK_DISABLE();
  }
}

/*********************************************************************************************
* 名称：AdcInit
* 功能：adc初始化
* 参数：无
* 返回：无
* 注意：
* 修改：无
*********************************************************************************************/
void  AdcInit(void)
{
  // PC3_C-----ADC3_CH1
  g_adc_handle.Instance = ADC3;
  g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;            /* 2分频，ADCCLK=PER_CK/2=64/2=32MHZ */
  g_adc_handle.Init.Resolution = ADC_RESOLUTION_16B;                      /* 16位模式 */
  g_adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                      /* 非扫描模式 */
  g_adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                   /* 关闭EOC中断 */
  g_adc_handle.Init.LowPowerAutoWait = DISABLE;                           /* 自动低功耗关闭 */
  g_adc_handle.Init.ContinuousConvMode = DISABLE;                         /* 关闭连续转换 */
  g_adc_handle.Init.NbrOfConversion = 1;                                  /* 1个转换在规则序列中 也就是只转换规则序列1 */
  g_adc_handle.Init.DiscontinuousConvMode = DISABLE;                      /* 禁止不连续采样模式 */
  g_adc_handle.Init.NbrOfDiscConversion = 0;                              /* 不连续采样通道数为0 */
  g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
  g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 使用软件触发 */
  g_adc_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;     /* 规则通道的数据仅仅保存在DR寄存器里面 */
  g_adc_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                   /* 有新的数据的死后直接覆盖掉旧数据 */
  g_adc_handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  g_adc_handle.Init.OversamplingMode = DISABLE;                           /* 过采样关闭 */
  if (HAL_ADC_Init(&g_adc_handle) != HAL_OK)                              /* 初始化  */
  {
    Error_Handler();
  }
  HAL_ADCEx_Calibration_Start(&g_adc_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED); /* ADC校准 */
}

 /*********************************************************************************************
* 名称：AdcGet
* 功能：ADC转换函数
* 参数：ch: 通道值 0~19，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_19
* 返回：adc1转换结果
* 修改：无
*********************************************************************************************/
uint32_t AdcGet(uint32_t ch)   
{
  ADC_ChannelConfTypeDef adc_ch_conf;

  adc_ch_conf.Channel = ch;                               /* 通道 */
  adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                  /* 1个序列 */
  adc_ch_conf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* 采样时间，设置最大采样周期: 810.5个ADC周期 */
  adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED;              /* 单边采集 */
  adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;
  adc_ch_conf.Offset = 0;   
  adc_ch_conf.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&g_adc_handle, &adc_ch_conf) != HAL_OK)/* 通道配置 */
  {
    Error_Handler();
  }
  
  HAL_ADC_Start(&g_adc_handle);                           /* 开启ADC */

  HAL_ADC_PollForConversion(&g_adc_handle, 10);           /* 轮询转换 */
  return HAL_ADC_GetValue(&g_adc_handle);                 /* 返回最近一次ADC1规则组的转换结果 */
}
