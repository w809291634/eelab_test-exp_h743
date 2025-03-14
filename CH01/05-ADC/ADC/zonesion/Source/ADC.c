/*********************************************************************************************
* �ļ���ADC.c
* ���ߣ�chennian 2017.10.25
* ˵������ʱ����     
* �޸ģ�
*********************************************************************************************/
/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"
#include "ADC.h"

ADC_HandleTypeDef g_adc_handle;           /* ADC��� */

/**
 * @brief       ADC�ײ��������������ã�ʱ��ʹ��
 * @param       �˺����ᱻHAL_ADC_Init()����
 * @param       hadc:ADC���
 * @retval      ��
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
* ���ƣ�AdcInit
* ���ܣ�adc��ʼ��
* ��������
* ���أ���
* ע�⣺
* �޸ģ���
*********************************************************************************************/
void  AdcInit(void)
{
  // PC3_C-----ADC3_CH1
  g_adc_handle.Instance = ADC3;
  g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;            /* 2��Ƶ��ADCCLK=PER_CK/2=64/2=32MHZ */
  g_adc_handle.Init.Resolution = ADC_RESOLUTION_16B;                      /* 16λģʽ */
  g_adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                      /* ��ɨ��ģʽ */
  g_adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                   /* �ر�EOC�ж� */
  g_adc_handle.Init.LowPowerAutoWait = DISABLE;                           /* �Զ��͹��Ĺر� */
  g_adc_handle.Init.ContinuousConvMode = DISABLE;                         /* �ر�����ת�� */
  g_adc_handle.Init.NbrOfConversion = 1;                                  /* 1��ת���ڹ��������� Ҳ����ֻת����������1 */
  g_adc_handle.Init.DiscontinuousConvMode = DISABLE;                      /* ��ֹ����������ģʽ */
  g_adc_handle.Init.NbrOfDiscConversion = 0;                              /* ����������ͨ����Ϊ0 */
  g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* ������� */
  g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* ʹ��������� */
  g_adc_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;     /* ����ͨ�������ݽ���������DR�Ĵ������� */
  g_adc_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                   /* ���µ����ݵ�����ֱ�Ӹ��ǵ������� */
  g_adc_handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  g_adc_handle.Init.OversamplingMode = DISABLE;                           /* �������ر� */
  if (HAL_ADC_Init(&g_adc_handle) != HAL_OK)                              /* ��ʼ��  */
  {
    Error_Handler();
  }
  HAL_ADCEx_Calibration_Start(&g_adc_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED); /* ADCУ׼ */
}

 /*********************************************************************************************
* ���ƣ�AdcGet
* ���ܣ�ADCת������
* ������ch: ͨ��ֵ 0~19��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_19
* ���أ�adc1ת�����
* �޸ģ���
*********************************************************************************************/
uint32_t AdcGet(uint32_t ch)   
{
  ADC_ChannelConfTypeDef adc_ch_conf;

  adc_ch_conf.Channel = ch;                               /* ͨ�� */
  adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                  /* 1������ */
  adc_ch_conf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* ����ʱ�䣬��������������: 810.5��ADC���� */
  adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED;              /* ���߲ɼ� */
  adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;
  adc_ch_conf.Offset = 0;   
  adc_ch_conf.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&g_adc_handle, &adc_ch_conf) != HAL_OK)/* ͨ������ */
  {
    Error_Handler();
  }
  
  HAL_ADC_Start(&g_adc_handle);                           /* ����ADC */

  HAL_ADC_PollForConversion(&g_adc_handle, 10);           /* ��ѯת�� */
  return HAL_ADC_GetValue(&g_adc_handle);                 /* �������һ��ADC1�������ת����� */
}
