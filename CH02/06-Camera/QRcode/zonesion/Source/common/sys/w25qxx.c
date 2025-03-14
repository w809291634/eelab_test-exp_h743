#include "w25qxx.h" 
#include "spi.h"
#include "delay.h"	   
#include "extend.h"
#include "font_config.h"

/* ���� */
#define W25QXX_SPI_CS_GPIO_PORT             GPIOA
#define W25QXX_SPI_CS_PIN                   GPIO_PIN_15
#define W25QXX_SPI_CS_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)

/* LED�˿ڶ��� */
#define W25QXX_CS(x)    do{ x ? \
                          HAL_GPIO_WritePin(W25QXX_SPI_CS_GPIO_PORT, W25QXX_SPI_CS_PIN, GPIO_PIN_SET) : \
                          HAL_GPIO_WritePin(W25QXX_SPI_CS_GPIO_PORT, W25QXX_SPI_CS_PIN, GPIO_PIN_RESET); \
                        }while(0)    

u16 W25QXX_TYPE=W25Q64;	    //Ĭ����W25Q64
u32 W25QXX_SIZE;

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q64
//����Ϊ8M�ֽ�,����128��Block,2048��Sector 
													 
/*********************************************************************************************
* ����:W25QXX_Init
* ����:��ʼ��SPI FLASH��IO��
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void W25QXX_Init(void)
{ 
  GPIO_InitTypeDef gpio_init_struct={0};
  W25QXX_SPI_CS_CLK_ENABLE();                             /* ʱ��ʹ�� */
  
  gpio_init_struct.Pin = W25QXX_SPI_CS_PIN;               /* ���� */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
  gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* ���� */
  HAL_GPIO_Init(W25QXX_SPI_CS_GPIO_PORT, &gpio_init_struct);       /* ��ʼ�� */
  
  W25QXX_CS(1);             //SPI FLASH��ѡ��
  SPI1_Init();              //��ʼ��SPI
  //SPI1_SetSpeed(SPI_BaudRatePrescaler_4);		//����Ϊ21Mʱ��
  W25QXX_TYPE=W25QXX_ReadID();      //��ȡFLASH ID.
  debug_info("W25QXX_TYPE:0x%04X\r\n",W25QXX_TYPE);
  if(W25QXX_TYPE==0xEF16){
    W25QXX_SIZE = 8*1024*1024;
  }
  else if(W25QXX_TYPE==0xEF17){
    W25QXX_SIZE = 16*1024*1024;
  }
}  

/*********************************************************************************************
* ����:W25QXX_ReadSR
* ����:��ȡW25QXX��״̬�Ĵ���
* ����:��
* ����:��
* �޸�:��
* ע��:BIT7  6   5   4   3   2   1   0
*        SPR   RV  TB BP2 BP1 BP0 WEL BUSY
*        SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
*        TB,BP2,BP1,BP0:FLASH����д��������
*        WEL:дʹ������
*        BUSY:æ���λ(1,æ;0,����)
*        Ĭ��:0x00
*********************************************************************************************/
u8 W25QXX_ReadSR(void)   
{  
  u8 byte=0;   
  W25QXX_CS(0);                            //ʹ������   
  SPI1_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
  byte=SPI1_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
  W25QXX_CS(1);                            //ȡ��Ƭѡ     
  return byte;   
} 

/*********************************************************************************************
* ����:W25QXX_Write_SR
* ����:дW25QXX״̬�Ĵ���
* ����:��
* ����:��
* �޸�:��
* ע��:ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
*********************************************************************************************/
void W25QXX_Write_SR(u8 sr)   
{   
  W25QXX_CS(0);                            //ʹ������   
  SPI1_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
  SPI1_ReadWriteByte(sr);               //д��һ���ֽ�  
  W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
}   

/*********************************************************************************************
* ����:W25QXX_Write_Enable
* ����:W25QXXдʹ��	
* ����:��
* ����:��
* �޸�:��
* ע��:��WEL��λ   
*********************************************************************************************/
void W25QXX_Write_Enable(void)   
{
  W25QXX_CS(0);                            //ʹ������   
  SPI1_ReadWriteByte(W25X_WriteEnable);      //����дʹ��  
  W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
} 

/*********************************************************************************************
* ����:W25QXX_Write_Disable
* ����:W25QXXд��ֹ	
* ����:��
* ����:��
* �޸�:��
* ע��:��WEL����  
*********************************************************************************************/
void W25QXX_Write_Disable(void)   
{  
  W25QXX_CS(0);                            //ʹ������   
  SPI1_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
  W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
} 		

/*********************************************************************************************
* ����:W25QXX_ReadID
* ����:��ȡоƬID
* ����:��
* ����:����ֵ����:				   
*        0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
*        0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
*        0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
*        0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
*        0XEF17,��ʾоƬ�ͺ�ΪW25Q128 
* �޸�:��
* ע��:��
*********************************************************************************************/	  
u16 W25QXX_ReadID(void)
{
  u16 Temp = 0;	  
  W25QXX_CS(0);				    
  SPI1_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
  SPI1_ReadWriteByte(0x00); 	    
  SPI1_ReadWriteByte(0x00); 	    
  SPI1_ReadWriteByte(0x00); 	 			   
  Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
  Temp|=SPI1_ReadWriteByte(0xFF);	 
  W25QXX_CS(1);				    
  return Temp;
}   		    

