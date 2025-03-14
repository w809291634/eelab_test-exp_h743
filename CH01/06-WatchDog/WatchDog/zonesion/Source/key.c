#include "key.h"
#include "delay.h"

// 初始化
void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;                          /* GPIO配置参数存储变量 */
    KEY1_GPIO_CLK_ENABLE();                                     /* KEY1时钟使能 */
    KEY2_GPIO_CLK_ENABLE();                                     /* KEY2时钟使能 */
    KEY3_GPIO_CLK_ENABLE();                                     /* KEY3时钟使能 */
    KEY4_GPIO_CLK_ENABLE();                                     /* KEY4时钟使能 */

    gpio_init_struct.Pin = KEY1_GPIO_PIN;                       /* KEY1引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(KEY1_GPIO_PORT, &gpio_init_struct);           /* KEY1引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = KEY2_GPIO_PIN;                       /* KEY2引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(KEY2_GPIO_PORT, &gpio_init_struct);           /* KEY2引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = KEY3_GPIO_PIN;                       /* KEY3引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(KEY3_GPIO_PORT, &gpio_init_struct);           /* KEY3引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = KEY4_GPIO_PIN;                       /* KEY4引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(KEY4_GPIO_PORT, &gpio_init_struct);           /* KEY4引脚模式设置,上拉输入 */
}

/*********************************************************************************************
* 名称：get_key_status
* 功能：按键管脚初始化
* 参数：无
* 返回：key_status
* 修改：
*********************************************************************************************/
char get_key_status(void)
{
  char key_status = 0;
  if(HAL_GPIO_ReadPin(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == 0)       //判断PB12引脚电平状态
    key_status |= K1_PRES;                                      //低电平key_status bit0位置1
  if(HAL_GPIO_ReadPin(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == 0)       //判断PB13引脚电平状态
    key_status |= K2_PRES;                                      //低电平key_status bit1位置1
  if(HAL_GPIO_ReadPin(KEY3_GPIO_PORT,KEY3_GPIO_PIN) == 0)       //判断PB14引脚电平状态
    key_status |= K3_PRES;                                      //低电平key_status bit2位置1
  if(HAL_GPIO_ReadPin(KEY4_GPIO_PORT,KEY4_GPIO_PIN) == 0)       //判断PB15引脚电平状态
    key_status |= K4_PRES;                                      //低电平key_status bit3位置1
  return key_status;
}


/*********************************************************************************************
* 名称：KEY_Scan
* 功能：按键处理函数
* 参数：mode: 0,不支持连续按; 1,支持连续按;
* 返回：无
* 修改：
* 注释：注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
*********************************************************************************************/

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_count(1000);//去抖动 
		key_up=0;
		if(KEY1==0)return K1_PRES;
		else if(KEY2==0)return K2_PRES;
		else if(KEY3==0)return K3_PRES;
		else if(KEY4==0)return K4_PRES;
	}else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	    
 	return 0;// 无按键按下
}
