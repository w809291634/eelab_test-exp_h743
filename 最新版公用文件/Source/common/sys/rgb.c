/*********************************************************************************************
* 文件: led.c
* 作者：liutong 2015.8.19
* 说明：
* 修改：
* 注释：
*********************************************************************************************/
#include "rgb.h"

/*********************************************************************************************
* 名称：rgb_init
* 功能：初始化RGB对应的GPIO
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void rgb_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  LEDR_GPIO_CLK_ENABLE();                                 /* LEDR时钟使能 */
  LEDG_GPIO_CLK_ENABLE();                                 /* LEDG时钟使能 */
  LEDB_GPIO_CLK_ENABLE();                                 /* LEDB时钟使能 */
  
  gpio_init_struct.Pin = LEDR_GPIO_PIN;                   /* LED引脚 */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;        /* 中速 */
  HAL_GPIO_Init(LEDR_GPIO_PORT, &gpio_init_struct);       /* 初始化LED引脚 */

  gpio_init_struct.Pin = LEDG_GPIO_PIN ;
  HAL_GPIO_Init(LEDG_GPIO_PORT, &gpio_init_struct);       /* 初始化LED引脚 */
  
  gpio_init_struct.Pin = LEDB_GPIO_PIN ;
  HAL_GPIO_Init(LEDB_GPIO_PORT, &gpio_init_struct);       /* 初始化LED引脚 */
  
  turn_off(LEDR);
  turn_off(LEDG);
  turn_off(LEDB);
}

/*********************************************************************************************
* 名称：turn_off
* 功能：置引脚高电平，关闭灯
* 参数：rgb
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void turn_off(unsigned char rgb){
  if(rgb & LEDR)
    HAL_GPIO_WritePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN, GPIO_PIN_SET);        //置引脚高电平,关闭
  if(rgb & LEDG)
    HAL_GPIO_WritePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN, GPIO_PIN_SET);
  if(rgb & LEDB)
    HAL_GPIO_WritePin(LEDB_GPIO_PORT, LEDB_GPIO_PIN, GPIO_PIN_SET);
}

/*********************************************************************************************
* 名称：turn_on
* 功能：置引脚低电平，打开灯
* 参数：rgb
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void turn_on(unsigned char rgb){
  if(rgb & LEDR)
    HAL_GPIO_WritePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN, GPIO_PIN_RESET);     //置引脚低电平，打开灯
  if(rgb & LEDG)
    HAL_GPIO_WritePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN, GPIO_PIN_RESET);
  if(rgb & LEDB)
    HAL_GPIO_WritePin(LEDB_GPIO_PORT, LEDB_GPIO_PIN, GPIO_PIN_RESET);
}

/*********************************************************************************************
* 名称：get_rgb_status
* 功能：获取灯的状态
* 参数：
* 返回：status，按位分别表示灯的状态
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_rgb_status(void){
  unsigned char status = 0;
  if(HAL_GPIO_ReadPin(LEDR_GPIO_PORT, LEDR_GPIO_PIN))
    status |= LEDR;
  else
    status &= ~LEDR;
  
  if(HAL_GPIO_ReadPin(LEDG_GPIO_PORT, LEDG_GPIO_PIN))
    status |= LEDG; 
  else
    status &= ~LEDG;
  
  if(HAL_GPIO_ReadPin(LEDB_GPIO_PORT, LEDB_GPIO_PIN))
    status |= LEDB;
  else
    status &= ~LEDB;
  return status;                                                //返回status
}

/*********************************************************************************************
* 名称：void rgb_ctrl(uint8_t cfg)
* 功能：RGB控制函数
* 参数：cfg:RGB的控制位，低三位分别控制一盏灯
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void rgb_ctrl(uint8_t cfg)
{
  uint8_t set1, set2, set3;
  set1 = cfg & 0x01;
  set2 = (cfg & 0x02) >> 1;
  set3 = (cfg & 0x04) >> 2;
  
  _LEDR(set1);
  _LEDG(set2);
  _LEDB(set3);
}
