/*********************************************************************************************
* 文件：iwdg.c
* 作者：chennian 2017.11.2
* 说明：看门狗程序代码    
* 功能：
* 实验现象：
*********************************************************************************************/
#include  "iwdg.h"

IWDG_HandleTypeDef g_iwdg_handle;                       /* 独立看门狗句柄 */
/*********************************************************************************************
* 名称：wdg_init
* 功能：看门狗初始化
* 参数：prer  : IWDG_PRESCALER_4~IWDG_PRESCALER_256,对应4~256分频  rlr   : 自动重装载值,0~0XFFF. 
* 返回：无
* 修改：
*********************************************************************************************/
void wdg_init(char prer,int rlr)
{
  g_iwdg_handle.Instance = IWDG1;
  g_iwdg_handle.Init.Prescaler = prer;                /* 设置IWDG分频系数 */
  g_iwdg_handle.Init.Reload = rlr;                    /* 从加载寄存器 IWDG->RLR 重装载值 */
  g_iwdg_handle.Init.Window = IWDG_WINDOW_DISABLE;    /* 关闭窗口功能 */
  HAL_IWDG_Init(&g_iwdg_handle);                      /* 初始化IWDG并使能 */
}

/*********************************************************************************************
* 名称：wdg_feed
* 功能：喂狗程序
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
void wdg_feed(void)
{
  HAL_IWDG_Refresh(&g_iwdg_handle);                   /* 喂狗 */
}