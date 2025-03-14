/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "ov2640.h"
#include "dcmi.h"
#include "gui.h"
#include "dev/timer.h"
#include "stdio.h"
#include "string.h"
#include "qr_decoder_app.h"
#include "malloc.h"
#include "utf8togbk.h"

/*********************************************************************************************
* 名称：hardware_init()
* 功能：硬件初始化
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
static void hardware_init(void)
{
  Board_MPU_Config(0, MPU_Normal_WT, 0x24000000, MPU_512KB);    //AXISRAM 区域，空间512KB
  Board_MPU_Config(1, MPU_Normal_WT, 0x30000000, MPU_512KB);    //SRAM123 区域，实际使用空间288
  sys_cache_enable();                                           //打开L1-Cache 强制D-Cache透写*/
  HAL_Init();                                                   //初始化HAL库 */
  SystemClock_Config();                                         //设置时钟, 400Mhz */
  PeriphCommonClock_Config();                                   //外设特殊时钟初始化
  delay_init(400);                                              //延时初始化
  usart_init(115200);                                           //串口初始化
  led_init();                                                   //LED初始化
  key_init();                                                   //按键初始化
  my_mem_init(SRAMIN);                                          //初始化内部内存池

  TIMX_Int_Init(10000-1,20000 - 1);                             //20KHz的计数频率,1秒钟中断一次,用于计算帧率
  DCMI_init();                                                  //DCMI配置

  DCMI_DMA_Init((uint32_t)&g_dcmi_multi_buf[0],                 //设置双缓存的BUF1
                (uint32_t)&g_dcmi_multi_buf[1],                 //设置双缓存的BUF2
                Dma_MultiBuffer_size,                           //设置双缓存的buf大小
                DMA_MDATAALIGN_WORD,                            //对齐为word
                DMA_MINC_ENABLE);                               //DCMI DMA配置 

  lcd_init(QRCODE);                                             //LCD初始化
}

#define RESULT_START_X    50                                    //定义二维码信息显示起点
#define RESULT_START_Y    360                                   //定义二维码信息显示起点

/*********************************************************************************************
* 名称：clean_lcd_info()
* 功能：清理LCD上的多余信息
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
static void clean_lcd_info(int num)
{
  switch(num){
    /* 清理第二行信息 */
    case 1:
    LCD_Fill(RESULT_START_X, RESULT_START_Y+32+SPACING,
            LCD_WIDTH-RESULT_START_X, 32, WHITE);break;
    /* 清理第三行信息的显示 */
//      case 2:
//      LCD_Fill(RESULT_START_X, RESULT_START_Y+32*2+SPACING*2,
//              LCD_WIDTH-RESULT_START_X, 32, WHITE);break;
  }
}

/*********************************************************************************************
* 名称：DCMI_isr_app()
* 功能：DCMI帧中断中为了显示LCD的其他数据
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
void DCMI_isr_app(void)
{
  char buf[20];
  char result_buf[100];
  static char count=0;count++;
  GUI_RESET_WIN;
  
  dcmi_data_len = 0;                                            // 双缓存的数据接收长度复位
  
  /* 显示帧率 */ 
  POINT_COLOR=BLACK;
  snprintf(buf,20,"%2d",ov_frame);
  LCD_ShowString(lcd_dev.width-GUI_STAUS_BAR_FONT*2,
                 lcd_dev.height-GUI_STAUS_BAR_FONT*1-8,
                 GUI_STAUS_BAR_FONT*3,24,24,(u8*)buf);


  /* 解析并显示二维码结果信息 */ 
  char symbol_num=QR_decoder();
  if(symbol_num){
    count=0;

    for(int i=0;i<symbol_num;i++){
      // decoded_buf 解码类型长度(8bit)+解码类型名称+解码数据长度(16bit,高位在前低位在后)+解码数据）
      u8* type_len=(u8*)&decoded_buf[i][0];                     // 解码类型长度(8bit)
      char* type_section=&decoded_buf[i][0]+1;                  // 解码类型名称
      u8* data_len=(u8*)&decoded_buf[i][0]+decoded_buf[i][0]+1; // 解码数据长度(16bit,高位在前低位在后)
      char* data_section=&decoded_buf[i][0]+decoded_buf[i][0]+3;// 解码数据
      //printf("%d %s %d %s\r\n",*type_len,type_section,(*data_len<<8)+*(data_len+1),data_section);
      
      int len=utf8togbk2312((unsigned char*)data_section,(unsigned char*)data_section);
      data_section[len]=0;                                      // 清理多余字符
      snprintf(result_buf,100,"QRcode%d:%s                    \r\n",i,data_section);
      LCDShowFont32(RESULT_START_X,RESULT_START_Y+32*i+SPACING*i,result_buf,LCD_WIDTH,BLACK,WHITE);
      printf("%s",result_buf);
      if(i==1)break;                                            // 设置最多显示2个结果
    }
    printf("******************\r\n"); 
    printf("\r\n");
    clean_lcd_info(symbol_num);                                 // 根据识别出来的数量实时清理LCD屏幕信息
    
    QR_decoder_clear();                                         // 数据处理完成，清理缓存
  }
  
  /* 延时清理LCD的二维码结果 */
  if(count>20){
    LCD_Fill( RESULT_START_X,RESULT_START_Y,                    // 起点
              lcd_dev.width-RESULT_START_X,                     // 自动填充宽度
              lcd_dev.height-GUI_STAUS_BAR_FONT*1-8-RESULT_START_Y,// 自动填充高度
              WHITE);
    count=0;
  }
}

