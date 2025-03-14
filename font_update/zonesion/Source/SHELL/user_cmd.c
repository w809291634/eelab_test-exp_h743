#include "user_cmd.h"
#include "shell.h"
#include "usart1.h"
#include "w25qxx.h" 
#include "stdlib.h"
#include "font_config.h"

static void SoftReset(void* arg)
{ 
  (void)arg;
  __set_FAULTMASK(1); // 关闭所有中端
  NVIC_SystemReset(); // 复位
}

static void font_update(void * arg)
{
  usart1_mode=1;
}

// 字体数据读取，读取一个字体的数据，用作flash写入数据测试
// 24号字体的一个点阵为32*32 一个字体占用128字节
static void font_read(void * arg)
{
#define SIZE 128
  char * argv[2];
  char buf[SIZE];
  int argc =cmdline_strtok((char*)arg,argv,2);
  int addr=atoi(argv[1]);
  
  printf("\r\n");
  W25QXX_Read((u8*)buf,FONT_LIB_STORE_ADD+addr,SIZE);
  for(int m=0;m<SIZE/16;m++){   // 行
    for(int n=0;n<16;n++){      // 列
      if(n==0)printf("%08d ",addr+16*m);
      printf("%02X ",buf[16*m+n]);
      if(n==7)printf(" ");
      if(n==15)printf("\r\n");
    }
  }
  printf("\r\n");
}

// 用户命令注册
void register_user_cmd(void)
{
  shell_register_command("reboot",SoftReset);
  shell_register_command("font_update",font_update);
  shell_register_command("font_read",font_read);
}
