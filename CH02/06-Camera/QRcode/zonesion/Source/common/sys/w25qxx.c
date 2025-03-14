#include "w25qxx.h" 
#include "spi.h"
#include "delay.h"	   
#include "extend.h"
#include "font_config.h"

/* 配置 */
#define W25QXX_SPI_CS_GPIO_PORT             GPIOA
#define W25QXX_SPI_CS_PIN                   GPIO_PIN_15
#define W25QXX_SPI_CS_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)

/* LED端口定义 */
#define W25QXX_CS(x)    do{ x ? \
                          HAL_GPIO_WritePin(W25QXX_SPI_CS_GPIO_PORT, W25QXX_SPI_CS_PIN, GPIO_PIN_SET) : \
                          HAL_GPIO_WritePin(W25QXX_SPI_CS_GPIO_PORT, W25QXX_SPI_CS_PIN, GPIO_PIN_RESET); \
                        }while(0)    

u16 W25QXX_TYPE=W25Q64;	    //默认是W25Q64
u32 W25QXX_SIZE;

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q64
//容量为8M字节,共有128个Block,2048个Sector 
													 
/*********************************************************************************************
* 名称:W25QXX_Init
* 功能:初始化SPI FLASH的IO口
* 参数:无
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void W25QXX_Init(void)
{ 
  GPIO_InitTypeDef gpio_init_struct={0};
  W25QXX_SPI_CS_CLK_ENABLE();                             /* 时钟使能 */
  
  gpio_init_struct.Pin = W25QXX_SPI_CS_PIN;               /* 引脚 */
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
  gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 中速 */
  HAL_GPIO_Init(W25QXX_SPI_CS_GPIO_PORT, &gpio_init_struct);       /* 初始化 */
  
  W25QXX_CS(1);             //SPI FLASH不选中
  SPI1_Init();              //初始化SPI
  //SPI1_SetSpeed(SPI_BaudRatePrescaler_4);		//设置为21M时钟
  W25QXX_TYPE=W25QXX_ReadID();      //读取FLASH ID.
  debug_info("W25QXX_TYPE:0x%04X\r\n",W25QXX_TYPE);
  if(W25QXX_TYPE==0xEF16){
    W25QXX_SIZE = 8*1024*1024;
  }
  else if(W25QXX_TYPE==0xEF17){
    W25QXX_SIZE = 16*1024*1024;
  }
}  

/*********************************************************************************************
* 名称:W25QXX_ReadSR
* 功能:读取W25QXX的状态寄存器
* 参数:无
* 返回:无
* 修改:无
* 注释:BIT7  6   5   4   3   2   1   0
*        SPR   RV  TB BP2 BP1 BP0 WEL BUSY
*        SPR:默认0,状态寄存器保护位,配合WP使用
*        TB,BP2,BP1,BP0:FLASH区域写保护设置
*        WEL:写使能锁定
*        BUSY:忙标记位(1,忙;0,空闲)
*        默认:0x00
*********************************************************************************************/
u8 W25QXX_ReadSR(void)   
{  
  u8 byte=0;   
  W25QXX_CS(0);                            //使能器件   
  SPI1_ReadWriteByte(W25X_ReadStatusReg);    //发送读取状态寄存器命令    
  byte=SPI1_ReadWriteByte(0Xff);             //读取一个字节  
  W25QXX_CS(1);                            //取消片选     
  return byte;   
} 

/*********************************************************************************************
* 名称:W25QXX_Write_SR
* 功能:写W25QXX状态寄存器
* 参数:无
* 返回:无
* 修改:无
* 注释:只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
*********************************************************************************************/
void W25QXX_Write_SR(u8 sr)   
{   
  W25QXX_CS(0);                            //使能器件   
  SPI1_ReadWriteByte(W25X_WriteStatusReg);   //发送写取状态寄存器命令    
  SPI1_ReadWriteByte(sr);               //写入一个字节  
  W25QXX_CS(1);                            //取消片选     	      
}   

/*********************************************************************************************
* 名称:W25QXX_Write_Enable
* 功能:W25QXX写使能	
* 参数:无
* 返回:无
* 修改:无
* 注释:将WEL置位   
*********************************************************************************************/
void W25QXX_Write_Enable(void)   
{
  W25QXX_CS(0);                            //使能器件   
  SPI1_ReadWriteByte(W25X_WriteEnable);      //发送写使能  
  W25QXX_CS(1);                            //取消片选     	      
} 

/*********************************************************************************************
* 名称:W25QXX_Write_Disable
* 功能:W25QXX写禁止	
* 参数:无
* 返回:无
* 修改:无
* 注释:将WEL清零  
*********************************************************************************************/
void W25QXX_Write_Disable(void)   
{  
  W25QXX_CS(0);                            //使能器件   
  SPI1_ReadWriteByte(W25X_WriteDisable);     //发送写禁止指令    
  W25QXX_CS(1);                            //取消片选     	      
} 		

/*********************************************************************************************
* 名称:W25QXX_ReadID
* 功能:读取芯片ID
* 参数:无
* 返回:返回值如下:				   
*        0XEF13,表示芯片型号为W25Q80  
*        0XEF14,表示芯片型号为W25Q16    
*        0XEF15,表示芯片型号为W25Q32  
*        0XEF16,表示芯片型号为W25Q64 
*        0XEF17,表示芯片型号为W25Q128 
* 修改:无
* 注释:无
*********************************************************************************************/	  
u16 W25QXX_ReadID(void)
{
  u16 Temp = 0;	  
  W25QXX_CS(0);				    
  SPI1_ReadWriteByte(0x90);//发送读取ID命令	    
  SPI1_ReadWriteByte(0x00); 	    
  SPI1_ReadWriteByte(0x00); 	    
  SPI1_ReadWriteByte(0x00); 	 			   
  Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
  Temp|=SPI1_ReadWriteByte(0xFF);	 
  W25QXX_CS(1);				    
  return Temp;
}   		    

