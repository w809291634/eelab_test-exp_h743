/*********************************************************************************************
* �ļ���MP-4.c
* ���ߣ�Lixm 2017.10.17
* ˵������ȼ������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "MP-4.h"

ADC_HandleTypeDef g_adc_handle;           /* ADC��� */
/*********************************************************************************************
* ���ƣ�combustiblegas_init()
* ���ܣ���ȼ���崫������ʼ��  PC0-->ADC123ͨ��10 ���� ȼ��������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void combustiblegas_init(void)
{
  ADC_MultiModeTypeDef multimode = {0};

  g_adc_handle.Instance = ADC_ADCX;
  g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;            /* 2��Ƶ��ADCCLK=PER_CK/2=64/2=32MHZ */
  g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;                      /* 12λģʽ */
  g_adc_handle.Init.ScanConvMode = DISABLE;                               /* ��ɨ��ģʽ */
  g_adc_handle.Init.ContinuousConvMode = DISABLE;                         /* �ر�����ת�� */
  g_adc_handle.Init.DiscontinuousConvMode = DISABLE;                      /* ��ֹ����������ģʽ */
  g_adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                   /* �ر�EOC�ж� */
  g_adc_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                   /* ���µ����ݵ�����ֱ�Ӹ��ǵ������� */
  g_adc_handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;                 /* ���ݲ��������� */
  g_adc_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;     /* ����ͨ�������ݽ���������DR�Ĵ������� */
  g_adc_handle.Init.LowPowerAutoWait = DISABLE;                           /* �Զ��͹��Ĺر� */
  
  g_adc_handle.Init.NbrOfConversion = 1;                                  /* 1��ת���ڹ��������� Ҳ����ֻת����������1 */
  g_adc_handle.Init.NbrOfDiscConversion = 0;                              /* ����������ͨ����Ϊ0 */
  g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* ������� */
  g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* ʹ��������� */
  g_adc_handle.Init.OversamplingMode = DISABLE;                           /* �������ر� */
  if (HAL_ADC_Init(&g_adc_handle) != HAL_OK)                              /* ��ʼ��  */
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;                                  /* ADC����ģʽ  */
  if (HAL_ADCEx_MultiModeConfigChannel(&g_adc_handle, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_ADCEx_Calibration_Start(&g_adc_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED); /* ADCУ׼ */
}

/*********************************************************************************************
* ���ƣ�HAL_ADC_MspInit()
* ���ܣ�ADC�ײ�Ӳ����ʼ��
* ������adcHandle ADC���
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
    /** ѡ��ADC������ʱ��Դ
    */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_CLKP; /* ADCѡ��ʱ��ԴΪ PER_CK */
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    ADC_ADCX_CHY_CLK_ENABLE();
    ADC_ADCX_CHY_GPIO_CLK_ENABLE();                             /* ʱ��ʹ�� */

    GPIO_InitStruct.Pin = ADC_ADCX_CHY_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &GPIO_InitStruct);    /* ��ʼ������ */
  }
}

/*********************************************************************************************
* ���ƣ�HAL_ADC_MspDeInit()
* ���ܣ�ADC�ײ�Ӳ��ж��
* ������adcHandle ADC���
* ���أ���
* �޸ģ�
* ע�ͣ�
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
* ���ƣ�get_combustiblegas_status(void)
* ���ܣ���ȡ��ȼ���崫����״̬
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned int get_combustiblegas_data(void)
{ 
  ADC_ChannelConfTypeDef adc_ch_conf;

  adc_ch_conf.Channel = ADC_ADCX_CHY;                     /* ͨ�� */
  adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                  /* �ڹ���ͨ�����ź�Ϊ1 */
  adc_ch_conf.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* ����ʱ�䣬��������������: 810.5��ADC���� */
  adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED;              /* ���߲ɼ� */
  adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;
  adc_ch_conf.Offset = 0;   
  HAL_ADC_ConfigChannel(&g_adc_handle, &adc_ch_conf);     /* ͨ������ */

  HAL_ADC_Start(&g_adc_handle);                           /* ����ADC */

  HAL_ADC_PollForConversion(&g_adc_handle, 10);           /* ��ѯת�� */
  return HAL_ADC_GetValue(&g_adc_handle);                 /* �������һ��ADC1�������ת����� */
}