/*********************************************************************************************
* 名称：rgb565_app()
* 功能：RGB565测试应用程序
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
static void rgb565_app(void)
{ 
  /* 数据处理完成，清理缓存，防止UTF8转换时出现错误 */
  QR_decoder_clear();
  /* 摄像头界面初始化 */
  LCD_Clear(WHITE);
  GUI_RESET_WIN;
  
  //显示标题
  LCDDrawFont40_Mid(0,  20,  "二维码识别",LCD_WIDTH,BLACK,WHITE);
  //显示帧率
  LCD_ShowString(lcd_dev.width-GUI_STAUS_BAR_FONT*8+GUI_STAUS_BAR_FONT/2,
                 lcd_dev.height-GUI_STAUS_BAR_FONT*1-8,
                 200,
                 GUI_STAUS_BAR_FONT,GUI_STAUS_BAR_FONT,"Frame rate:");
  
  /* 摄像头参数初始化 */
  OV2640_RGB565_Mode();
  OV2640_Resolution_800_600();
  OV2640_OutSize_Set(IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT); 
  /* 摄像头数据捕获 */
  DCMI_Start();
  while(1)
  {
    /* 这里不做处理 */
    delay_ms(10);                                               //延时
  }
}

/*********************************************************************************************
* 名称：main()
* 功能：主函数
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
int main(void)
{
  u8 count=0,blink=1;
  u8 key;
  u16 MID=0xffff,PID=0xffff;
  char buf[100];
  hardware_init();                                              //硬件初始化
  
  /* 初始化OV2640 */
  while(OV2640_Init(&MID,&PID))                                 
  {
    snprintf(buf,100,"OV2640初始化失败,Product ID:0x%04x",PID);
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,                    //LCD显示文本
                  buf,LCD_WIDTH,BLACK,WHITE);    
    delay_ms(500);
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,                    //LCD显示文本
                  "请检查...                               ",LCD_WIDTH,BLACK,WHITE);   
    delay_ms(500);
    led_app(1000);
  }
  snprintf(buf,100,"OV2640初始化成功,Product ID:0x%04x",PID);
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,                      //LCD显示文本
                buf,LCD_WIDTH,BLACK,WHITE);    
  LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,                  //LCD显示文本
                "点击任意按键进入二维码识别界面...",LCD_WIDTH,BLACK,WHITE);   
  while(1){
    key=KEY_Scan(0);
    /* 任意按键进入APP */
    if(key){
      rgb565_app();
    }
    /* 定期切换显示 */
    if(count>100){
      count=0;
      blink=!blink;
      if(blink)
        LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,            //LCD显示文本
                      "点击任意按键进入二维码识别界面...",LCD_WIDTH,BLACK,WHITE);    
      else
        LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,            //LCD显示文本
                      "                                  ",LCD_WIDTH,BLACK,WHITE);    
    }

    count++;
    delay_ms(10);                                               //延时10毫秒
  }
}  
