/*********************************************************************************************
* 文件: led.c
* 作者：liutong 2015.8.19
* 说明：
* 修改：
* 注释：
*********************************************************************************************/
#include "led.h"

/*********************************************************************************************
* 名称：led_init
* 功能：初始化led对应的GPIO
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void led_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  LED1_GPIO_CLK_ENABLE();                                 /* LED1时钟使能 */
  LED2_GPIO_CLK_ENABLE();                                 /* LED2时钟使能 */
  LED3_GPIO_CLK_ENABLE();                                 /* LED3时钟使能 */
  LED4_GPIO_CLK_ENABLE();                                 /* LED4时钟使能 */
  
  gpio_init_struct.Pin = LED1_GPIO_PIN;                   /* LED引脚 */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        /* 中速 */
  HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_struct);       /* 初始化LED引脚 */

  gpio_init_struct.Pin = LED2_GPIO_PIN | LED3_GPIO_PIN | LED4_GPIO_PIN;
  HAL_GPIO_Init(LED2_GPIO_PORT, &gpio_init_struct);       /* 初始化LED引脚 */

  led_turn_off(LED1_NUM);
  led_turn_off(LED2_NUM);
  led_turn_off(LED3_NUM);
  led_turn_off(LED4_NUM);
}

/*********************************************************************************************
* 名称：led_turn_off
* 功能：置引脚高电平，关闭灯
* 参数：cmd
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void led_turn_off(unsigned char cmd){
  if(cmd & LED1_NUM)
    HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET);                        //置引脚低电平,关闭
  if(cmd & LED2_NUM)
    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_RESET);
  if(cmd & LED3_NUM)
    HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_RESET);
  if(cmd & LED4_NUM)
    HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_GPIO_PIN, GPIO_PIN_RESET);
}

/*********************************************************************************************
* 名称：led_turn_on
* 功能：置引脚低电平，打开灯
* 参数：cmd
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void led_turn_on(unsigned char cmd){
  if(cmd & LED1_NUM)
    HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_SET);                          //置引脚高电平，打开灯
  if(cmd & LED2_NUM)
    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_SET);
  if(cmd & LED3_NUM)
    HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_SET);
  if(cmd & LED4_NUM)
    HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_GPIO_PIN, GPIO_PIN_SET);
}

/*********************************************************************************************
* 名称：led_setState
* 功能：设置 LED 的状态
* 参数：cmd 具体控制的LED 号
            state 指定为新的状态
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void led_setState(uint16_t cmd, unsigned char state)
{
  if(state)
  {
    if(cmd & LED1_NUM)
      led_turn_on(LED1_NUM);
    if(cmd & LED2_NUM)         
      led_turn_on(LED2_NUM);
    if(cmd & LED3_NUM)         
      led_turn_on(LED3_NUM);
    if(cmd & LED4_NUM)         
      led_turn_on(LED4_NUM);
  }
  else
  {
    if(cmd & LED1_NUM)
      led_turn_off(LED1_NUM);
    if(cmd & LED2_NUM)         
      led_turn_off(LED2_NUM);
    if(cmd & LED3_NUM)         
      led_turn_off(LED3_NUM);
    if(cmd & LED4_NUM)         
      led_turn_off(LED4_NUM);
  }
}

/*********************************************************************************************
* 名称：led_ctrl
* 功能：LED 控制
* 参数：无
* 返回：无
* 修改：
* 注释：控制位为1表示点亮
*********************************************************************************************/
void led_ctrl(uint16_t cmd)
{
  if(cmd & LED1_NUM)
    led_turn_on(LED1_NUM);
  else                         
    led_turn_off(LED1_NUM);
  if(cmd & LED2_NUM)         
    led_turn_on(LED2_NUM);
  else                         
    led_turn_off(LED2_NUM);
  if(cmd & LED3_NUM)         
    led_turn_on(LED3_NUM);
  else                         
    led_turn_off(LED3_NUM);
  if(cmd & LED4_NUM)         
    led_turn_on(LED4_NUM);
  else                         
    led_turn_off(LED4_NUM);
}

/*********************************************************************************************
* 名称：led_app
* 功能：cycle 外部循环周期
* 参数：无
* 返回：无
* 修改：
* 注释：无
*********************************************************************************************/
void led_app(u16 cycle)
{
  static u16 time;time++;
  static u8 ledon;
  if(cycle>1000)cycle=1000;
  if(time>=1000/cycle){
    time=0;
    ledon=!ledon;
    if(ledon) led_setState(LED4_NUM,1);
    else led_setState(LED4_NUM,0);
  }
}

/*********************************************************************************************
* 名称：led_app_ex
* 功能：应用层高速循环led应用程序
* 参数：无
* 返回：无
* 修改：
* 注释：无
*********************************************************************************************/
void led_app_ex(u32 cycle)
{
  static u32 time;time++;
  static u8 ledon;
  if(time>=(1000*1000)/cycle){
    time=0;
    ledon=!ledon;
    if(ledon) led_setState(LED4_NUM,1);
    else led_setState(LED4_NUM,0);
  }
}