/*********************************************************************************************
* ����:W25QXX_Read
* ����:��ȡSPI FLASH  
* ����:pBuffer:���ݴ洢��
*        ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
*        NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
* ����:��
* �޸�:��
* ע��:��ָ����ַ��ʼ��ȡָ�����ȵ�����
*********************************************************************************************/
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
  u16 i;   										    
  W25QXX_CS(0);                            //ʹ������   
  SPI1_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
  SPI1_ReadWriteByte((u8)((ReadAddr)>>16));  //����24bit��ַ    
  SPI1_ReadWriteByte((u8)((ReadAddr)>>8));   
  SPI1_ReadWriteByte((u8)ReadAddr);   
  for(i=0;i<NumByteToRead;i++)
  { 
    pBuffer[i]=SPI1_ReadWriteByte(0XFF);   //ѭ������  
  }
  W25QXX_CS(1);  				    	      
}  

/*********************************************************************************************
* ����:W25QXX_Write_Page
* ����:SPI��һҳ(0~65535)��д������256���ֽڵ�����
* ����:pBuffer:���ݴ洢��
*        WriteAddr:��ʼд��ĵ�ַ(24bit)
*        NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
* ����:��
* �޸�:��
* ע��:��ָ����ַ��ʼд�����256�ֽڵ�����
*********************************************************************************************/	 
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
  u16 i;  
  W25QXX_Write_Enable();                  //SET WEL 
  W25QXX_CS(0);                            //ʹ������   
  SPI1_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
  SPI1_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
  SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
  SPI1_ReadWriteByte((u8)WriteAddr);   
  for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);//ѭ��д��  
  W25QXX_CS(1);                            //ȡ��Ƭѡ 
  W25QXX_Wait_Busy();					   //�ȴ�д�����
} 

/*********************************************************************************************
* ����:W25QXX_Write_NoCheck
* ����:�޼���дSPI FLASH 
* ����:pBuffer:���ݴ洢��
*        WriteAddr:��ʼд��ĵ�ַ(24bit)
*        NumByteToWrite:Ҫд����ֽ���(���65535)
* ����:��
* �޸�:��
* ע��:����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
*        �����Զ���ҳ���� 
*        ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
*        CHECK OK
*********************************************************************************************/
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
	 
/*********************************************************************************************
* ����:W25QXX_Write
* ����:дSPI FLASH  
* ����:pBuffer:���ݴ洢��
*        WriteAddr:��ʼд��ĵ�ַ(24bit)	
*        NumByteToWrite:Ҫд����ֽ���(���65535)   
* ����:��
* �޸�:��
* ע��:��ָ����ַ��ʼд��ָ�����ȵ�����
*        �ú�������������!
*        ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
*        CHECK OK
*********************************************************************************************/
u8 W25QXX_BUFFER[4096];	
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * W25QXX_BUF;	  
  W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
      secpos++;//������ַ��1
      secoff=0;//ƫ��λ��Ϊ0 	 

      pBuffer+=secremain;  //ָ��ƫ��
      WriteAddr+=secremain;//д��ַƫ��	   
      NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}

/*********************************************************************************************
* ����:W25QXX_Erase_Chip
* ����:��������оƬ	
* ����:��
* ����:��
* �޸�:��
* ע��:�ȴ�ʱ�䳬��...
*********************************************************************************************/
void W25QXX_Erase_Chip(void)   
{                                   
  W25QXX_Write_Enable();                  //SET WEL 
  W25QXX_Wait_Busy();   
  W25QXX_CS(0);                            //ʹ������   
  SPI1_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
  W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
  W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
}   

/*********************************************************************************************
* ����:W25QXX_Erase_Sector
* ����:����һ������
* ����:Dst_Addr:������ַ ����ʵ����������
* ����:��
* �޸�:��
* ע��:����һ��ɽ��������ʱ��:150ms
*********************************************************************************************/
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
  //����falsh�������,������   
//  printf("fe:%x\r\n",Dst_Addr);	  
  Dst_Addr*=4096;
  W25QXX_Write_Enable();                  //SET WEL 	 
  W25QXX_Wait_Busy();   
  W25QXX_CS(0);                            //ʹ������   
  SPI1_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
  SPI1_ReadWriteByte((u8)Dst_Addr);  
  W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
  W25QXX_Wait_Busy();   				   //�ȴ��������
}  

/*********************************************************************************************
* ����:W25QXX_Wait_Busy
* ����:�ȴ�����
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
}  

/*********************************************************************************************
* ����:W25QXX_PowerDown
* ����:�������ģʽ
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void W25QXX_PowerDown(void)   
{ 
  W25QXX_CS(0);                            //ʹ������   
  SPI1_ReadWriteByte(W25X_PowerDown);        //���͵�������  
  W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
  delay_us(3);                               //�ȴ�TPD  
}   

/*********************************************************************************************
* ����:W25QXX_WAKEUP
* ����:����
* ����:��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/
void W25QXX_WAKEUP(void)   
{  
  W25QXX_CS(0);                            //ʹ������   
  SPI1_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
  W25QXX_CS(1);                            //ȡ��Ƭѡ     	      
  delay_us(3);                               //�ȴ�TRES1
}   
