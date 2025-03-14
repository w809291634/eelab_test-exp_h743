/*********************************************************************************************
* 文件：vk16k33.c
* 作者：xuzhy
* 说明：vk16k33驱动程序
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "vk16k33.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "iic3.h"
#include "delay.h"

static int iic3_read_buf(char addr, char r, char *buf, int len)
{
  int i;
  
  iic_start();
  if (iic_write_byte(addr<<1)) goto err;
  if (iic_write_byte(r)) goto err;
  iic_start();
  if (iic_write_byte((addr<<1)|1)) goto err;
  for (i=0; i<len-1; i++) {
    buf[i] = iic_read_byte(0);
  }
  buf[i++] = iic_read_byte(1);
  iic_stop();
  return i;
err:
  iic_stop();
  return -1;
}

static int iic3_write_buf(char addr, char r, char *buf, int len)
{
  int i;

  iic_start();
  if (iic_write_byte(addr<<1)) goto err;
  if (iic_write_byte(r)) goto err;
  for (i=0; i<len; i++) {
    if (iic_write_byte(buf[i])) goto err;
  }
  iic_stop();
  return i;
err:
  iic_stop();
  return -1;
}

#define wk16k33_write_one_byte(cmd)  iic3_write_buf(VK16K33_ADDRESS,cmd, NULL,0)
#define wk16k33_write_two_byte(cmd1,cmd2) do{   \
    char c = cmd2;                              \
    iic3_write_buf(VK16K33_ADDRESS,cmd1, &c,1); \
  }while(0)

/*********************************************************************************************
* 名称：vk16k33_init
* 功能：vk16k33初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vk16k33_init(void)
{
  iic_init();		                                        //I2C初始化
  wk16k33_write_one_byte(VK16K33_CMD_OSC_ON);
  wk16k33_write_one_byte(VK16K33_CMD_BRIGHT(4));
  wk16k33_write_one_byte(VK16K33_CMD_DISPLAY_ON);
  wk16k33_write_one_byte(VK16K33_INT_LOW);
  segment_display(0);
}

/*********************************************************************************************
* 名称：segment_display()
* 功能：数码管显示数字
* 参数：num -- 数据（最大9999）
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
char onoff = 0;
static char ledbuf[16];
void segment_display(unsigned int num)
{
  unsigned char h = 0,j = 0,k = 0,l = 0;
  if(num > 9999) num = 0;
  h = num % 10;
  j = num % 100 /10;
  k = num % 1000 / 100;
  l = num /1000;

  char map[10] = {0x3f,0x06, 0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
  ledbuf[0] = map[l];
  ledbuf[2] = map[k];
  ledbuf[4] = map[j];
  ledbuf[6] = map[h];
  iic3_write_buf(VK16K33_ADDRESS, 0, ledbuf,16);
  if (onoff == 0) {
    wk16k33_write_one_byte(VK16K33_CMD_DISPLAY_ON);
    onoff = 1;
  }
}

void display_off(void)
{
  wk16k33_write_one_byte(VK16K33_CMD_DISPLAY_OFF);
  onoff = 0;
}

void display_on(void)
{
  wk16k33_write_one_byte(VK16K33_CMD_DISPLAY_ON);
  onoff = 1;
}

unsigned char vk16k33_get_keyval(void)
{
  static char lastBuf[1];
  char buf[1] = {0};
  char __buf[6];

  if (6 == iic3_read_buf(VK16K33_ADDRESS, 0x40, __buf, 6)){
    if (lastBuf[0] == 0 && __buf[0] != 0) {
      buf[0] = __buf[0];
    } 
    lastBuf[0] = __buf[0];
    if (buf[0] & 0x01) return IICKEY_DOWN;
    if (buf[0] & 0x02) return IICKEY_RIGHT;
    if (buf[0] & 0x04) return IICKEY_CENTER;
    if (buf[0] & 0x08) return IICKEY_UP;
    if (buf[0] & 0x10) return IICKEY_LEFT;
  }
  return 0;
}
