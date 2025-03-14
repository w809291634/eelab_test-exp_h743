#include "qr_decoder_app.h"
#include "string.h"
#include "zbar.h"
#include "malloc.h"
#include "string.h"

/* ��Ƶ���������� */
/* RGB565 16λ��ʾһ������ */
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
volatile unsigned int dcmi_buf[IMAGE_WIN_WIDTH*IMAGE_WIN_HEIGHT/2];      // ����ͷ��Ƶ������

/* ��ά��ɨ�������ݴ洢���� */
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
  * @brief  ��RGB565ת��Ϊy800�Ҷ�ͼ
  * @param  c RGB565����
  * @return  �Ҷ�ͼ����
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
  * @brief  ��Ƶ���ڽ�ȡ��ά��ʶ�𴰿�
  * @param  ��
  * @return  ��
  * @note  
  */
void qr_img_intercept(void)
{
  unsigned short *p=(unsigned short *)dcmi_buf;
  /** ��ȫ��� **/
  if(QR_IMG_W>IMAGE_WIN_WIDTH || QR_IMG_H>IMAGE_WIN_HEIGHT) {
    printf("qr_img_intercept parameter error\r\n");
    return ;
  }
  
  for(int y = 0; y < QR_IMG_H; y++){          // ��ʾ�к�
    for(int x = 0; x < QR_IMG_W; x++){        // ��ʾ�к�
      int idx = y * QR_IMG_W + x;             // ɨ�贰��ͼ���������������ϵ��£�����������
      /* ��ƫ��+��ƫ�ƣ�������ͷ��Ƶ���н�ȡһ����Ҫ�Ĵ��ڽ���ͼ�δ������ﲻһ���ܹ��õ� */
      p[idx] = p[((IMAGE_WIN_HEIGHT-QR_IMG_H)/2+y)*IMAGE_WIN_WIDTH+(IMAGE_WIN_WIDTH-QR_IMG_W)/2+x];
    }
  }
}

/**
  * @brief  ��ά��ͼ��ת������
  * @param  ��
  * @return  ��
  * @note  ���Ƚ�����Ƶ���ڲü���������Ҫ����Ȼ����лҶȴ���
  */
static void qr_img_convert(void)
{
  char *p=(char *)dcmi_buf;
  unsigned short *p1=(unsigned short *)dcmi_buf;
  for(int y = 0; y < QR_IMG_H; y++){          // ��ʾ�к�
    for(int x = 0; x < QR_IMG_W; x++){        // ��ʾ�к�
      int idx = y * QR_IMG_W + x;             // ɨ�贰��ͼ���������������ϵ��£�����������
      /* ��ƫ��+��ƫ�ƣ�������ͷ��Ƶ���н�ȡһ����Ҫ�Ĵ��ڽ���ͼ�δ������ﲻһ���ܹ��õ� */
      p1[idx] = p1[((IMAGE_WIN_HEIGHT-QR_IMG_H)/2+y)*IMAGE_WIN_WIDTH+(IMAGE_WIN_WIDTH-QR_IMG_W)/2+x];
      /* ��ͼ����лҶȴ��� */
      p[idx] = rgb565_to_y800(p1[idx]);
    }
  }
}

// ����ÿ������������
void QR_decoder_clear(void)
{
  for(int t=0;t<QR_SYMBOL_NUM;t++)
    memset(decoded_buf[t],0,QR_SYMBOL_SIZE);
}

/**
  * @brief  ��ά��ʶ����
  * @param  ��
  * @return  ʶ����Ķ�ά�����
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
  
  /** ��ȫ��� **/
  if(QR_IMG_W>IMAGE_WIN_WIDTH || QR_IMG_H>IMAGE_WIN_HEIGHT) {
    printf("QR_decoder parameter error\r\n");
    return NULL;
  }
#if USE_QR_IMG==1
  /** ͼ��Ҷȴ��� **/
  for(int y = 0; y < QR_IMG_H; y++){          // ��ʾ�к�
    for(int x = 0; x < QR_IMG_W; x++){        // ��ʾ�к�
      int idx = y * QR_IMG_W + x;             // ɨ�贰��ͼ���������������ϵ��£�����������
      p1[idx] = rgb565_to_y800(p[idx]);
    }
  }
#else
  /* ��ͼ�񴰿��н�ȡһ����ͼ�����ͼ���� */
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
  symbol_num = zbar_scan_image(scanner, img);//����ʶ������ĸ���
  
  if(symbol_num)
  {
    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(img);
    for(; symbol; symbol = zbar_symbol_next(symbol)) 
    {
      zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
      
      const char *qr_typ =  zbar_get_symbol_name(typ);    //��������
      type_len =  strlen(zbar_get_symbol_name(typ));      //���ͳ���
      
      const char *data   =  zbar_symbol_get_data(symbol); //���������
      data_len =  zbar_symbol_get_data_length(symbol);    //���ݳ���
      
      //�������ݷ�װΪ��ά���飬��ʽΪ��
      // ����һ�飺�������ͳ���(8bit)+������������+�������ݳ���(16bit,��λ��ǰ��λ�ں�)+�������ݣ�
      // ���ڶ��飺�������ͳ���(8bit)+������������+�������ݳ���(16bit,��λ��ǰ��λ�ں�)+�������ݣ�
      //  ������
      //�Դ�����
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
