/*********************************************************************************************
* 文件：lis3dh.c
* 作者：Zhoucj 2019.03.07
* 说明：lis3dh驱动程序,包括跌倒,计步,久坐
* 修改：
* 注释：
*********************************************************************************************/

#include "lis3dh.h"
#include "iic3.h"
#include "delay.h"

/*********************************************************************************************
* 名称:lis3dh_readReg()
* 功能:读取数据
* 参数:regAddr：读取的地址
* 返回:data
* 修改:
* 注释:
*********************************************************************************************/
int lis3dh_readReg(unsigned char regAddr)
{
  int data;
  I2C3_Start();
  I2C3_WriteByte(LIS3DH_ADDR & 0xFE);
  if(I2C3_WaitAck()) 
    return -1;
  I2C3_WriteByte(regAddr);
  if(I2C3_WaitAck()) 
    return -1;
  I2C3_Start();
  I2C3_WriteByte(LIS3DH_ADDR | 0x01);
  if(I2C3_WaitAck()) return -1;
  data = I2C3_ReadByte();
  I2C3_Stop();
  return data;
}

/*********************************************************************************************
* 名称:lis3dh_writeReg()
* 功能:写数据
* 参数:regAddr：读取的地址，data：需要写的数据
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
int lis3dh_writeReg(unsigned char regAddr, unsigned char data)
{
  I2C3_Start();
  I2C3_WriteByte(LIS3DH_ADDR & 0xFE);
  if(I2C3_WaitAck()) return -1;
  I2C3_WriteByte(regAddr);
  if(I2C3_WaitAck()) return -1;
  I2C3_WriteByte(data);
  if(I2C3_WaitAck()) return -1;
  I2C3_Stop();
  return 0;
}

/*********************************************************************************************
* 名称:lis3dh_init()
* 功能:初始化
* 参数:
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
int lis3dh_init(void)
{
  I2C3_GPIOInit();
  delay_ms(50);
  
  int data=lis3dh_readReg(LIS3DH_IDADDR);
  if(LIS3DH_ID != lis3dh_readReg(LIS3DH_IDADDR))                // 获取ID
    return -1;
  
  delay_ms(50);
  if(lis3dh_writeReg(LIS3DH_CTRL_REG1, 0x97) < 0)               // 设置速率1.25KHz,启用XYZ轴
    return -1;
  
  delay_ms(50);
  if(lis3dh_writeReg(LIS3DH_CTRL_REG4, 0x10) < 0)               
    return -1;
  return 0;
}

/*********************************************************************************************
* 名称:get_lis3dhInfo()
* 功能:三轴数据获取
* 参数:
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void get_lis3dhInfo(float *accX, float *accY, float *accZ)
{
  char accXL, accXH, accYL, accYH, accZL, accZH;
  accXL = lis3dh_readReg(LIS3DH_OUT_X_L);
  accXH = lis3dh_readReg(LIS3DH_OUT_X_H);
  if(accXH & 0x80)
    *accX = (float)(((int)accXH << 4 | (int)accXL >> 4)-4096)/2048*9.8*4;
  else
    *accX = (float)((int)accXH << 4 | (int)accXL >> 4)/2048*9.8*4;
  accYL = lis3dh_readReg(LIS3DH_OUT_Y_L);
  accYH = lis3dh_readReg(LIS3DH_OUT_Y_H);
  if(accYH & 0x80)
    *accY = (float)(((int)accYH << 4 | (int)accYL >> 4)-4096)/2048*9.8*4;
  else
    *accY = (float)((int)accYH << 4 | (int)accYL >> 4)/2048*9.8*4;
  accZL = lis3dh_readReg(LIS3DH_OUT_Z_L);
  accZH = lis3dh_readReg(LIS3DH_OUT_Z_H);
  if(accZH & 0x80)
    *accZ = (float)(((int)accZH << 4 | (int)accZL >> 4)-4096)/2048*9.8*4;
  else
    *accZ = (float)((int)accZH << 4 | (int)accZL >> 4)/2048*9.8*4;
}
