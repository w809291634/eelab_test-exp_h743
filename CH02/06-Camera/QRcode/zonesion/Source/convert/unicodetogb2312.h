#ifndef _GB2312ToUnicode_H_BAB
#define _GB2312ToUnicode_H_BAB
#include "extend.h"

u16 UnicodeToGB2312(u16 unicode);
u16 GB2312ToUnicode(u16 data);
void GB2312ToUnicode_Str(u16 * const pOutUnicodeStr,u8 * const pGB2312Str);
s16 NetworkByteOrder(u8 * const pOut,u16 * const pIn,u16  const nByte);
#endif
