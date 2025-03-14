#ifndef __QR_DECODER_APP_H__
#define	__QR_DECODER_APP_H__

#include "extend.h"
#include <string.h>
#include <stdio.h>
#include "gui.h"

#define  USE_QR_IMG       1         //使用二维码识别小窗口
#define  QR_IMG_W         256       //二维码识别窗口大小
#define  QR_IMG_H         256

#define  QR_SYMBOL_NUM    2         //识别二维码的最大个数
#define  QR_SYMBOL_SIZE   256       //每组二维码的的最大容量

//解码数据封装为二维数组decoded_buf，格式为：
// （第一组：解码类型长度(8bit)+解码类型名称+解码数据长度(16bit,高位在前低位在后)+解码数据）
// （第二组：解码类型长度(8bit)+解码类型名称+解码数据长度(16bit,高位在前低位在后)+解码数据）
//  。。。
//以此类推
extern char decoded_buf[QR_SYMBOL_NUM][QR_SYMBOL_SIZE];
extern volatile unsigned int dcmi_buf[IMAGE_WIN_WIDTH*IMAGE_WIN_HEIGHT/2];

char QR_decoder(void);                //解码函数，返回值为识别条码的个数
void QR_decoder_clear(void);
void qr_img_intercept(void);
#endif /* __QR_DECODER_USER_H */
