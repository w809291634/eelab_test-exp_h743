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
* 名称:sys_cache_enable
* 功能:使能STM32H7的L1-Cache, 同时开启D cache的强制透写
* 参数:无
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void sys_cache_enable(void)
{
    SCB_EnableICache();     /* 使能I-Cache,函数在core_cm7.h里面定义 */
    SCB_EnableDCache();     /* 使能D-Cache,函数在core_cm7.h里面定义 */
    SCB->CACR |= 1 << 2;    /* 强制D-Cache透写,如不开启透写,实际使用中可能遇到各种问题 */
}

/*********************************************************************************************
* 名称:SystemClock_Config
* 功能:系统时钟配置
*        使能STM32H7的L1-Cache, 同时开启D cache的强制透写
* 参数:无
* 返回:无
* 修改:无
* 注释:cubemx中配置
 *        外部晶振为8M的时候, 推荐值: plln = 200, pllm = 2, pllp = 2, pllq = 4.
 *        得到:Fvco = 8 * (200 / 2) = 800Mhz
 *                Fsys = pll1_p_ck = 800 / 2 = 400Mhz
 *                Fq   = pll1_q_ck = 800 / 4 = 200Mhz
 *        H743默认需要配置的频率如下:
 *                CPU频率(rcc_c_ck) = sys_d1cpre_ck = 400Mhz
 *                rcc_aclk = rcc_hclk3 = 200Mhz
 *                AHB1/2/3/4(rcc_hclk1/2/3/4) = 200Mhz
 *                APB1/2/3/4(rcc_pclk1/2/3/4) = 100Mhz
 *                pll2_p_ck = (25 / 25) * 440 / 2) = 220Mhz
 *                pll2_r_ck = FMC时钟频率 = ((25 / 25) * 440 / 2) = 220Mhz
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);    /* VOS = 1, Scale1, 1.2V内核电压,FLASH访问可以得到最高性能 */

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}                    /* 等待电压稳定 */
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  /* 使能HSE，并选择HSE作为PLL时钟源，配置PLL1，开启USB时钟 */
  /* 开启HSI 时钟，主要用于ADC，ADC选择PER_CK.输入频率为64MHZ */
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
   *  选择PLL的输出作为系统时钟
   *  配置RCC_CLOCKTYPE_SYSCLK系统时钟,400M
   *  配置RCC_CLOCKTYPE_HCLK 时钟,200Mhz,对应AHB1，AHB2，AHB3和AHB4总线
   *  配置RCC_CLOCKTYPE_PCLK1时钟,100Mhz,对应APB1总线
   *  配置RCC_CLOCKTYPE_PCLK2时钟,100Mhz,对应APB2总线
   *  配置RCC_CLOCKTYPE_D1PCLK1时钟,100Mhz,对应APB3总线
   *  配置RCC_CLOCKTYPE_D3PCLK1时钟,100Mhz,对应APB4总线
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
  
  HAL_PWREx_EnableUSBVoltageDetector();   /* 使能USB电压电平检测器 */
  __HAL_RCC_CSI_ENABLE() ;                /* 使能CSI时钟 */
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;         /* 使能SYSCFG时钟 */
  HAL_EnableCompensationCell();           /* 使能IO补偿单元 */
}

/*********************************************************************************************
* 名称:PeriphCommonClock_Config
* 功能:外设通用时钟配置
* 参数:无
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;     // 使用HSI作为CKPER的时钟输入源,当ADC时钟输入源选择CKPER，这里需要明确配置
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/*********************************************************************************************
* 名称:BSP_MPU_ConfigRegion
* 功能:配置MPU区域属性和大小寄存器值
* 参数: Region  		MPU保护区域，取值范围（0—7）
*        DisableExec  		指令访问禁用位，0=允许指令访问，1=禁止指令访问。
*        AccessPermission	数据访问权限，取值范围（MPU_NO_ACCESS—MPU_PRIV_RO_URO）
*        TypeExtField 	                键入扩展字段，允许您配置内存访问类型，例如强排序，外设。可以取值（0，1，2）
*        Address 	                MPU保护区域基地址，特别注意配置的Address需要被Size整除
*        Size 		MPU保护区域大小,可以取值（MPU_1KB，MPU_4KB ...MPU_512MB）
*        IsShareable 	                保护的存储空间是否可以共享，1=允许共享，0=禁止共享。
*        IsCacheable 		保护的存储空间是否可以缓存，1=允许缓存，0=禁止缓存。
*        IsBufferable 	                 使能Cache之后，策略是write-through还是write-back(bufferable)，1=允许缓冲，即回写（write-back），0=禁止缓冲，即直写（write-through）。
* 返回:无
* 修改:无
* 注释:无
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
  
  /* 禁用MPU */
  HAL_MPU_Disable();

  /* 配置MPU属性*/
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;											
  MPU_InitStruct.BaseAddress = Address;									//区域基地址。
  MPU_InitStruct.Size = Size;														//要配置的区域的区域大小。
  MPU_InitStruct.AccessPermission = AccessPermission;		//数据访问权限允许您配置用户和特权模式的读/写访问权限。
  MPU_InitStruct.IsBufferable = IsBufferable;						//区域是可缓冲的，即使用回写缓存。 可缓存但不可缓冲的区域使用直写策略。
  MPU_InitStruct.IsCacheable = IsCacheable;							//区域是否可缓存的，即其值是否可以保存在缓存中。
  MPU_InitStruct.IsShareable = IsShareable;							//区域是否可以在多个总线主控器之间共享。
  MPU_InitStruct.Number = Region;												//区域保护。
  MPU_InitStruct.TypeExtField = TypeExtField;						//键入扩展字段，允许您配置内存访问类型。
  MPU_InitStruct.SubRegionDisable = 0x00;								//子区域禁用字段。
  MPU_InitStruct.DisableExec = DisableExec;							//指令访问禁用位。

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* 启用MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void Board_MPU_Config(uint8_t	Region ,uint8_t Mode,uint32_t Address,uint32_t Size)
{
	switch(Mode)
	{
		case MPU_Normal_WB:
				 /* 设置内存为Normal类型,禁用共享, 回写模式不带写入读取分配*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,0,Address,Size,0,1,1);	
				 break;

		case MPU_Normal_WBWARA:
				 /* 设置内存为Normal类型,禁用共享, 回写模式带写入读取分配*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,1,Address,Size,0,1,1);	
				 break;
		
		case MPU_Normal_WT:
				 /* 设置内存为Normal类型,禁用共享, 直写模式*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,0,Address,Size,0,1,0);	
				 break;		
		
		case MPU_Normal_NonCache:
				 /* 设置内存为Normal类型,禁用共享，禁用缓存模式*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,1,Address,Size,0,0,0);	
				 break;
		
		case MPU_Device_NonCache:
				 /*设置内存为Device类型,共享设置无效，禁止共享，禁用缓存模式*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,2,Address,Size,0,0,0);	
				 break;
	}
	
}

#ifdef  USE_FULL_ASSERT

/*********************************************************************************************
* 名称:assert_failed
* 功能:当编译提示出错的时候此函数用来报告错误的文件和所在行
* 参数:file：指向源文件
*        line：指向在文件中的行数
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void (uint8_t *file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif
