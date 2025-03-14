#include "buzzer.h"

/*********************************************************************************************
* 名称:buzzer_init()
* 功能:蜂鸣器初始化
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void buzzer_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  BUZZER_GPIO_CLK_ENABLE();                                 /* 时钟使能 */
  
  gpio_init_struct.Pin = BUZZER_GPIO_PIN;                   /* LED引脚 */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;              /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLDOWN;                    /* 下拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;          /* 中速 */
  HAL_GPIO_Init(BUZZER_GPIO_PORT, &gpio_init_struct);       /* 初始化引脚 */  
  
  BUZZER_CLOSE;
}

/*********************************************************************************************
* 名称:buzzer_tweet()
* 功能:蜂鸣器响一下
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void buzzer_tweet(void)
{
  BUZZER_OPEN;
  delay_ms(30);
  BUZZER_CLOSE;
}
