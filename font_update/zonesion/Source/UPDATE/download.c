#include "download.h"
#include "ymodem.h"
#include "delay.h"
#include "w25qxx.h"
#include "font_config.h"

extern uint8_t file_name[FILE_NAME_LENGTH];

void font_download(void)
{
  uint8_t Number[10] = "          ";
  int32_t Size = 0;
  uint32_t timeout=300;
  int32_t errarr[MAX_ERRORS]={0};
  
  /* 进入下载模式 */
  printf("font_download address:0x%08x,flash size:%d Bytes\n\r",FONT_LIB_STORE_ADD,W25QXX_SIZE);
  printf("Waiting for the file to be sent ... (press 'a' key to exit Download mode)\n\r");
  
  // 阻塞运行
  Size = Ymodem_Receive(FONT_LIB_STORE_ADD,W25QXX_SIZE,timeout,errarr);
  delay_ms(500);
  if (Size > 0)
  {
    printf("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: %s",(char*)file_name);
    Int2Str(Number, Size);
    printf("\n\r Size: %s Bytes\r\n",(char*)Number);
    printf("-------------------\r\n");
    font_info.font_flag = FONT_OK;
    WRITE_FON_INFO;
  }
  else if (Size == 0) debug_err("\n\r"ERR"Termination by sender!\n\r");
  else if (Size == -1) debug_err("\n\r"ERR"Programming address error or File too large!\n\r");
  else if (Size == -2) debug_err("\n\r"ERR"Erase flash error!\n\r");
  else if (Size == -3) debug_err("\n\r"ERR"Programming flash error!\r\n");
  else if (Size == -10) {
    debug_err("\n\r"ERR"Error count exceeded.\r\n");
    for(int i=0;i<ARRAY_SIZE(errarr);i++){
      debug_err(ERR"Error%d=%d.\r\n",i,errarr[i]);
    }
  }
  else if (Size == -11) debug_err("\n\r"ERR"Waiting timeout\n\r");
  else if (Size == -30) debug_war("\n\r"WARNING"Aborted by user.\n\r");
  else debug_err("\n\r"ERR"Failed to receive the file!\n\r");

  /* 进入控制台模式 */
  usart1_mode=0;
}

