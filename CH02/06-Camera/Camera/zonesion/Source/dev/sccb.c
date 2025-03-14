#include "extend.h"
#include "sccb.h"
#include "delay.h"

/*********************************************************************************************
* ���ƣ�SCCB_Init
* ���ܣ���ʼ��SCCB�ӿ� 
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*******************************************************************************************/
void SCCB_Init(void)
{
  GPIO_InitTypeDef gpio_init_struct;

  SCCB_SCL_GPIO_CLK_ENABLE();                           /* SCL����ʱ��ʹ�� */
  SCCB_SDA_GPIO_CLK_ENABLE();                           /* SDA����ʱ��ʹ�� */

  gpio_init_struct.Pin = SCCB_SCL_GPIO_PIN;
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;         /* ������� */
  gpio_init_struct.Pull = GPIO_PULLUP;                 /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  /* ���� */
  HAL_GPIO_Init(SCCB_SCL_GPIO_PORT, &gpio_init_struct); /* SCL */

  gpio_init_struct.Pin = SCCB_SDA_GPIO_PIN;
  HAL_GPIO_Init(SCCB_SDA_GPIO_PORT, &gpio_init_struct); /* SDA */
  
  SCCB_SDA_OUT;
  SCCB_SCL_H;
  SCCB_SDA_H;
}

/*********************************************************************************************
* ���ƣ�SCCB_Start
* ���ܣ�SCCB��ʼ�ź�
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���ʱ��Ϊ�ߵ�ʱ��,�����ߵĸߵ���,ΪSCCB��ʼ�ź�
*           �ڼ���״̬��,SDA��SCL��Ϊ�͵�ƽ
*******************************************************************************************/
void SCCB_Start(void)
{
  SCCB_SDA_H;     //�����߸ߵ�ƽ	   
  SCCB_SCL_H;	    //��ʱ���߸ߵ�ʱ���������ɸ�����
  delay_us(2);
  SCCB_SDA_L;
  delay_us(2);
  SCCB_SCL_L;	    //�����߻ָ��͵�ƽ��������������Ҫ
}

/*********************************************************************************************
* ���ƣ�SCCB_Stop
* ���ܣ�SCCBֹͣ�ź�
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���ʱ��Ϊ�ߵ�ʱ��,�����ߵĵ͵���,ΪSCCBֹͣ�ź�
*           ����״����,SDA,SCL��Ϊ�ߵ�ƽ
*******************************************************************************************/
void SCCB_Stop(void)
{
  SCCB_SDA_L;
  delay_us(2);
  SCCB_SCL_H;
  delay_us(2); 
  SCCB_SDA_H;
  delay_us(2);
}

/*********************************************************************************************
* ���ƣ�SCCB_No_Ack
* ���ܣ�����NA�ź�
* ��������
* ���أ���
* �޸ģ���
* ע�ͣ���
*******************************************************************************************/
void SCCB_No_Ack(void)
{
  delay_us(2);
  SCCB_SDA_H;
  SCCB_SCL_H;
  delay_us(2);
  SCCB_SCL_L;
  delay_us(2);
  SCCB_SDA_L;
  delay_us(2);
}

/*********************************************************************************************
* ���ƣ�SCCB_WR_Byte
* ���ܣ�SCCB,д��һ���ֽ�
* ��������
* ���أ�����ֵ:0,�ɹ�;1,ʧ��. 
* �޸ģ���
* ע�ͣ���
*******************************************************************************************/
u8 SCCB_WR_Byte(u8 dat)
{
  u8 j,res;	 
  for(j=0;j<8;j++)      //ѭ��8�η�������
  {
    if(dat&0x80)SCCB_SDA_H;
    else SCCB_SDA_L;
    dat<<=1;
    delay_us(2);
    SCCB_SCL_H;
    delay_us(2);
    SCCB_SCL_L;
  }
  SCCB_SDA_IN;          //����SDAΪ���� 
  delay_us(2);
  SCCB_SCL_H;           //���յھ�λ,���ж��Ƿ��ͳɹ�
  delay_us(2);
  if(SCCB_SDA_R)res=1;  //SDA=1����ʧ�ܣ�����1
  else res=0;           //SDA=0���ͳɹ�������0
  SCCB_SCL_L;
  SCCB_SDA_OUT;         //����SDAΪ���    
  return res;  
}

/*********************************************************************************************
* ���ƣ�SCCB_RD_Byte
* ���ܣ�SCCB ��ȡһ���ֽ�
* ��������
* ���أ�����ֵ:����������
* �޸ģ���
* ע�ͣ���SCL��������,��������
*******************************************************************************************/
u8 SCCB_RD_Byte(void)
{
  u8 temp=0,j;    
  SCCB_SDA_IN;		//����SDAΪ����  
  for(j=8;j>0;j--) 	//ѭ��8�ν�������
  {		     	  
    delay_us(2);
    SCCB_SCL_H;
    temp=temp<<1;
    if(SCCB_SDA_R)temp++;   
    delay_us(2);
    SCCB_SCL_L;
  }	
  SCCB_SDA_OUT;		//����SDAΪ���    
  return temp;
}

/*********************************************************************************************
* ���ƣ�SCCB_WR_Reg
* ���ܣ�д�Ĵ���
* ��������
* ���أ�����ֵ:0,�ɹ�;1,ʧ��.
* �޸ģ���
* ע�ͣ���
*******************************************************************************************/
u8 SCCB_WR_Reg(u8 reg,u8 data)
{
  u8 res=0;
  SCCB_Start();   //����SCCB����
  if(SCCB_WR_Byte(SCCB_ID))res=1;	//д����ID	  
  delay_us(2);
  if(SCCB_WR_Byte(reg))res=1;		//д�Ĵ�����ַ	  
  delay_us(2);
  if(SCCB_WR_Byte(data))res=1; 	//д����	 
  SCCB_Stop();
  return res;
}

/*********************************************************************************************
* ���ƣ�SCCB_RD_Reg
* ���ܣ����Ĵ���
* ��������
* ���أ�����ֵ:�����ļĴ���ֵ
* �޸ģ���
* ע�ͣ���
*******************************************************************************************/
u8 SCCB_RD_Reg(u8 reg)
{
  u8 val=0;
  SCCB_Start(); 				//����SCCB����
  SCCB_WR_Byte(SCCB_ID);		//д����ID	  
  delay_us(2);	 
  SCCB_WR_Byte(reg);			//д�Ĵ�����ַ	  
  delay_us(2);	  
  SCCB_Stop();   
  delay_us(2);	   
  //���üĴ�����ַ�󣬲��Ƕ�
  SCCB_Start();
  SCCB_WR_Byte(SCCB_ID|0X01);	//���Ͷ�����	  
  delay_us(2);
  val=SCCB_RD_Byte();		 	//��ȡ����
  SCCB_No_Ack();
  SCCB_Stop();
  return val;
}
