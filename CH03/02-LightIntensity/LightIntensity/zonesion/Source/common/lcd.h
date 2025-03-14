#ifndef _LCD_H_
#define _LCD_H_
#include "ili9806.h"
#include "text.h"

/* ÕÂ½Ú±àºÅ1 */
#define UART                    1
#define IIC                     2
#define SPI                     3
#define LCD                     4
#define OLED                    5
#define TOUCH                   6
#define CAMERA                  7

/* ÕÂ½Ú±àºÅ2 */
#define TEMPHUMI                11
#define LIGHTINTENSITY          12
#define FAN                     13
#define INFRARED                14

#define COMBUSTIBLEGAS          15
#define STEPMOTOR               16

#define AIRPRESSURE             17
#define LOCK                    18
#define FLAME                   19

#define VIBRATION               20
#define DCMOTOR                 21

#define DISPLAYKEY              22
#define ACCELERATION            23
#define RELAY                   24

#define AIRGAS                  25
#define HALL                    26
#define SERVOMOTOR              27
#define GRATING                 28

#define REF_POS                 LCD_HEIGHT/2+20
#define SPACING                 10        // ÐÐ¼ä¾à

void lcd_init(unsigned char name);

#endif