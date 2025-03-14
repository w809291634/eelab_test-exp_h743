/*********************************************************************************************
* �ļ���timer.c
* ���ߣ�chennian 2017.11.2
* ˵����
* ���ܣ���ʱ����������
* �޸ģ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "timer.h"

/* ��ʱ�����þ�� ���� */
static TIM_HandleTypeDef g_timx_handle;                          /* ��ʱ��x��� */

/*********************************************************************************************
* ���ƣ�ͨ�ö�ʱ��TIMX��ʱ�жϳ�ʼ������
* ���ܣ���ʼ��TIM3
* ������period���Զ���װֵ�� prescaler��ʱ��Ԥ��Ƶ��
* ���أ���
* �޸ģ�
* ע�ͣ���ʱ����ʱ������APB1 �� APB2, ��D2PPRE1��D2PPRE2����2��Ƶ��ʱ��
 *              ͨ�ö�ʱ����ʱ��ΪAPB1 �� APB2ʱ�ӵ�2��, ��APB1��APB2��Ϊ100M,
 *              ���Զ�ʱ��ʱ�� = 200Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
*********************************************************************************************/
void timer_init(void)
{
    GTIM_TIMX_INT_CLK_ENABLE();                             /* ʹ��TIMxʱ�� */
    //�����жϴ���ʱ��Ϊ10MS
    g_timx_handle.Instance = GTIM_TIMX_INT;                 /* ͨ�ö�ʱ��x */
    g_timx_handle.Init.Period = 1000 - 1;                   /* �Զ�װ��ֵ */
    g_timx_handle.Init.Prescaler = 2000 - 1;                /* Ԥ��Ƶϵ�� */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;    /* ��������ģʽ */
    HAL_TIM_Base_Init(&g_timx_handle);

    HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 2, 0);         /* �����ж����ȼ�����ռ���ȼ�2�������ȼ�0 */
    HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);                 /* ����ITMx�ж� */

    HAL_TIM_Base_Start_IT(&g_timx_handle);                  /* ʹ�ܶ�ʱ��x�Ͷ�ʱ��x�����ж� */
}


/*********************************************************************************************
* ���ƣ�GTIM_TIMX_INT_IRQHandler
* ���ܣ�ͨ�ö�ʱ���жϴ�����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
extern uint8_t rgb_twinkle;
extern uint8_t tim_second;
extern uint8_t mode_twinkle;
uint8_t count_sec = 0;
uint8_t count_rgb = 0;
uint8_t count_mode = 0;
uint32_t ticks = 0;                                             // ϵͳ������ʱ�����ֵ

void GTIM_TIMX_INT_IRQHandler(void)
{
  if (__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET) {   //����жϱ�־������
    ticks++;
    
    if((count_rgb % 20) == 19){
      rgb_twinkle = 1;
      count_rgb = 0; 
    }else count_rgb ++; 
    
    if((count_sec % 50) == 49){
      count_sec = 0;
      tim_second = 1;
    }else  count_sec ++;
    
    if((count_mode % 40) == 39){
      mode_twinkle = 1;
      count_mode = 0;
    }else count_mode ++;
    
    __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);                  //����жϱ�־
  }
}

/*********************************************************************************************
* ���ƣ�xtime()
* ���ܣ�ϵͳ������ʱ���ȡ
* ��������
* ���أ�ϵͳ������ʱ�䣬��λΪms��ע��������Ҫ������tim��ʱ�����
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
uint32_t xtime(void){
  return (uint32_t)(ticks*10);
}