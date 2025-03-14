#include "qr_decoder_app.h"
#include "string.h"
#include "zbar.h"
#include "malloc.h"
#include "string.h"

/* 视频流缓冲数组 */
/* RGB565 16位表示一个像素 */
#if defined(__ICCARM__)
#pragma location = "dcmi_buf"
#pragma data_alignment=4
#elif defined(__CC_ARM)
__attribute__((section(".dcmi_buf"), zero_init))
__attribute__ ((aligned (4)))
#elif defined(__GNUC__)
__attribute__((section(".dcmi_buf")))
__attribute__ ((aligned (4)))
#else
#error Unknown compiler
#endif
volatile unsigned int dcmi_buf[IMAGE_WIN_WIDTH*IMAGE_WIN_HEIGHT/2];      // 摄像头视频流数据

/* 二维码扫描结果数据存储数组 */
#if defined(__ICCARM__)
#pragma location = "decoded_buf"
#pragma data_alignment=4
#elif defined(__CC_ARM)
__attribute__((section(".decoded_buf"), zero_init))
__attribute__ ((aligned (4)))
#elif defined(__GNUC__)
__attribute__((section(".decoded_buf")))
__attribute__ ((aligned (4)))
#else
#error Unknown compiler
#endif
char decoded_buf[QR_SYMBOL_NUM][QR_SYMBOL_SIZE] ;

void zbar_img_cleanup(zbar_image_t *image)
{
}

/**
  * @brief  将RGB565转换为y800灰度图
  * @param  c RGB565数据
  * @return  灰度图数据
  * @note
  */
static unsigned char rgb565_to_y800(unsigned short c)
{
  unsigned char r, g, b;
  unsigned short Gray;
  r = (c >> 11) & 0x1F;
  g = (c >> 5) & 0x3F;
  b = c & 0x1F;

  Gray = (r*299 + g*587 + b*114 + 500) / 1000;
  //Gray = (r*30 + g*59 + b*11 + 50) / 100;
  //Gray = (r*313524 + g*615514 + b*119538) >> 20;
  
  if(Gray > 255)
    Gray = 255;

  return Gray;
}

/**
  * @brief  视频窗口截取二维码识别窗口
  * @param  无
  * @return  无
  * @note  
  */
void qr_img_intercept(void)
{
  unsigned short *p=(unsigned short *)dcmi_buf;
  /** 安全检查 **/
  if(QR_IMG_W>IMAGE_WIN_WIDTH || QR_IMG_H>IMAGE_WIN_HEIGHT) {
    printf("qr_img_intercept parameter error\r\n");
    return ;
  }
  
  for(int y = 0; y < QR_IMG_H; y++){          // 表示行号
    for(int x = 0; x < QR_IMG_W; x++){        // 表示列号
      int idx = y * QR_IMG_W + x;             // 扫描窗口图像数据索引，从上到下，从左到右排列
      /* 行偏移+列偏移，从摄像头视频流中截取一个需要的窗口进行图形处理，这里不一定能够用到 */
      p[idx] = p[((IMAGE_WIN_HEIGHT-QR_IMG_H)/2+y)*IMAGE_WIN_WIDTH+(IMAGE_WIN_WIDTH-QR_IMG_W)/2+x];
    }
  }
}

/**
  * @brief  二维码图像转换处理
  * @param  无
  * @return  无
  * @note  首先进行视频窗口裁剪（根据需要），然后进行灰度处理
  */
static void qr_img_convert(void)
{
  char *p=(char *)dcmi_buf;
  unsigned short *p1=(unsigned short *)dcmi_buf;
  for(int y = 0; y < QR_IMG_H; y++){          // 表示行号
    for(int x = 0; x < QR_IMG_W; x++){        // 表示列号
      int idx = y * QR_IMG_W + x;             // 扫描窗口图像数据索引，从上到下，从左到右排列
      /* 行偏移+列偏移，从摄像头视频流中截取一个需要的窗口进行图形处理，这里不一定能够用到 */
      p1[idx] = p1[((IMAGE_WIN_HEIGHT-QR_IMG_H)/2+y)*IMAGE_WIN_WIDTH+(IMAGE_WIN_WIDTH-QR_IMG_W)/2+x];
      /* 将图像进行灰度处理 */
      p[idx] = rgb565_to_y800(p1[idx]);
    }
  }
}

