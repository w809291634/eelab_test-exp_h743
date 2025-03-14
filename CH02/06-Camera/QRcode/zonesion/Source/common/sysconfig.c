#include "extend.h"

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

/*********************************************************************************************
* ����:sys_cache_enable
* ����:ʹ��STM32H7��L1-Cache, ͬʱ����D cache��ǿ��͸д
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void sys_cache_enable(void)
{
    SCB_EnableICache();     /* ʹ��I-Cache,������core_cm7.h���涨�� */
    SCB_EnableDCache();     /* ʹ��D-Cache,������core_cm7.h���涨�� */
    SCB->CACR |= 1 << 2;    /* ǿ��D-Cache͸д,�粻����͸д,ʵ��ʹ���п��������������� */
}

/*********************************************************************************************
* ����:SystemClock_Config
* ����:ϵͳʱ������
*        ʹ��STM32H7��L1-Cache, ͬʱ����D cache��ǿ��͸д
* ����:��
* ����:��
* �޸�:��
* ע��:cubemx������
 *        �ⲿ����Ϊ8M��ʱ��, �Ƽ�ֵ: plln = 200, pllm = 2, pllp = 2, pllq = 4.
 *        �õ�:Fvco = 8 * (200 / 2) = 800Mhz
 *                Fsys = pll1_p_ck = 800 / 2 = 400Mhz
 *                Fq   = pll1_q_ck = 800 / 4 = 200Mhz
 *        H743Ĭ����Ҫ���õ�Ƶ������:
 *                CPUƵ��(rcc_c_ck) = sys_d1cpre_ck = 400Mhz
 *                rcc_aclk = rcc_hclk3 = 200Mhz
 *                AHB1/2/3/4(rcc_hclk1/2/3/4) = 200Mhz
 *                APB1/2/3/4(rcc_pclk1/2/3/4) = 100Mhz
 *                pll2_p_ck = (25 / 25) * 440 / 2) = 220Mhz
 *                pll2_r_ck = FMCʱ��Ƶ�� = ((25 / 25) * 440 / 2) = 220Mhz
*********************************************************************************************/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);    /* VOS = 1, Scale1, 1.2V�ں˵�ѹ,FLASH���ʿ��Եõ�������� */

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}                    /* �ȴ���ѹ�ȶ� */
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  /* ʹ��HSE����ѡ��HSE��ΪPLLʱ��Դ������PLL1������USBʱ�� */
  /* ����HSI ʱ�ӣ���Ҫ����ADC��ADCѡ��PER_CK.����Ƶ��Ϊ64MHZ */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  /*
   *  ѡ��PLL�������Ϊϵͳʱ��
   *  ����RCC_CLOCKTYPE_SYSCLKϵͳʱ��,400M
   *  ����RCC_CLOCKTYPE_HCLK ʱ��,200Mhz,��ӦAHB1��AHB2��AHB3��AHB4����
   *  ����RCC_CLOCKTYPE_PCLK1ʱ��,100Mhz,��ӦAPB1����
   *  ����RCC_CLOCKTYPE_PCLK2ʱ��,100Mhz,��ӦAPB2����
   *  ����RCC_CLOCKTYPE_D1PCLK1ʱ��,100Mhz,��ӦAPB3����
   *  ����RCC_CLOCKTYPE_D3PCLK1ʱ��,100Mhz,��ӦAPB4����
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  
  HAL_PWREx_EnableUSBVoltageDetector();   /* ʹ��USB��ѹ��ƽ����� */
  __HAL_RCC_CSI_ENABLE() ;                /* ʹ��CSIʱ�� */
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;         /* ʹ��SYSCFGʱ�� */
  HAL_EnableCompensationCell();           /* ʹ��IO������Ԫ */
}

