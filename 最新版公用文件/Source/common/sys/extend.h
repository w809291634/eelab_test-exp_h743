#ifndef _EXTEND_H_
#define _EXTEND_H_
#include "stm32h7xx_hal.h"
#include "stdio.h"
//#include "font_config.h"

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  /*!< Read Only */
typedef __I int16_t vsc16;  /*!< Read Only */
typedef __I int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  /*!< Read Only */
typedef __I uint16_t vuc16;  /*!< Read Only */
typedef __I uint8_t vuc8;   /*!< Read Only */

/* This type MUST be 8 bit */
typedef unsigned char	BYTE;

/* These types MUST be 16 bit */
typedef short			SHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types MUST be 16 bit or 32 bit */
typedef int				INT;
typedef unsigned int	UINT;

/* These types MUST be 32 bit */
typedef long			LONG;
typedef unsigned long	DWORD;

/** debug ����� **/
// 0xff ��ʾ���в����Ϣ
// 0x00 ���в����Ϣ������ʾ
#define MY_DEBUG
#define DEBUG           0x0f        // 0x01-��ʾ debug ��Ϣ 
                                    // 0x02-��ʾ error ��Ϣ 
                                    // 0x04-��ʾ warning ��Ϣ 
                                    // 0x08-��ʾ info ��Ϣ 
#define ERR "ERROR:"
#define WARNING "WARNING:"
#define INFO "INFORMATION:"

#if (DEBUG & 0x01)
#define debug printf
#else
#define debug(...)
#endif

#if (DEBUG & 0x02)
#define debug_err printf
#else
#define debug_err(...)
#endif

#if (DEBUG & 0x04)
#define debug_war printf
#else
#define debug_war(...)
#endif

#if (DEBUG & 0x08)
#define debug_info printf
#else
#define debug_info(...)
#endif

#define ARRAY_SIZE(p) (sizeof(p)/sizeof(p[0]))

void sys_cache_enable(void);
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void assert_failed(uint8_t *file, uint32_t line);
void Error_Handler(void);

/***************************MPU���ú���*****************************/

//����MPU->RASR�Ĵ���AP[26:24]λ������ֵ

#define	MPU_1KB		MPU_REGION_SIZE_1KB
#define	MPU_2KB		MPU_REGION_SIZE_2KB
#define	MPU_4KB		MPU_REGION_SIZE_4KB
#define	MPU_8KB		MPU_REGION_SIZE_8KB
#define	MPU_16KB	MPU_REGION_SIZE_16KB
#define	MPU_32KB	MPU_REGION_SIZE_32KB
#define	MPU_64KB	MPU_REGION_SIZE_64KB
#define	MPU_128KB	MPU_REGION_SIZE_128KB
#define	MPU_256KB	MPU_REGION_SIZE_256KB
#define	MPU_512KB	MPU_REGION_SIZE_512KB

#define	MPU_1MB		MPU_REGION_SIZE_1MB
#define	MPU_2MB		MPU_REGION_SIZE_2MB
#define	MPU_4MB		MPU_REGION_SIZE_4MB
#define	MPU_8MB		MPU_REGION_SIZE_8MB
#define	MPU_16MB	MPU_REGION_SIZE_16MB
#define	MPU_32MB	MPU_REGION_SIZE_32MB
#define	MPU_64MB	MPU_REGION_SIZE_64MB
#define	MPU_128MB	MPU_REGION_SIZE_128MB
#define	MPU_256MB	MPU_REGION_SIZE_256MB
#define	MPU_512MB	MPU_REGION_SIZE_512MB


#define  MPU_NO_ACCESS      ((uint8_t)0x00U)
#define  MPU_PRIV_RW        ((uint8_t)0x01U)
#define  MPU_PRIV_RW_URO    ((uint8_t)0x02U)
#define  MPU_FULL_ACCESS    ((uint8_t)0x03U)
#define  MPU_PRIV_RO        ((uint8_t)0x05U)
#define  MPU_PRIV_RO_URO    ((uint8_t)0x06U)

/*����ΪNormal��cache������Ϊ write-back����������cache�� 
 *�ں��ʵ�ʱ��(��cache���Ծ����������ǿ�Ƹ���)�����ݸ��µ���Ӧ��SRAM�ռ�
 *�ر�ע�⣺���Ҫ�����������£�д֮��ҪSCB_CleanDCache��������ʱҪSCB_InvalidateDCache
 */
#define	MPU_Normal_WB									0x00  


/*����ΪNormal��cache������Ϊ write-back����������cache�� 
 *�ں��ʵ�ʱ��(��cache���Ծ����������ǿ�Ƹ���)�����ݸ��µ���Ӧ��SRAM�ռ�
 *�ر�ע�⣺���Ҫ�����������£�д֮��ҪSCB_CleanDCache��������ʱҪSCB_InvalidateDCache
 */
#define	MPU_Normal_WBWARA							0x01  //�ⲿ���ڲ�д����д�����


/*����Ϊ normal��cache������Ϊ Write-through��������cache��ͬʱ��
 *������ͬʱд����Ӧ�������ַ�ռ�
 *�ر�ע�⣺���Ҫ�����������£�����ֱ�����ڴ�д���ݣ���������ʱҪSCB_InvalidateDCache
 */
#define	MPU_Normal_WT									0x02


/*����Ϊ normal�����ù���,���û���
 */
#define	MPU_Normal_NonCache						0x03


/*����Ϊ Device������������Ч�����ù���,���û���
 */
#define	MPU_Device_NonCache						0x04


void Board_MPU_Config(uint8_t	Region ,uint8_t Mode,uint32_t Address,uint32_t Size);
#endif
