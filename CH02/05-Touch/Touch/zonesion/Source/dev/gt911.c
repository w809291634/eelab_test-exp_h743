#include "gt911.h"
#include "iic4.h"
#include "delay.h"
#include "stdio.h"

GT911_point_t GT911Point[GT911_TOUCH_NUM] = {{0}};      // ������λ��Ϣ�洢
unsigned char GT911_pointNum = 0, touchFlag = 0;
void (*GT911_irq_cb)(void);                             //Ӧ�ò�Ļص�����

/*********************************************************************************************
* ���ƣ�GT911_gpio_init
* ���ܣ��������ų�ʼ��
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
static void GT911_gpio_init(void)
{
  GPIO_InitTypeDef gpio_init_struct;

  GT911_RST_GPIO_CLK_ENABLE();                        /* RST����ʱ��ʹ�� */
  GT911_EXTI_GPIO_CLK_ENABLE();                       /* EXTI����ʱ��ʹ�� */

  gpio_init_struct.Pin = GT911_RST_GPIO_PIN;
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;         /* ������� */
  gpio_init_struct.Pull = GPIO_PULLUP;                 /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  /* ���� */
  HAL_GPIO_Init(GT911_RST_GPIO_PORT, &gpio_init_struct); /* SCL */

  gpio_init_struct.Pin = GT911_EXTI_GPIO_PIN;
  HAL_GPIO_Init(GT911_EXTI_GPIO_PORT, &gpio_init_struct); /* SDA */
}

/*********************************************************************************************
* ���ƣ�GT911_exti_init
* ���ܣ�����оƬ�жϳ�ʼ��
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
static void GT911_exti_init(void)
{
  GPIO_InitTypeDef gpio_init_struct;                          /* GPIO���ò����洢���� */
  // GPIO ��ʼ��
  GT911_EXTI_GPIO_CLK_ENABLE(); 
  
  gpio_init_struct.Pin = GT911_EXTI_GPIO_PIN;                 /* ���� */
  gpio_init_struct.Mode = GPIO_MODE_IT_RISING;                /* �����ش��� */
  gpio_init_struct.Pull = GPIO_NOPULL;                        /* û�������� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
  HAL_GPIO_Init(GT911_EXTI_GPIO_PORT, &gpio_init_struct);     /* ��ʼ������ */
  
  // EXTI ��ʼ��
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;                               //ʹ��SYSCFGʱ��
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);                       /* ��ռ0�������ȼ�0 */
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);                               /* ʹ���ж���2 */
}

/*********************************************************************************************
* ���ƣ�EXTI2_IRQHandler
* ���ܣ��ⲿ�жϷ�����
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
void EXTI2_IRQHandler(void)
{
  touchFlag = 1;
  if(GT911_irq_cb != NULL) GT911_irq_cb();                      //��Ӧ�ò�����лص�
  __HAL_GPIO_EXTI_CLEAR_IT(GT911_EXTI_GPIO_PIN);                /* �˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
}

/*********************************************************************************************
* ���ƣ�GT911_writeReg
* ���ܣ�д�Ĵ���
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
static int GT911_writeReg(unsigned short reg, unsigned char *data, unsigned char len)
{
  unsigned char ret = 0;
  I2C4_Start();
  I2C4_WriteByte(GT911_CMD_WR);
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg >> 8);       // Register_H
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg & 0xFF);     // Register_L
  if(I2C4_WaitAck())return -1;
  for(unsigned char i=0; i<len; i++)
  {
    I2C4_WriteByte(data[i]); 
    if(I2C4_WaitAck())return -1;
    ret++;  
  }
  I2C4_Stop();
  return ret;
}

/*********************************************************************************************
* ���ƣ�GT911_readReg
* ���ܣ����Ĵ���
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
static int GT911_readReg(unsigned short reg, unsigned char *data, unsigned char len)
{
  /* �趨���Ĵ����׵�ַ */
  I2C4_Start();	
  I2C4_WriteByte(GT911_CMD_WR);  
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg >> 8);       // Register_H
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg & 0xFF);     // Register_L
  if(I2C4_WaitAck())return -1;
//  I2C4_Stop();

  /* ��ȡ���� */
  I2C4_Start(); 
  I2C4_WriteByte(GT911_CMD_RD); 
  if(I2C4_WaitAck())return -1;
  for(unsigned char i=0; i<len; i++){
    data[i] = I2C4_ReadByte(i == (len - 1) ? 0 : 1);
  } 
  I2C4_Stop();
  return len;
}

/*********************************************************************************************
* ���ƣ�GT911_cfg
* ���ܣ����ú���
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
int GT911_cfg(void)
{
  unsigned char cfgBuf[14] = {0};
  GT911_writeReg(GT911_CTRL_REG, cfgBuf, 1);    // д������
  GT911_readReg(GT911_PID_REG, cfgBuf, 14);     // ��ȡPID
  if(cfgBuf[0] == 0x39 && cfgBuf[1] == 0x31 && cfgBuf[2] == 0x31)
    return 0;
  return -1;
}

/*********************************************************************************************
* ���ƣ�GT911_scan
* ���ܣ�GT911����ɨ�躯��
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
void GT911_scan(void)
{
  if(touchFlag == 0) return ;
  
  unsigned char pointBuf[GT911_TOUCH_NUM * 8] = {0};    // �洢��λ��Ϣ
  unsigned char info = 0;
  
  GT911_readReg(GT911_GSTID_REG, &info, 1);
  GT911_pointNum = info & 0x0F;                         // ��ȡ������
  if(GT911_pointNum > GT911_TOUCH_NUM)
    GT911_pointNum = GT911_TOUCH_NUM;
  
  /* �д����� */
  if(info > 0x80)
  {
    GT911_readReg(GT911_GSTID_REG + 1, pointBuf, GT911_pointNum * 8);
    for(unsigned char i=0; i<GT911_pointNum; i++){
      GT911Point[i].id = pointBuf[i*8];
      GT911Point[i].x = 854 - (pointBuf[4+(i*8)]<<8 | pointBuf[3+(i*8)]);
      GT911Point[i].y = (pointBuf[2+(i*8)]<<8 | pointBuf[1+(i*8)]);
      GT911Point[i].size = (pointBuf[6+(i*8)]<<8 | pointBuf[5+(i*8)]);
    }
  }
  /* û�д�����ʱΪ���ֵ */
  else if(info == 0x80)
  {
    for(unsigned char i=0; i<GT911_TOUCH_NUM; i++){
      GT911Point[i].x = 0xFFFF;
      GT911Point[i].y = 0xFFFF;
      GT911Point[i].size = 0;
    }
    GT911_pointNum = 0;
  }
  
  info = 0;
  GT911_writeReg(GT911_GSTID_REG, &info, 1);
  touchFlag = 0;
}

/*********************************************************************************************
* ���ƣ�GT911_init
* ���ܣ�GT911���س�ʼ������
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*********************************************************************************************/
int GT911_init(void)
{
  GT911_gpio_init();
  I2C4_IO_Init();
  delay_ms(10);     // �ȴ�
  
  // �趨��ַΪ 0x28/0x29
  GT911_RST_L;
  GT911_EXTI_H;
  delay_ms(1);      // ����10us
  GT911_RST_H;
  delay_ms(6);      // ����5ms
  
  GT911_exti_init();// EXTI תΪ����̬
  delay_ms(50);     // ��ʱһ��ʱ���ٷ���������Ϣ
  return GT911_cfg();
}
