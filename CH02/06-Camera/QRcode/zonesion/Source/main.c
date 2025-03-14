/*********************************************************************************************
* ͷ�ļ�
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
* ���ƣ�hardware_init()
* ���ܣ�Ӳ����ʼ��
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
static void hardware_init(void)
{
  Board_MPU_Config(0, MPU_Normal_WT, 0x24000000, MPU_512KB);    //AXISRAM ���򣬿ռ�512KB
  Board_MPU_Config(1, MPU_Normal_WT, 0x30000000, MPU_512KB);    //SRAM123 ����ʵ��ʹ�ÿռ�288
  sys_cache_enable();                                           //��L1-Cache ǿ��D-Cache͸д*/
  HAL_Init();                                                   //��ʼ��HAL�� */
  SystemClock_Config();                                         //����ʱ��, 400Mhz */
  PeriphCommonClock_Config();                                   //��������ʱ�ӳ�ʼ��
  delay_init(400);                                              //��ʱ��ʼ��
  usart_init(115200);                                           //���ڳ�ʼ��
  led_init();                                                   //LED��ʼ��
  key_init();                                                   //������ʼ��
  my_mem_init(SRAMIN);                                          //��ʼ���ڲ��ڴ��

  TIMX_Int_Init(10000-1,20000 - 1);                             //20KHz�ļ���Ƶ��,1�����ж�һ��,���ڼ���֡��
  DCMI_init();                                                  //DCMI����

  DCMI_DMA_Init((uint32_t)&g_dcmi_multi_buf[0],                 //����˫�����BUF1
                (uint32_t)&g_dcmi_multi_buf[1],                 //����˫�����BUF2
                Dma_MultiBuffer_size,                           //����˫�����buf��С
                DMA_MDATAALIGN_WORD,                            //����Ϊword
                DMA_MINC_ENABLE);                               //DCMI DMA���� 

  lcd_init(QRCODE);                                             //LCD��ʼ��
}

#define RESULT_START_X    50                                    //�����ά����Ϣ��ʾ���
#define RESULT_START_Y    360                                   //�����ά����Ϣ��ʾ���

/*********************************************************************************************
* ���ƣ�clean_lcd_info()
* ���ܣ�����LCD�ϵĶ�����Ϣ
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
static void clean_lcd_info(int num)
{
  switch(num){
    /* ����ڶ�����Ϣ */
    case 1:
    LCD_Fill(RESULT_START_X, RESULT_START_Y+32+SPACING,
            LCD_WIDTH-RESULT_START_X, 32, WHITE);break;
    /* �����������Ϣ����ʾ */
//      case 2:
//      LCD_Fill(RESULT_START_X, RESULT_START_Y+32*2+SPACING*2,
//              LCD_WIDTH-RESULT_START_X, 32, WHITE);break;
  }
}

/*********************************************************************************************
* ���ƣ�DCMI_isr_app()
* ���ܣ�DCMI֡�ж���Ϊ����ʾLCD����������
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
void DCMI_isr_app(void)
{
  char buf[20];
  char result_buf[100];
  static char count=0;count++;
  GUI_RESET_WIN;
  
  dcmi_data_len = 0;                                            // ˫��������ݽ��ճ��ȸ�λ
  
  /* ��ʾ֡�� */ 
  POINT_COLOR=BLACK;
  snprintf(buf,20,"%2d",ov_frame);
  LCD_ShowString(lcd_dev.width-GUI_STAUS_BAR_FONT*2,
                 lcd_dev.height-GUI_STAUS_BAR_FONT*1-8,
                 GUI_STAUS_BAR_FONT*3,24,24,(u8*)buf);


  /* ��������ʾ��ά������Ϣ */ 
  char symbol_num=QR_decoder();
  if(symbol_num){
    count=0;

    for(int i=0;i<symbol_num;i++){
      // decoded_buf �������ͳ���(8bit)+������������+�������ݳ���(16bit,��λ��ǰ��λ�ں�)+�������ݣ�
      u8* type_len=(u8*)&decoded_buf[i][0];                     // �������ͳ���(8bit)
      char* type_section=&decoded_buf[i][0]+1;                  // ������������
      u8* data_len=(u8*)&decoded_buf[i][0]+decoded_buf[i][0]+1; // �������ݳ���(16bit,��λ��ǰ��λ�ں�)
      char* data_section=&decoded_buf[i][0]+decoded_buf[i][0]+3;// ��������
      //printf("%d %s %d %s\r\n",*type_len,type_section,(*data_len<<8)+*(data_len+1),data_section);
      
      int len=utf8togbk2312((unsigned char*)data_section,(unsigned char*)data_section);
      data_section[len]=0;                                      // ��������ַ�
      snprintf(result_buf,100,"QRcode%d:%s                    \r\n",i,data_section);
      LCDShowFont32(RESULT_START_X,RESULT_START_Y+32*i+SPACING*i,result_buf,LCD_WIDTH,BLACK,WHITE);
      printf("%s",result_buf);
      if(i==1)break;                                            // ���������ʾ2�����
    }
    printf("******************\r\n"); 
    printf("\r\n");
    clean_lcd_info(symbol_num);                                 // ����ʶ�����������ʵʱ����LCD��Ļ��Ϣ
    
    QR_decoder_clear();                                         // ���ݴ�����ɣ�������
  }
  
  /* ��ʱ����LCD�Ķ�ά���� */
  if(count>20){
    LCD_Fill( RESULT_START_X,RESULT_START_Y,                    // ���
              lcd_dev.width-RESULT_START_X,                     // �Զ������
              lcd_dev.height-GUI_STAUS_BAR_FONT*1-8-RESULT_START_Y,// �Զ����߶�
              WHITE);
    count=0;
  }
}

