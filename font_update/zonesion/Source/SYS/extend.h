#ifndef _EXTEND_H_
#define _EXTEND_H_
#include "stm32h7xx_hal.h"
#include "stdio.h"
#include "font_config.h"

/*!< STM32F10x Standard Peripheral Library old types (maintained for legacy purpose) */
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

/** debug 层控制 **/
// 0xff 显示所有层的信息
// 0x00 所有层的信息都不显示
#define DEBUG           0x0f        // 0x01-显示 debug 信息 
                                    // 0x02-显示 error 信息 
                                    // 0x04-显示 warning 信息 
                                    // 0x08-显示 info 信息 
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
void assert_failed(uint8_t *file, uint32_t line);
void Error_Handler(void);
#endif