// 清理每个解析缓存区
void QR_decoder_clear(void)
{
  for(int t=0;t<QR_SYMBOL_NUM;t++)
    memset(decoded_buf[t],0,QR_SYMBOL_SIZE);
}

/**
  * @brief  二维码识别函数
  * @param  无
  * @return  识别出的二维码个数
  * @note 
  */
char QR_decoder(void)
{
  zbar_image_scanner_t *scanner;
  unsigned short *p=(unsigned short *)dcmi_buf;
  char *p1=(char *)dcmi_buf;
  
  char type_len;
  short data_len;
  int addr=0;
  int i=0,j=0;
  int symbol_num=0;
  
  /** 安全检查 **/
  if(QR_IMG_W>IMAGE_WIN_WIDTH || QR_IMG_H>IMAGE_WIN_HEIGHT) {
    printf("QR_decoder parameter error\r\n");
    return NULL;
  }
#if USE_QR_IMG==1
  /** 图像灰度处理 **/
  for(int y = 0; y < QR_IMG_H; y++){          // 表示行号
    for(int x = 0; x < QR_IMG_W; x++){        // 表示列号
      int idx = y * QR_IMG_W + x;             // 扫描窗口图像数据索引，从上到下，从左到右排列
      p1[idx] = rgb565_to_y800(p[idx]);
    }
  }
#else
  /* 从图像窗口中截取一部分图像进行图像处理 */
  qr_img_convert();
#endif
  /* wrap image data */
  zbar_image_t *img = zbar_image_create();
  if (img == NULL) 
  {
    return NULL;
  }
  //zbar_image_set_format(img, zbar_fourcc('Y','8','0','0'));
  zbar_image_set_format(img, zbar_fourcc_parse("Y800"));
  zbar_image_set_userdata(img, NULL);
  zbar_image_set_size(img, QR_IMG_W, QR_IMG_H);
  zbar_image_set_data(img, (char*)dcmi_buf, QR_IMG_W * QR_IMG_H, zbar_img_cleanup); 
 
  /* create a reader */
  scanner = zbar_image_scanner_create();
  /* configure the reader */
  zbar_image_scanner_set_config(scanner, ZBAR_NONE, ZBAR_CFG_ENABLE, 1);    // ZBAR_NONE

  /* scan the image for barcodes */
  symbol_num = zbar_scan_image(scanner, img);//返回识别条码的个数
  
  if(symbol_num)
  {
    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(img);
    for(; symbol; symbol = zbar_symbol_next(symbol)) 
    {
      zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
      
      const char *qr_typ =  zbar_get_symbol_name(typ);    //类型名称
      type_len =  strlen(zbar_get_symbol_name(typ));      //类型长度
      
      const char *data   =  zbar_symbol_get_data(symbol); //解码的数据
      data_len =  zbar_symbol_get_data_length(symbol);    //数据长度
      
      //解码数据封装为二维数组，格式为：
      // （第一组：解码类型长度(8bit)+解码类型名称+解码数据长度(16bit,高位在前低位在后)+解码数据）
      // （第二组：解码类型长度(8bit)+解码类型名称+解码数据长度(16bit,高位在前低位在后)+解码数据）
      //  。。。
      //以此类推
      decoded_buf[j][addr++] = type_len;
      i=type_len;
      for(; i > 0; i--)
          decoded_buf[j][addr++] = *(qr_typ++);
      
      decoded_buf[j][addr++] = data_len>>8;
      decoded_buf[j][addr++] = data_len;
      i=data_len;
      for(; i > 0; i--)
          decoded_buf[j][addr++] = *(data++);
      j++;
      addr=0;
    }
  }
  /* clean up */
  zbar_image_destroy(img);
  zbar_image_scanner_destroy(scanner);       

  return symbol_num;
}
