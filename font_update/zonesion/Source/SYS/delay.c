/*********************************************************************************************
* �ļ�: delay.c
* ���ߣ�zonesion 2016.12.22
* ˵������ʱ��غ���  
* �޸ģ�
* ע�ͣ����õ�contiki����ϵͳʱ�轫SYSTEM_SUPPORT_CONTIKI��1����delay.h�ļ���
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "delay.h"

static uint32_t g_fac_us = 0;       /* us��ʱ������ */

/**
 * @brief     ��ʼ���ӳٺ���
 * @param     sysclk: ϵͳʱ��Ƶ��, ��CPUƵ��(HCLK), ����ϵͳ��Ƶ, ��λ Mhz
 * @retval    ��
 */  
void delay_init(uint16_t sysclk)
{
    g_fac_us = sysclk;                                  /* �����Ƿ�ʹ��OS,g_fac_us����Ҫʹ�� */
}

/**
 * @brief     ��ʱnus
 * @note      �����Ƿ�ʹ��OS, ������ʱ��ժȡ������us��ʱ
 * @param     nus: Ҫ��ʱ��us��
 * @note      nusȡֵ��Χ: 0 ~ (2^32 / fac_us) (fac_usһ�����ϵͳ��Ƶ, �����������)
 * @retval    ��
 */ 
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;        /* LOAD��ֵ */
    ticks = nus * g_fac_us;                 /* ��Ҫ�Ľ����� */

    told = SysTick->VAL;                    /* �ս���ʱ�ļ�����ֵ */
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;        /* ����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ����� */
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) 
            {
                break;                      /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
            }
        }
    }
} 

/**
 * @brief     ��ʱnms
 * @param     nms: Ҫ��ʱ��ms�� (0< nms <= (2^32 / fac_us / 1000))(fac_usһ�����ϵͳ��Ƶ, �����������)
 * @retval    ��
 */
void delay_ms(uint16_t nms)
{
    delay_us((uint32_t)(nms * 1000));                   /* ��ͨ��ʽ��ʱ */
}

/**
 * @brief       HAL���ڲ������õ�����ʱ
 * @note        HAL�����ʱĬ����Systick���������û�п�Systick���жϻᵼ�µ��������ʱ���޷��˳�
 * @param       Delay : Ҫ��ʱ�ĺ�����
 * @retval      ��
 */
void HAL_Delay(uint32_t Delay)
{
     delay_ms(Delay);
}
