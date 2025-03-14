#ifndef __GT911_H_
#define __GT911_H_

// 引脚定义
// RST---PF2  EXTI---PG2
#define GT911_RST_GPIO_PORT               GPIOF
#define GT911_RST_GPIO_PIN                GPIO_PIN_2
#define GT911_RST_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* 时钟使能 */

#define GT911_EXTI_GPIO_PORT               GPIOG
#define GT911_EXTI_GPIO_PIN                GPIO_PIN_2
#define GT911_EXTI_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)   /* 时钟使能 */

/* IO操作 */
#define GT911_RST(x)        do{ x ? \
                              HAL_GPIO_WritePin(GT911_RST_GPIO_PORT, GT911_RST_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GT911_RST_GPIO_PORT, GT911_RST_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0) 

#define GT911_EXTI(x)       do{ x ? \
                              HAL_GPIO_WritePin(GT911_EXTI_GPIO_PORT, GT911_EXTI_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GT911_EXTI_GPIO_PORT, GT911_EXTI_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0) 

#define GT911_RST_L           GT911_RST(0)
#define GT911_RST_H           GT911_RST(1)

#define GT911_EXTI_L          GT911_EXTI(0)
#define GT911_EXTI_H          GT911_EXTI(1)

// IIC 命令定义
#define GT911_CMD_WR          0x28
#define GT911_CMD_RD          0x29

#define GT911_CTRL_REG        0x8040
#define GT911_CFG_REG         0x8047
#define GT911_CHECK_REG       0X80FF
#define GT911_PID_REG         0x8140
#define GT911_GSTID_REG       0X814E

// 触摸定义
#define GT911_TOUCH_NUM       5

typedef struct{
  unsigned char id;
  unsigned short x;
  unsigned short y;
  unsigned short size;
}GT911_point_t;

// 外部引用
extern GT911_point_t GT911Point[GT911_TOUCH_NUM];
extern unsigned char GT911_pointNum;

int GT911_init(void);
void GT911_scan(void);

#endif // __GT911_H_
