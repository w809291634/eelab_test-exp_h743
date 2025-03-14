#ifndef __TEXT_H__
#define __TEXT_H__
#include "ili9806.h"
#include "font_config.h"
#include "w25qxx.h"

void font_lib_init(void);
void LCDShowFont32(u16 x,u16 y,char* str,u16 width,u16 color,u16 Bcolor);
void LCDDrawFont32_Mid(u16 x,u16 y,char *str,u16 len,int color, int Bcolor);
void LCDDrawFont40(u16 x,u16 y,char *str,u16 color,u16 Bcolor);
void LCDDrawFont40_Mid(u16 x,u16 y,char *str,u16 len,int color, int Bcolor);
#endif
