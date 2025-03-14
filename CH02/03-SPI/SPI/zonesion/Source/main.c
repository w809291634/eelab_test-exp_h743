/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"

/*********************************************************************************************
* 名称：hardware_init()
* 功能：硬件初始化
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
static void hardware_init(void)
{
  sys_cache_enable();                                           //打开L1-Cache 强制D-Cache透写*/
  HAL_Init();                                                   //初始化HAL库 */
  SystemClock_Config();                                         //设置时钟, 400Mhz */
  PeriphCommonClock_Config();                                   //外设特殊时钟初始化
  delay_init(400);                                              //延时初始化
  usart_init(115200);                                           //串口初始化
  led_init();                                                   //LED初始化
  key_init();                                                   //按键初始化
  
  lcd_init(SPI);                                                //LCD初始化
}

/*********************************************************************************************
* 名称：main()
* 功能：主函数
* 参数：无
* 返回：无
* 修改：
* 注意：SPI flash中存放了LCD GB2312 字库的数据信息，请不要随意修改 SPI flash 中的数据
*       具体可修改位置参考配置文件。
*********************************************************************************************/
const u8 TEXT_Buffer[]={"Hello IOT! This is flash test!"};
#define SIZE sizeof(TEXT_Buffer)
int main(void)
{
  u16 id = 0;
  u8 datatemp[SIZE];
  u32 start_addr=FLASH_EXP_ADD;                                 //统一flash的读写示例地址
  //start_addr=FLASH_EXP_ADD+4096-10;                           //可以在跨扇区读写
  //start_addr=FLASH_EXP_ADD+4096*4-10;                         //错误地址
  char temp[100]={0};
  hardware_init();                                              //硬件初始化
  /* 读取设备ID */
	while(1)
	{
    id = W25QXX_ReadID();
    if (id == W25Q64 || id == W25Q128) break;
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,                    //LCD显示文本
                  "W25Q64检查失败",LCD_WIDTH,BLACK,WHITE);    
    delay_ms(500);
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,                    //LCD显示文本
                  "请检查        ",LCD_WIDTH,BLACK,WHITE);   
    delay_ms(500);
    led_app(1000);
	}
  snprintf(temp,100,"W25Q64准备就绪，ID:0x%04X",id);
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);   
  /* 检查读写flash的范围 */
    if(start_addr<FLASH_EXP_ADD ||                              //起始地址不能小于分区起始地址
        start_addr+SIZE > FLASH_EXP_ADD+FLASH_EXP_SIZE){        //结束地址不能大于分区结束地址
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,
           "地址不允许，请检查地址范围       ",LCD_WIDTH,BLACK,WHITE);   
      while(1);
    }
  /* 应用大循环 */
  while(1){                                                     //应用大循环
		u8 key=KEY_Scan(0);
    if(key==K1_PRES)                                            //KEY1按下,写入W25Q64
    {
      LCD_Fill(8+32*2,REF_POS+32*2+SPACING*2,
               LCD_WIDTH,32*2+SPACING,WHITE);   
      LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,
                    "开始写W25Q64....",LCD_WIDTH,BLACK,WHITE);  
      W25QXX_Write((u8*)TEXT_Buffer,start_addr,SIZE);           //从定义的地址上开始写入字符串
      printf("W25Q64 Write Finished!\r\n");
      LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,
                    "W25Q64写入成功   ",LCD_WIDTH,BLACK,WHITE); 
		}
		if(key==K2_PRES)                                            //KEY2按下,读取字符串并显示
		{
      memset(datatemp,0,SIZE);                                  //清除之前读取的数据
      LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,
                    "开始读W25Q64....",LCD_WIDTH,BLACK,WHITE); 

      W25QXX_Read(datatemp,start_addr,SIZE);					          //从定义的地址上开始,读出SIZE个字节
      printf("W25Q64 Read Finished!data:%s\r\n",datatemp);
      snprintf(temp,100,"读取的数据是:%s",datatemp);
      LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,
                    temp,LCD_WIDTH,BLACK,WHITE); 
    } 

    led_app(10);                                                //LED状态显示
    delay_ms(10);                                               //延时
  }
}  
