#ifndef __FONT_CONFIG_H__
#define __FONT_CONFIG_H__

typedef struct {
  int font_flag;            // 存储标志
}font_info_t;
extern font_info_t font_info;

#define FONT_OK                   0XA5B4C3D2
#define FONT_NOK                  0Xffffffff
/** 分区信息 **/
#define FONT_INFO_ADD             0x1000              // 字库存储信息的起始地址，占用4KB
#define FONT_LIB_STORE_ADD        0x2000              // GB2312_24.DZK 二进制文件的存储起始地址。
#define FONT_LIB_PARTITION_SIZE   0x200000            // 字库占用2MB的空间
// SPI flash 的实验所用空间
#define FLASH_EXP_ADD             FONT_LIB_STORE_ADD+FONT_LIB_PARTITION_SIZE     // SPI flash 读写 实验 的起始地址
#define FLASH_EXP_SIZE            0x4000              // SPI flash 读写 实验 16kb的占用空间

/** 字体信息读写接口 **/
#define READ_FON_INFO             {W25QXX_Read((u8*)&font_info,FONT_INFO_ADD,sizeof(font_info_t));}
#define WRITE_FON_INFO            {W25QXX_Erase_Sector(FONT_INFO_ADD/4096); \
                                  W25QXX_Write_NoCheck((u8*)&font_info,FONT_INFO_ADD,sizeof(font_info_t));}
#endif