/*********************************************************************************************
* ����:PeriphCommonClock_Config
* ����:����ͨ��ʱ������
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;     // ʹ��HSI��ΪCKPER��ʱ������Դ,��ADCʱ������Դѡ��CKPER��������Ҫ��ȷ����
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/*********************************************************************************************
* ����:BSP_MPU_ConfigRegion
* ����:����MPU�������Ժʹ�С�Ĵ���ֵ
* ����: Region  		MPU��������ȡֵ��Χ��0��7��
*        DisableExec  		ָ����ʽ���λ��0=����ָ����ʣ�1=��ָֹ����ʡ�
*        AccessPermission	���ݷ���Ȩ�ޣ�ȡֵ��Χ��MPU_NO_ACCESS��MPU_PRIV_RO_URO��
*        TypeExtField 	                ������չ�ֶΣ������������ڴ�������ͣ�����ǿ�������衣����ȡֵ��0��1��2��
*        Address 	                MPU�����������ַ���ر�ע�����õ�Address��Ҫ��Size����
*        Size 		MPU���������С,����ȡֵ��MPU_1KB��MPU_4KB ...MPU_512MB��
*        IsShareable 	                �����Ĵ洢�ռ��Ƿ���Թ���1=������0=��ֹ����
*        IsCacheable 		�����Ĵ洢�ռ��Ƿ���Ի��棬1=�����棬0=��ֹ���档
*        IsBufferable 	                 ʹ��Cache֮�󣬲�����write-through����write-back(bufferable)��1=�����壬����д��write-back����0=��ֹ���壬��ֱд��write-through����
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void BSP_MPU_ConfigRegion(	uint8_t		Region,
														uint8_t  	DisableExec,
														uint8_t 	AccessPermission,
														uint8_t 	TypeExtField, 
														uint32_t 	Address, 
														uint32_t 	Size,
														uint8_t 	IsShareable, 
														uint8_t  	IsCacheable, 
														uint8_t  	IsBufferable	)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  /* ����MPU */
  HAL_MPU_Disable();

  /* ����MPU����*/
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;											
  MPU_InitStruct.BaseAddress = Address;									//�������ַ��
  MPU_InitStruct.Size = Size;														//Ҫ���õ�����������С��
  MPU_InitStruct.AccessPermission = AccessPermission;		//���ݷ���Ȩ�������������û�����Ȩģʽ�Ķ�/д����Ȩ�ޡ�
  MPU_InitStruct.IsBufferable = IsBufferable;						//�����ǿɻ���ģ���ʹ�û�д���档 �ɻ��浫���ɻ��������ʹ��ֱд���ԡ�
  MPU_InitStruct.IsCacheable = IsCacheable;							//�����Ƿ�ɻ���ģ�����ֵ�Ƿ���Ա����ڻ����С�
  MPU_InitStruct.IsShareable = IsShareable;							//�����Ƿ�����ڶ������������֮�乲��
  MPU_InitStruct.Number = Region;												//���򱣻���
  MPU_InitStruct.TypeExtField = TypeExtField;						//������չ�ֶΣ������������ڴ�������͡�
  MPU_InitStruct.SubRegionDisable = 0x00;								//����������ֶΡ�
  MPU_InitStruct.DisableExec = DisableExec;							//ָ����ʽ���λ��

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* ����MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void Board_MPU_Config(uint8_t	Region ,uint8_t Mode,uint32_t Address,uint32_t Size)
{
	switch(Mode)
	{
		case MPU_Normal_WB:
				 /* �����ڴ�ΪNormal����,���ù���, ��дģʽ����д���ȡ����*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,0,Address,Size,0,1,1);	
				 break;

		case MPU_Normal_WBWARA:
				 /* �����ڴ�ΪNormal����,���ù���, ��дģʽ��д���ȡ����*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,1,Address,Size,0,1,1);	
				 break;
		
		case MPU_Normal_WT:
				 /* �����ڴ�ΪNormal����,���ù���, ֱдģʽ*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,0,Address,Size,0,1,0);	
				 break;		
		
		case MPU_Normal_NonCache:
				 /* �����ڴ�ΪNormal����,���ù������û���ģʽ*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,1,Address,Size,0,0,0);	
				 break;
		
		case MPU_Device_NonCache:
				 /*�����ڴ�ΪDevice����,����������Ч����ֹ�������û���ģʽ*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,2,Address,Size,0,0,0);	
				 break;
	}
	
}

#ifdef  USE_FULL_ASSERT

/*********************************************************************************************
* ����:assert_failed
* ����:��������ʾ�����ʱ��˺����������������ļ���������
* ����:file��ָ��Դ�ļ�
*        line��ָ�����ļ��е�����
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void (uint8_t *file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif
