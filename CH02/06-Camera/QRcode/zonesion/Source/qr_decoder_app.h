#ifndef __QR_DECODER_APP_H__
#define	__QR_DECODER_APP_H__

#include "extend.h"
#include <string.h>
#include <stdio.h>
#include "gui.h"

#define  USE_QR_IMG       1         //ʹ�ö�ά��ʶ��С����
#define  QR_IMG_W         256       //��ά��ʶ�𴰿ڴ�С
#define  QR_IMG_H         256

#define  QR_SYMBOL_NUM    2         //ʶ���ά���������
#define  QR_SYMBOL_SIZE   256       //ÿ���ά��ĵ��������

//�������ݷ�װΪ��ά����decoded_buf����ʽΪ��
// ����һ�飺�������ͳ���(8bit)+������������+�������ݳ���(16bit,��λ��ǰ��λ�ں�)+�������ݣ�
// ���ڶ��飺�������ͳ���(8bit)+������������+�������ݳ���(16bit,��λ��ǰ��λ�ں�)+�������ݣ�
//  ������
//�Դ�����
extern char decoded_buf[QR_SYMBOL_NUM][QR_SYMBOL_SIZE];
extern volatile unsigned int dcmi_buf[IMAGE_WIN_WIDTH*IMAGE_WIN_HEIGHT/2];

char QR_decoder(void);                //���뺯��������ֵΪʶ������ĸ���
void QR_decoder_clear(void);
void qr_img_intercept(void);
#endif /* __QR_DECODER_USER_H */