/*********************************************************************************************
* 名称:W25QXX_Read
* 功能:读取SPI FLASH  
* 参数:pBuffer:数据存储区
*        ReadAddr:开始读取的地址(24bit)
*        NumByteToRead:要读取的字节数(最大65535)
* 返回:无
* 修改:无
* 注释:在指定地址开始读取指定长度的数据
*********************************************************************************************/
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
  u16 i;   										    
  W25QXX_CS(0);                            //使能器件   
  SPI1_ReadWriteByte(W25X_ReadData);         //发送读取命令   
  SPI1_ReadWriteByte((u8)((ReadAddr)>>16));  //发送24bit地址    
  SPI1_ReadWriteByte((u8)((ReadAddr)>>8));   
  SPI1_ReadWriteByte((u8)ReadAddr);   
  for(i=0;i<NumByteToRead;i++)
  { 
    pBuffer[i]=SPI1_ReadWriteByte(0XFF);   //循环读数  
  }
  W25QXX_CS(1);  				    	      
}  

/*********************************************************************************************
* 名称:W25QXX_Write_Page
* 功能:SPI在一页(0~65535)内写入少于256个字节的数据
* 参数:pBuffer:数据存储区
*        WriteAddr:开始写入的地址(24bit)
*        NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
* 返回:无
* 修改:无
* 注释:在指定地址开始写入最大256字节的数据
*********************************************************************************************/	 
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
  u16 i;  
  W25QXX_Write_Enable();                  //SET WEL 
  W25QXX_CS(0);                            //使能器件   
  SPI1_ReadWriteByte(W25X_PageProgram);      //发送写页命令   
  SPI1_ReadWriteByte((u8)((WriteAddr)>>16)); //发送24bit地址    
  SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
  SPI1_ReadWriteByte((u8)WriteAddr);   
  for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);//循环写数  
  W25QXX_CS(1);                            //取消片选 
  W25QXX_Wait_Busy();					   //等待写入结束
} 

/*********************************************************************************************
* 名称:W25QXX_Write_NoCheck
* 功能:无检验写SPI FLASH 
* 参数:pBuffer:数据存储区
*        WriteAddr:开始写入的地址(24bit)
*        NumByteToWrite:要写入的字节数(最大65535)
* 返回:无
* 修改:无
* 注释:必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
*        具有自动换页功能 
*        在指定地址开始写入指定长度的数据,但是要确保地址不越界!
*        CHECK OK
*********************************************************************************************/
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 
	 
/*********************************************************************************************
* 名称:W25QXX_Write
* 功能:写SPI FLASH  
* 参数:pBuffer:数据存储区
*        WriteAddr:开始写入的地址(24bit)	
*        NumByteToWrite:要写入的字节数(最大65535)   
* 返回:无
* 修改:无
* 注释:在指定地址开始写入指定长度的数据
*        该函数带擦除操作!
*        在指定地址开始写入指定长度的数据,但是要确保地址不越界!
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
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			W25QXX_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
      secpos++;//扇区地址增1
      secoff=0;//偏移位置为0 	 

      pBuffer+=secremain;  //指针偏移
      WriteAddr+=secremain;//写地址偏移	   
      NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
}

/*********************************************************************************************
* 名称:W25QXX_Erase_Chip
* 功能:擦除整个芯片	
* 参数:无
* 返回:无
* 修改:无
* 注释:等待时间超长...
*********************************************************************************************/
void W25QXX_Erase_Chip(void)   
{                                   
  W25QXX_Write_Enable();                  //SET WEL 
  W25QXX_Wait_Busy();   
  W25QXX_CS(0);                            //使能器件   
  SPI1_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令  
  W25QXX_CS(1);                            //取消片选     	      
  W25QXX_Wait_Busy();   				   //等待芯片擦除结束
}   

/*********************************************************************************************
* 名称:W25QXX_Erase_Sector
* 功能:擦除一个扇区
* 参数:Dst_Addr:扇区地址 根据实际容量设置
* 返回:无
* 修改:无
* 注释:擦除一个山区的最少时间:150ms
*********************************************************************************************/
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
  //监视falsh擦除情况,测试用   
//  printf("fe:%x\r\n",Dst_Addr);	  
  Dst_Addr*=4096;
  W25QXX_Write_Enable();                  //SET WEL 	 
  W25QXX_Wait_Busy();   
  W25QXX_CS(0);                            //使能器件   
  SPI1_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令 
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));  //发送24bit地址    
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
  SPI1_ReadWriteByte((u8)Dst_Addr);  
  W25QXX_CS(1);                            //取消片选     	      
  W25QXX_Wait_Busy();   				   //等待擦除完成
}  

/*********************************************************************************************
* 名称:W25QXX_Wait_Busy
* 功能:等待空闲
* 参数:无
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}  

/*********************************************************************************************
* 名称:W25QXX_PowerDown
* 功能:进入掉电模式
* 参数:无
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void W25QXX_PowerDown(void)   
{ 
  W25QXX_CS(0);                            //使能器件   
  SPI1_ReadWriteByte(W25X_PowerDown);        //发送掉电命令  
  W25QXX_CS(1);                            //取消片选     	      
  delay_us(3);                               //等待TPD  
}   

/*********************************************************************************************
* 名称:W25QXX_WAKEUP
* 功能:唤醒
* 参数:无
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/
void W25QXX_WAKEUP(void)   
{  
  W25QXX_CS(0);                            //使能器件   
  SPI1_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
  W25QXX_CS(1);                            //取消片选     	      
  delay_us(3);                               //等待TRES1
}   
