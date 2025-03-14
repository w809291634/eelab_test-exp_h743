/* Copyright (c) 2006-2021, RT-Thread Development Team
 * SPDX-License-Identifier: Apache-2.0 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-03     RT-Thread    first version
 * 本文件只在RTTSTUDIO中使用，防止GCC缺少库报错，IAR和KEIL中不用添加
 */
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#undef errnoextern
int errno;
extern int _end;
caddr_t _sbrk(int incr)
{
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;
  if (heap == NULL)
  {
    heap = (unsigned char *) &_end;
  }
  prev_heap = heap;
  heap += incr;
  return (caddr_t) prev_heap;
}

int _close(int file)
{
  return -1;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file)
{
  return 1;
}

int _lseek(int file, int ptr, int dir)
{
  return 0;
}

int _write(int file, char *ptr, int len)
{
  return len;
}

int _read(int file, char *ptr, int len)
{
  return 0;
}

void abort(void)
{
  /* Abort called */
  while (1)        ;
}
