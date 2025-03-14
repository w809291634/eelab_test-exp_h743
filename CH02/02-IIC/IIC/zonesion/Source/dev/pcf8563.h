#ifndef __PCF8563_H_
#define __PCF8563_H_

typedef struct {
  unsigned char hour : 5;                                       // 时（0-23）
  unsigned char week : 3;                                       // 星期（0-6）
  unsigned char minute;                                         // 分（0-59）
  unsigned char second;                                         // 秒（0-59）
  unsigned char day;                                            // 日（1-31）
  unsigned char month;                                          // 月（1-12）
  unsigned char year;                                           // 年（0-99）
}calendar_t;

void PCF8563_init(void);
int PCF8563_gettime(calendar_t *time);
int PCF8563_settime(calendar_t *time);
char CountWeek(int iY, int iM, int iD) ;

#endif // __PCF8563_H_
