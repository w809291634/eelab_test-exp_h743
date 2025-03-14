#include "key.h"
#include "delay.h"

// ��ʼ��
void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;                          /* GPIO���ò����洢���� */
    KEY1_GPIO_CLK_ENABLE();                                     /* KEY1ʱ��ʹ�� */
    KEY2_GPIO_CLK_ENABLE();                                     /* KEY2ʱ��ʹ�� */
    KEY3_GPIO_CLK_ENABLE();                                     /* KEY3ʱ��ʹ�� */
    KEY4_GPIO_CLK_ENABLE();                                     /* KEY4ʱ��ʹ�� */

    gpio_init_struct.Pin = KEY1_GPIO_PIN;                       /* KEY1���� */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(KEY1_GPIO_PORT, &gpio_init_struct);           /* KEY1����ģʽ����,�������� */

    gpio_init_struct.Pin = KEY2_GPIO_PIN;                       /* KEY2���� */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(KEY2_GPIO_PORT, &gpio_init_struct);           /* KEY2����ģʽ����,�������� */

    gpio_init_struct.Pin = KEY3_GPIO_PIN;                       /* KEY3���� */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(KEY3_GPIO_PORT, &gpio_init_struct);           /* KEY3����ģʽ����,�������� */

    gpio_init_struct.Pin = KEY4_GPIO_PIN;                       /* KEY4���� */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(KEY4_GPIO_PORT, &gpio_init_struct);           /* KEY4����ģʽ����,�������� */
}

/*********************************************************************************************
* ���ƣ�get_key_status
* ���ܣ������ܽų�ʼ��
* ��������
* ���أ�key_status
* �޸ģ�
*********************************************************************************************/
char get_key_status(void)
{
  char key_status = 0;
  if(HAL_GPIO_ReadPin(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == 0)       //�ж�PB12���ŵ�ƽ״̬
    key_status |= K1_PRES;                                      //�͵�ƽkey_status bit0λ��1
  if(HAL_GPIO_ReadPin(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == 0)       //�ж�PB13���ŵ�ƽ״̬
    key_status |= K2_PRES;                                      //�͵�ƽkey_status bit1λ��1
  if(HAL_GPIO_ReadPin(KEY3_GPIO_PORT,KEY3_GPIO_PIN) == 0)       //�ж�PB14���ŵ�ƽ״̬
    key_status |= K3_PRES;                                      //�͵�ƽkey_status bit2λ��1
  if(HAL_GPIO_ReadPin(KEY4_GPIO_PORT,KEY4_GPIO_PIN) == 0)       //�ж�PB15���ŵ�ƽ״̬
    key_status |= K4_PRES;                                      //�͵�ƽkey_status bit3λ��1
  return key_status;
}


/*********************************************************************************************
* ���ƣ�KEY_Scan
* ���ܣ�����������
* ������mode: 0,��֧��������; 1,֧��������;
* ���أ���
* �޸ģ�
* ע�ͣ�ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
*********************************************************************************************/

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_count(1000);//ȥ���� 
		key_up=0;
		if(KEY1==0)return K1_PRES;
		else if(KEY2==0)return K2_PRES;
		else if(KEY3==0)return K3_PRES;
		else if(KEY4==0)return K4_PRES;
	}else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	    
 	return 0;// �ް�������
}
