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
#include "timer.h"
#include "stdio.h"
#include "string.h"

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

  TIMX_Int_Init(10000-1,20000 - 1);                             //10KHz的计数频率,1秒钟中断一次,用于计算帧率
  DCMI_init();                                                  //DCMI配置
  DCMI_DMA_Init((u32)Bank1_LCD_D,                               //DCMI DMA 目标地址配置
                1,                                              //DMA 目标宽度16位
                DMA_MDATAALIGN_HALFWORD,                        //DMA 目标宽度16位
                DMA_MINC_DISABLE);                              //DMA 目标地址不递增

  lcd_init(CAMERA);                                             //LCD初始化
}

const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	  //7种特效 
u8 contrast=2,saturation=2,effect=0,resolution=0;
u8 contrast_buf[30];
u8 saturation_buf[30];
u8 effect_buf[30];
u8 resolution_buf[30];
volatile u8 key_active;

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
  GUI_RESET_WIN;
  // 显示帧率
  POINT_COLOR=BLACK;
  snprintf(buf,20,"%2d",ov_frame);
  LCD_ShowString(GUI_STAUS_BAR_X+GUI_STAUS_BAR_FONT/2*11,
                 GUI_STAUS_BAR_Y,GUI_STAUS_BAR_FONT*3,24,24,(u8*)buf);
  
  // 显示按钮当前模式
  if(key_active){
    key_active=0;
    POINT_COLOR=RED;
    LCD_ShowString_Mid(GUI_STAUS_BAR_X+4,GUI_BTU_Y2(1)-16*2,GUI_BTU_W,16,(char*)contrast_buf);
    LCD_ShowString_Mid(GUI_STAUS_BAR_X+4,GUI_BTU_Y2(2)-16*2,GUI_BTU_W,16,(char*)saturation_buf);
    LCD_ShowString_Mid(GUI_STAUS_BAR_X+4,GUI_BTU_Y2(3)-16*2,GUI_BTU_W,16,(char*)effect_buf);
    LCD_ShowString_Mid(GUI_STAUS_BAR_X+4,GUI_BTU_Y2(4)-16*2,GUI_BTU_W,16,(char*)resolution_buf);
  }
}

/*********************************************************************************************
* 名称：rgb565_app()
* 功能：RGB565测试应用程序,RGB数据直接显示在LCD上面
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
static void rgb565_app(void)
{ 
  u8 key;
  u8 msgbuf[30];
  /* 摄像头界面初始化 */
  LCD_Clear(WHITE);
  GUI_RESET_WIN;
  //显示帧率
  LCD_ShowString(GUI_STAUS_BAR_X,
                 GUI_STAUS_BAR_Y,200,
                 GUI_STAUS_BAR_FONT,GUI_STAUS_BAR_FONT,
                 "Frame rate:");                                
  // 绘制按钮
  gui_draw_btu(GUI_STAUS_BAR_X,GUI_BTU_Y1(1),
               lcd_dev.width-10,GUI_BTU_Y2(1),2,BLACK);
  gui_draw_btu(GUI_STAUS_BAR_X,GUI_BTU_Y1(2),
               lcd_dev.width-10,GUI_BTU_Y2(2),2,BLACK);
  gui_draw_btu(GUI_STAUS_BAR_X,GUI_BTU_Y1(3),
               lcd_dev.width-10,GUI_BTU_Y2(3),2,BLACK);
  gui_draw_btu(GUI_STAUS_BAR_X,GUI_BTU_Y1(4),
               lcd_dev.width-10,GUI_BTU_Y2(4),2,BLACK);
  // 显示按钮内容
  LCDDrawFont32_Mid(GUI_STAUS_BAR_X,GUI_BTU_Y1(1)+SPACING,"设置对比度",GUI_BTU_W,BLACK,WHITE); 
  LCDDrawFont32_Mid(GUI_STAUS_BAR_X,GUI_BTU_Y1(2)+SPACING,"设置饱和度",GUI_BTU_W,BLACK,WHITE); 
  LCDDrawFont32_Mid(GUI_STAUS_BAR_X,GUI_BTU_Y1(3)+SPACING,"设置特效",GUI_BTU_W,BLACK,WHITE); 
  LCDDrawFont32_Mid(GUI_STAUS_BAR_X,GUI_BTU_Y1(4)+SPACING,"设置分辨率",GUI_BTU_W,BLACK,WHITE); 

  /* 摄像头参数初始化 */
  OV2640_RGB565_Mode();
  OV2640_Resolution_800_600();
  OV2640_OutSize_Set(IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT); 
  /* 摄像头数据捕获 */
  DCMI_Start();
  /* 首次数据刷新 */
  sprintf((char*)contrast_buf,"   Contrast:%d   ",(signed char)contrast-2);
  sprintf((char*)saturation_buf,"   Saturation:%d   ",(signed char)saturation-2);
  sprintf((char*)effect_buf,"   %s   ",EFFECTS_TBL[effect]);
  strcpy((char*)resolution_buf,"   800x600 30fps   ");
  key_active=1;
  while(1)
  { 
    key=KEY_Scan(0); 
    if(key)
    { 
      DCMI_Stop(0);                                             //停止显示
      POINT_COLOR=RED;	                                        //画笔颜色
      key_active=1;
      switch(key)
      {
        case K1_PRES:                                           //对比度设置
          contrast++;
          if(contrast>4)contrast=0;
          OV2640_Contrast(contrast);
          sprintf((char*)msgbuf,"   Contrast:%d   ",(signed char)contrast-2);
          strncpy((char*)contrast_buf,(char*)msgbuf,30);
          break;
        case K2_PRES:                                           //饱和度Saturation
          saturation++;
          if(saturation>4)saturation=0;
          OV2640_Color_Saturation(saturation);
          sprintf((char*)msgbuf,"   Saturation:%d   ",(signed char)saturation-2);
          strncpy((char*)saturation_buf,(char*)msgbuf,30);
          break;
        case K3_PRES:                                           //特效设置
          effect++;
          if(effect>6)effect=0;
          OV2640_Special_Effects(effect);                       //设置特效
          sprintf((char*)msgbuf,"   %s   ",EFFECTS_TBL[effect]);
          strncpy((char*)effect_buf,(char*)msgbuf,30);
          break;
        case K4_PRES:                                           //分辨率设置
          resolution=!resolution;
          if(resolution){
            OV2640_Resolution_1600_1200();
            OV2640_OutSize_Set(IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT);
            strcpy((char*)msgbuf,"  1600x1200 15fps  ");
          }else{
            OV2640_Resolution_800_600();
            OV2640_OutSize_Set(IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT); 
            strcpy((char*)msgbuf,"   800x600 30fps   ");
          }
          strncpy((char*)resolution_buf,(char*)msgbuf,30);
          break;
      }
      GUI_RESET_WIN;
      LCD_ShowString(30+SPACING,50,210,16,16,msgbuf);           //显示提示内容
      delay_ms(100);                                            //显示等待
      DCMI_Start();                                             //重新开始传输
    } 
    led_app(10);                                                //LED指示应用
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
                "点击任意按键进入摄像头界面...",LCD_WIDTH,BLACK,WHITE);   
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
                      "点击任意按键进入摄像头界面...",LCD_WIDTH,BLACK,WHITE);    
      else
        LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,            //LCD显示文本
                      "                              ",LCD_WIDTH,BLACK,WHITE);    
    }

    count++;
    led_app(10);                                                //LED指示应用
    delay_ms(10);                                               //延时10毫秒
  }
}  