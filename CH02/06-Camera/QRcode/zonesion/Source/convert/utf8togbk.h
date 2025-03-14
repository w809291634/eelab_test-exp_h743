#ifndef __UTF8TOGBK_H__
#define __UTF8TOGBK_H__
#include "extend.h"

int enc_get_utf8_size(const unsigned char pInput);
int enc_utf8_to_unicode_one(const unsigned char* pInput, unsigned long *Unic);
int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput);
// 使用文件系统中的库对UNICODE进行转换为GBK
WCHAR ff_convert (	/* Converted code, 0 means conversion error */
	WCHAR	chr,	/* Character code to be converted */
	UINT	dir		/* 0: Unicode to OEMCP, 1: OEMCP to Unicode */
);
// UTF8类型编码转换为GB2312编码
int utf8togbk2312(unsigned char* pInput,unsigned char *pOutput);
#endif