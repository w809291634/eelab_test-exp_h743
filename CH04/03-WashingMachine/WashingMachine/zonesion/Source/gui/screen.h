#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "extend.h"
#include "stdio.h"
#include "led.h"
#include "ili9806.h"

void show_logo(void);
void show_switch(uint8_t status);
//void show_b_num(uint16_t x, uint16_t y, uint8_t num);
//void show_s_num(uint16_t x, uint16_t y, uint8_t num);
void show_soak(uint8_t status);
void show_wash(uint8_t status); 
void show_rinse(uint8_t status);
void show_dehyd(uint8_t status);
void show_all_mode(uint8_t status);
void show_timer(uint8_t hours, uint8_t minute);
void show_point(uint8_t status);
void show_level(uint8_t level);

#endif
