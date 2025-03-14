#include "timer.h"
#include "led.h"
#include "usart.h"
#include "dcmi.h"

/* ��ʱ�����þ�� ���� */
TIM_HandleTypeDef g_timx_handle;                          /* ��ʱ��x��� */

/*********************************************************************************************
* ����:TIMX_Int_Init
* ����:ͨ�ö�ʱ��TIMX��ʱ�жϳ�ʼ������
* ����:arr: �Զ���װֵ
*        psc: Ԥ��Ƶϵ��
* ����:��
* �޸�:��
* ע��:��ʱ����ʱ������APB1 �� APB2, ��D2PPRE1��D2PPRE2����2��Ƶ��ʱ��
*        ͨ�ö�ʱ����ʱ��ΪAPB1 �� APB2ʱ�ӵ�2��, ��APB1��APB2��Ϊ100M,
*        ���Զ�ʱ��ʱ�� = 200Mhz
*        ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
*        Ft=��ʱ������Ƶ��,��λ:Mhz
*********************************************************************************************/    
void TIMX_Int_Init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();                             /* ʹ��TIMxʱ�� */

    g_timx_handle.Instance = GTIM_TIMX_INT;                 /* ͨ�ö�ʱ��x */
    g_timx_handle.Init.Prescaler = psc;                     /* Ԥ��Ƶϵ�� */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;    /* ��������ģʽ */
    g_timx_handle.Init.Period = arr;                        /* �Զ�װ��ֵ */
    HAL_TIM_Base_Init(&g_timx_handle);

    HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 2, 0);         /* �����ж����ȼ�����ռ���ȼ�2�������ȼ�1 */
    HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);                 /* ����ITMx�ж� */

    HAL_TIM_Base_Start_IT(&g_timx_handle);                  /* ʹ�ܶ�ʱ��x�Ͷ�ʱ��x�����ж� */
}

//��ʱ��3�жϷ�����
/*********************************************************************************************
* ����:GTIM_TIMX_INT_IRQHandler
* ����:��ʱ���жϷ�����
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/    
void GTIM_TIMX_INT_IRQHandler(void)
{
  /* ���´���û��ʹ�ö�ʱ��HAL�⹲�ô���������������ֱ��ͨ���ж��жϱ�־λ�ķ�ʽ */
  if (__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET)
  {
    ov_frame=ov_frame_cnt;
    ov_frame_cnt=0;
    LED4_TOGGLE();                                      //LEDָʾӦ��
    //printf("frame:%d\r\n",ov_frame);                  //��ӡ֡��
    __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);  /* �����ʱ������жϱ�־λ */
  }
}
