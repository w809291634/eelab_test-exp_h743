#include "dcmotor.h"
#include "delay.h"

static vs32 encoder_count;
vu32 dcmotor_dir;
double dcmotor_speed;
static TIM_HandleTypeDef g_timx_handle;                         // ��ʱ���������
#define REDUCTION_RATIO       150                               // ������ٱ�
#define PULSE_PERIOD          6                                 // ʹ�õ������������������һȦ��Լ6����
/*********************************************************************************************
* ����:dcmotor_init()
* ����:ֱ�������ʼ������
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void dcmotor_init(void)
{
  GPIO_InitTypeDef gpio_init_struct={0};
  DCMOTOR_GPIO1_CLK_ENABLE();                                   /* ʱ��ʹ�� */
  DCMOTOR_GPIO2_CLK_ENABLE();
  
  gpio_init_struct.Pin = DCMOTOR_GPIO1_PIN;                     /* ���� */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                  /* ������� */
  gpio_init_struct.Pull = GPIO_NOPULL;                          /* �������� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;           /* ���� */
  HAL_GPIO_Init(DCMOTOR_GPIO1_PORT, &gpio_init_struct);         /* ��ʼ������ */

  gpio_init_struct.Pin = DCMOTOR_GPIO2_PIN ;
  HAL_GPIO_Init(DCMOTOR_GPIO2_PORT, &gpio_init_struct);         /* ��ʼ������ */
  
  DCMOTOR_STOP;
  encoder_init(1000,20000-1);                                   //APB1����200mhz��0.1ms�������ڣ�0.1*1000=0.1s���
}

/*********************************************************************************************
* ����:encoder_init()
* ����:ֱ�������������ʼ������ ʹ��PB1��ȡ�������������ֵ,��ʱ����Ϊһ���ٶ�ʱ��
* ����:arr ��ʱ�� arr�Ĵ���ֵ psr Ԥ��Ƶֵ
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void encoder_init(u32 arr,u32 psc)
{
  GPIO_InitTypeDef gpio_init_struct;
  // ��ʱ����ʼ��
  GTIM_TIMX_INT_CLK_ENABLE();                                   /* ʹ��TIMxʱ�� */

  g_timx_handle.Instance = GTIM_TIMX_INT;                       /* ͨ�ö�ʱ��x */
  g_timx_handle.Init.Prescaler = psc;                           /* Ԥ��Ƶϵ�� */
  g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;          /* ��������ģʽ */
  g_timx_handle.Init.Period = arr;                              /* �Զ�װ��ֵ */
  HAL_TIM_Base_Init(&g_timx_handle);

  HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 1, 0);               /* �����ж����ȼ�����ռ���ȼ�1�������ȼ�0 */
  HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);                       /* ����ITMx�ж� */

  HAL_TIM_Base_Start_IT(&g_timx_handle);                        /* ʹ�ܶ�ʱ��x�Ͷ�ʱ��x�����ж� */

  // EXTI ���� ��ʼ��
  DCMOTOR_ENCODER_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;                               /* ʹ��SYSCFGʱ�� */

  gpio_init_struct.Pin = DCMOTOR_ENCODER_PIN;                   /* ���� */
  gpio_init_struct.Mode = GPIO_MODE_IT_RISING;                  /* �����ش��� */
  gpio_init_struct.Pull = GPIO_PULLDOWN;                        /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                /* ���� */
  HAL_GPIO_Init(DCMOTOR_ENCODER_PORT, &gpio_init_struct);       /* ��ʼ������ */
  
  // EXTI �жϳ�ʼ��
  HAL_NVIC_SetPriority(DCMOTOR_ENCODER_IRQn, 0, 0);             /* ��ռ0�������ȼ�0 */
  HAL_NVIC_EnableIRQ(DCMOTOR_ENCODER_IRQn);                     /* ʹ���ж��� */
}

/*********************************************************************************************
* ����:DCMOTOR_ENCODER_IRQHandler()
* ����:�������жϷ������
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void DCMOTOR_ENCODER_IRQHandler(void)
{
  static char last_dir;
  if(last_dir!=dcmotor_dir){
    last_dir=dcmotor_dir;
    encoder_count=0;
  }
  if(dcmotor_dir) encoder_count++;
  else encoder_count--;
  
  __HAL_GPIO_EXTI_CLEAR_IT(DCMOTOR_ENCODER_PIN);                // �����жϱ�־λ
  delay_us(1);
  HAL_NVIC_ClearPendingIRQ(DCMOTOR_ENCODER_IRQn);               // �������ڹ�����жϺţ�������δ���
}

/*********************************************************************************************
* ����:GTIM_TIMX_INT_IRQHandler()
* ����:��ʱ���жϷ�����  
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void GTIM_TIMX_INT_IRQHandler(void)
{
  double count;
  /* ���´���û��ʹ�ö�ʱ��HAL�⹲�ô���������������ֱ��ͨ���ж��жϱ�־λ�ķ�ʽ */
  if (__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET)
  {
    count = encoder_count*1000/100;                         //��ʱ��100ms��������ﵥλ ����/s
    //count = encoder_count*60*1000/100;                    //����/min
    encoder_count =0;
    dcmotor_speed = count/REDUCTION_RATIO/PULSE_PERIOD;     //�����������ʵ��ת�� r/s
    __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);      //�����ʱ������жϱ�־λ
  }
}
