#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "extend.h"
#include "ili9806.h"
#include "stdio.h"
#include "led.h"

void show_logo(void);
void show_fill_light(uint8_t status);
void show_value(uint16_t value);
void show_mode(uint8_t status);

#endif
