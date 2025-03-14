/*********************************************************************************************
* �ļ���iic.c
* ���ߣ�zonesion
* ˵����iic��������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "iic1.h"
#include "extend.h"
#include "delay.h"
/*********************************************************************************************
* �궨��
*********************************************************************************************/
// SCL--PF1   SDA---PF0
#define I2C_GPIO                GPIOF
#define PIN_SCL                 GPIO_PIN_1
#define PIN_SDA                 GPIO_PIN_0

#define I2C_SCL_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* ʱ��ʹ�� */
#define I2C_SDA_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* ʱ��ʹ�� */

#define SDA_R                   HAL_GPIO_ReadPin(I2C_GPIO,PIN_SDA)
/*********************************************************************************************
* ���ƣ�iic_init()
* ���ܣ�I2C��ʼ������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_init(void)
{
  GPIO_InitTypeDef gpio_init_struct;

  I2C_SCL_GPIO_CLK_ENABLE();                            /* SCL����ʱ��ʹ�� */
  I2C_SDA_GPIO_CLK_ENABLE();                            /* SDA����ʱ��ʹ�� */

  gpio_init_struct.Pin = PIN_SCL;
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;          /* ������� */
  gpio_init_struct.Pull = GPIO_PULLUP;                  /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;   /* ���� */
  HAL_GPIO_Init(I2C_GPIO, &gpio_init_struct);           /* SCL */

  gpio_init_struct.Pin = PIN_SDA;
  HAL_GPIO_Init(I2C_GPIO, &gpio_init_struct);           /* SDA */
}

/*********************************************************************************************
* ���ƣ�sda_out()
* ���ܣ�����SDAΪ���
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sda_out(void)
{
  GPIO_InitTypeDef gpio_init_struct;

  gpio_init_struct.Pin = PIN_SDA;
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;          /* ������� */
  gpio_init_struct.Pull = GPIO_PULLUP;                  /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;   /* ���� */
  HAL_GPIO_Init(I2C_GPIO, &gpio_init_struct);           /* SDA */
}

/*********************************************************************************************
* ���ƣ�sda_in()
* ���ܣ�����SDAΪ����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sda_in(void)
{
  GPIO_InitTypeDef gpio_init_struct;

  gpio_init_struct.Pin = PIN_SDA;
  gpio_init_struct.Mode = GPIO_MODE_INPUT;              /* ����ģʽ */
  gpio_init_struct.Pull = GPIO_PULLUP;                  /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;   /* ���� */
  HAL_GPIO_Init(I2C_GPIO, &gpio_init_struct);           /* SDA */
}

/*********************************************************************************************
* ���ƣ�iic_start()
* ���ܣ�I2C��ʼ�ź�
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_start(void)
{
  sda_out();
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);           //����������
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);           //����ʱ����
  delay_us(5);                                                  //��ʱ
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_RESET);         //����������
  delay_us(5);                                                  //��ʱ
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);         //ǯסI2C���ߣ�׼�����ͻ�������� 
}

/*********************************************************************************************
* ���ƣ�iic_stop()
* ���ܣ�I2Cֹͣ�ź�
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_stop(void)
{
  sda_out();
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_RESET);         //����������
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);           //����ʱ����
  delay_us(5);                                                  //��ʱ5us
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);           //���������ߣ�����I2C���߽����ź�
  delay_us(5);                                                  //��ʱ5us
}

/*********************************************************************************************
* ���ƣ�iic_send_ack()
* ���ܣ�I2C����Ӧ���ź�
* ������ack��Ϊ0������ACKӦ��  Ϊ1��������ACKӦ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_send_ack(char ack)
{
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);         //����ʱ����
  sda_out();
  if(ack)
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);         //������ACKӦ��
  else
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_RESET);       //����ACKӦ��
  delay_us(5);                                                  //��ʱ 
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);           //����ʱ����
  delay_us(5);                                                  //��ʱ
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);         //����ʱ����
}

/*********************************************************************************************
* ���ƣ�iic_recv_ack()
* ���ܣ�I2C����Ӧ��
* ��������
* ���أ�Ӧ���ź�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char iic_recv_ack(void)
{
  unsigned char CY = 0;
  sda_in();
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);           //����������
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);           //����ʱ���� 
  delay_us(5);                                                  //��ʱ
  CY = SDA_R;                                                   //��Ӧ���ź�
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);         //�ͷ����� 
  return CY;
}

/*********************************************************************************************
* ���ƣ�iic_write_byte()
* ���ܣ�I2Cдһ���ֽ����ݣ�����ACK����NACK���Ӹߵ��ͣ����η���
* ������data -- Ҫд������
* ���أ�0���յ�Ӧ�� 1��û���յ�Ӧ��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char iic_write_byte(unsigned char data)
{
  unsigned char i;
  sda_out();
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);         //����ʱ����
  for(i = 0;i < 8;i++){                                         
    if(data & 0x80)                                             //�����߲���
      HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);
    else
      HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_RESET); 
    delay_us(5);                                                //��ʱ5us
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);         //���SDA�ȶ�������SCL���������أ��ӻ���⵽��������ݲ���
    delay_us(5);                                                //��ʱ5us
    data <<= 1;                                                 //��������һλ
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);       //����ʱ����
  } 
  /* ����Ӧ�� */
  sda_in();
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SDA, GPIO_PIN_SET);           //����������
  HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);           //����ʱ����
  delay_us(5);                                                  //��ʱ5us���ȴ��ӻ�Ӧ��
  if(SDA_R)return 1;                                            //SDAΪ�ߣ��յ�NACK
  else{                                                         //SDAΪ�ͣ��յ�ACK
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);       //�ͷ�����
    return 0;
  }
}

/*********************************************************************************************
* ���ƣ�iic_read_byte()
* ���ܣ�I2C��һ���ֽ����ݣ�����ACK����NACK���Ӹߵ��ͣ����η��͡�
* ������ack��Ϊ0������ACKӦ��  Ϊ1��������ACKӦ��Ӧ���ź�Ϊ�͵�ƽʱ���涨Ϊ��ЧӦ��λ
* ���أ�data����ȡ���ֽ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char iic_read_byte(unsigned char ack)
{  
  unsigned char i,data = 0;
  sda_in();                                                     // ����������Ϊ����ģʽ
  for(i=0;i<8;i++ )
  {
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_RESET);       // ����ʱ���ߣ���ʱ������������
    delay_us(5);                                                // �ȴ����ݾ���
    HAL_GPIO_WritePin(I2C_GPIO, PIN_SCL, GPIO_PIN_SET);         // ����ʱ���ߣ������ߴ�ʱ�ȶ�
    data<<=1;
    if(SDA_R)data++;                                            // ��ȡ����
    delay_us(5);                                                // ���ݱ���ʱ��
  }
  iic_send_ack(ack);                                            // ����Ӧ���ź�
  return data;
}

/*********************************************************************************************
* ���ƣ�delay()
* ���ܣ���ʱ
* ������t -- ����ʱ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void delay(unsigned int t)                                      //��ʱ����
{
  unsigned char i;
  while(t--){
    for(i = 0;i < 200;i++);
  }
}