/*********************************************************************************************
* ���ƣ�rgb565_app()
* ���ܣ�RGB565����Ӧ�ó���
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
static void rgb565_app(void)
{ 
  /* ���ݴ�����ɣ������棬��ֹUTF8ת��ʱ���ִ��� */
  QR_decoder_clear();
  /* ����ͷ�����ʼ�� */
  LCD_Clear(WHITE);
  GUI_RESET_WIN;
  
  //��ʾ����
  LCDDrawFont40_Mid(0,  20,  "��ά��ʶ��",LCD_WIDTH,BLACK,WHITE);
  //��ʾ֡��
  LCD_ShowString(lcd_dev.width-GUI_STAUS_BAR_FONT*8+GUI_STAUS_BAR_FONT/2,
                 lcd_dev.height-GUI_STAUS_BAR_FONT*1-8,
                 200,
                 GUI_STAUS_BAR_FONT,GUI_STAUS_BAR_FONT,"Frame rate:");
  
  /* ����ͷ������ʼ�� */
  OV2640_RGB565_Mode();
  OV2640_Resolution_800_600();
  OV2640_OutSize_Set(IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT); 
  /* ����ͷ���ݲ��� */
  DCMI_Start();
  while(1)
  {
    /* ���ﲻ������ */
    delay_ms(10);                                               //��ʱ
  }
}

/*********************************************************************************************
* ���ƣ�main()
* ���ܣ�������
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
int main(void)
{
  u8 count=0,blink=1;
  u8 key;
  u16 MID=0xffff,PID=0xffff;
  char buf[100];
  hardware_init();                                              //Ӳ����ʼ��
  
  /* ��ʼ��OV2640 */
  while(OV2640_Init(&MID,&PID))                                 
  {
    snprintf(buf,100,"OV2640��ʼ��ʧ��,Product ID:0x%04x",PID);
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,                    //LCD��ʾ�ı�
                  buf,LCD_WIDTH,BLACK,WHITE);    
    delay_ms(500);
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,                    //LCD��ʾ�ı�
                  "����...                               ",LCD_WIDTH,BLACK,WHITE);   
    delay_ms(500);
    led_app(1000);
  }
  snprintf(buf,100,"OV2640��ʼ���ɹ�,Product ID:0x%04x",PID);
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,                      //LCD��ʾ�ı�
                buf,LCD_WIDTH,BLACK,WHITE);    
  LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,                  //LCD��ʾ�ı�
                "������ⰴ�������ά��ʶ�����...",LCD_WIDTH,BLACK,WHITE);   
  while(1){
    key=KEY_Scan(0);
    /* ���ⰴ������APP */
    if(key){
      rgb565_app();
    }
    /* �����л���ʾ */
    if(count>100){
      count=0;
      blink=!blink;
      if(blink)
        LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,            //LCD��ʾ�ı�
                      "������ⰴ�������ά��ʶ�����...",LCD_WIDTH,BLACK,WHITE);    
      else
        LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,            //LCD��ʾ�ı�
                      "                                  ",LCD_WIDTH,BLACK,WHITE);    
    }

    count++;
    delay_ms(10);                                               //��ʱ10����
  }
}  
