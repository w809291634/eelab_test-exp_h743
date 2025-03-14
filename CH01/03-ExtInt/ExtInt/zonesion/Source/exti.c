/*********************************************************************************************
* 文件：exti.c
* 说明：
* 功能：外部中断驱动代码
* 修改：增加消抖操作
        zhoucj   2017.12.18 修改注释
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "exti.h"
#include "key.h"
#include "delay.h"
extern char rgb_status;  
/*********************************************************************************************
* 名称：exti_init
* 功能：外部中断初始化
* 参数：无
* 返回：无
* 修改：无
*********************************************************************************************/
// PB14 -- 中断线14  PB15 -- 中断线15  
void exti_init(void)
{
  GPIO_InitTypeDef gpio_init_struct;                            /* GPIO配置参数存储变量 */

  key_init();                                                   //按键引脚初始化
  
  // GPIO 初始化
  __HAL_RCC_GPIOB_CLK_ENABLE(); 
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;                               //使能SYSCFG时钟
  
  gpio_init_struct.Pin = GPIO_PIN_14|GPIO_PIN_15;             /* 引脚 */
  gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;               /* 下降沿触发 */
  gpio_init_struct.Pull = GPIO_NOPULL;                        /* 没有上下拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
  HAL_GPIO_Init(GPIOB, &gpio_init_struct);                    /* 初始化引脚 */
  
  // EXTI 初始化
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 1);                 /* 抢占0，子优先级1 */
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);                         /* 使能中断线2 */
}
/*********************************************************************************************
* 名称：EXTI15_10_IRQHandler
* 功能：外部中断15-10中断处理函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void EXTI15_10_IRQHandler(void)
{
  if(get_key_status() == K3_PRES || 
     get_key_status() == K4_PRES ){                             //检测K3被按下
    delay_count(1000);                                           //延时消抖
    if(get_key_status() == K3_PRES || 
       get_key_status() == K4_PRES ){                            //确认K3被按下
      while(get_key_status() == K3_PRES || 
            get_key_status() == K4_PRES);                       //等待按键松开
      rgb_status = !rgb_status;                                 //翻转led状态标志
    }
  }
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != 0x00U)
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);                        /* 退出时再清一次中断，避免按键抖动误触发 */
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != 0x00U)
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15); 
}
