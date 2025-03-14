#include "pcf8563.h"
#include "iic4.h"
#include <stdio.h>

/***********************************************************
* 名称: PCF8563_write_byte()
* 功能: 向8563时钟芯片写入一个字节
* 参数: 无
* 返回: 无
* 修改:
* 注释: addr为写的寄存器地址;dat寄存器要写入的值
***********************************************************/
void PCF8563_write_byte(unsigned char addr,unsigned char dat)
{
  I2C4_Start();                                                 //开启iic
  I2C4_WriteByte(0xA2);                                          //选中PCF8563，并让其做好被写入的准备(PCF8563的写地址为0xA2)
  I2C4_WaitAck();           
  I2C4_WriteByte(addr);                                          //写入要写的寄存器地址
  I2C4_WaitAck();           
  I2C4_WriteByte(dat);                                           //写入要写的数据
  I2C4_WaitAck();           
  I2C4_Stop();                                                  //停止iic
}

/***********************************************************
* 名称: PCF8563_read_byte()
* 功能: 从PCF8563读取一个字节
* 参数: 无
* 返回: 读出的数据
* 修改:
* 注释: addr为要读的地址，函数返回读出的数据
***********************************************************/
unsigned char PCF8563_read_byte(unsigned char addr)
{
  unsigned char dat;
  I2C4_Start();                                               //开启iic
  I2C4_WriteByte(0xA2);                                        //选中PCF8563，并让其做好被写入的准备(PCF8563的写地址为0xA2)
  I2C4_WaitAck();           
  I2C4_WriteByte(addr);                                        //写入要读的地址
  I2C4_WaitAck();           
  I2C4_Start();                                               //重新开始新通讯
  I2C4_WriteByte(0xA3);                                        //PCF8563的读地址为0xA3,准备开始读取
  I2C4_WaitAck();
  dat=I2C4_ReadByte(0);                                     //将读出来的值赋给参数
  I2C4_NAck();                                               //因为只读一个字节，所以不用发送应答信号
  I2C4_Stop();                                                //停止iic
  return(dat);
}

/***********************************************************
* 名称: PCF8563_init()
* 功能: 初始化时钟芯片
* 参数: 无
* 返回: 无
* 修改:
* 注释: 
***********************************************************/
void PCF8563_init(void)
{
  I2C4_IO_Init();
  PCF8563_write_byte(0x00,0x00);                                 //0x00,0x01为控制/状态寄存器地址
  PCF8563_write_byte(0x01,0x12);                                 //脉冲,报警中断有效
  PCF8563_write_byte(0x09,0x80);                                 //关闭分钟报警//  
  PCF8563_write_byte(0x0A,0x80);                                 //关闭小时报警
  PCF8563_write_byte(0x0B,0x80);                                 //关闭日报警
  PCF8563_write_byte(0x0C,0x80);                                 //关闭星期报警
  PCF8563_write_byte(0x0D,0x00);                                 //关闭CLKOUT输出
  PCF8563_write_byte(0x0E,0x03);                                 //关闭定时器
}

/*********************************************************************************************
* 名称: PCF8563_Read_NByte()
* 功能: 从PCF8563读取N个字节
* 参数: 
* 返回: 
* 修改:
* 注释: 
*********************************************************************************************/
unsigned char * PCF8563_Read_NByte(unsigned char addr ,unsigned char len,unsigned char *buf)
{
  unsigned char i;
  I2C4_Start();                
  I2C4_WriteByte(0xA2);       
  if (I2C4_WaitAck())
    return NULL;
  I2C4_WriteByte(addr);        
  if ( I2C4_WaitAck())
    return NULL;
  I2C4_Start();                
  I2C4_WriteByte(0xA3); 
  if (I2C4_WaitAck())
    return NULL;
  for(i=0;i<len-1;i++)
    buf[i]=I2C4_ReadByte(1);
  buf[len-1]=I2C4_ReadByte(0);        
  I2C4_Stop();              
  return buf;
}

int PCF8563_Write_NByte(unsigned char addr, unsigned char len, unsigned char *buf)
{
  unsigned char i = 0;
  I2C4_Start();                
  I2C4_WriteByte(0xA2); 
  if (I2C4_WaitAck())
    return -1;
  I2C4_WriteByte(addr);        
  if ( I2C4_WaitAck())
    return -1;
  for(i=0; i<len; i++)
  {
    I2C4_WriteByte(buf[i]);        
    if ( I2C4_WaitAck())
      return -1;
  }
  I2C4_Stop();
  return i;
}

/*********************************************************************************************
* 名称：bcd_to_hex
* 功能：bcd码转换为16进制数
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
unsigned char bcd_to_hex(unsigned char data)
{
    unsigned char temp;
 
    temp = ((data>>4)*10 + (data&0x0f));
    return temp;
}

/*********************************************************************************************
* 名称：hex_to_bcd
* 功能：16进制数转换为bcd码
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
unsigned char hex_to_bcd(unsigned char data)
{
    unsigned char temp;
 
    temp = (((data/10)<<4) + (data%10));
    return temp;
}

/*********************************************************************************************
* 名称：PCF8563_gettime
* 功能：PCF8563获取时间
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
int PCF8563_gettime(calendar_t *time)
{
  char timeBuf[7] = {0};
  if (PCF8563_Read_NByte(0x02, 7, (unsigned char *)timeBuf))
  {
    time->second = bcd_to_hex(timeBuf[0] & 0x7F);           //低7位有效 SECONDS (0 to 59)
    time->minute = bcd_to_hex(timeBuf[1] & 0x7F);           //低7位有效 MINUTES (0 to 59)
    time->hour = bcd_to_hex(timeBuf[2] & 0x3F);              //低6位有效 HOURS (0 to 23)
    time->day = bcd_to_hex(timeBuf[3] & 0x3F);               //低6位有效 DAYS (1 to 31)
    time->week = bcd_to_hex(timeBuf[4] & 0x07);              //低3位有效 WEEKDAYS (0 to 6)
    time->month = bcd_to_hex(timeBuf[5] & 0x1F);             //低5位有效 MONTHS (1 to 12)
    time->year = bcd_to_hex(timeBuf[6]);                     //8位有效 YEARS (0 to 99)
    return 1;
  }
  return 0;
}

/*********************************************************************************************
* 名称：CountWeek
* 功能：根据时间计算星期
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
char CountWeek(int iY, int iM, int iD) 
{
  int iWeekDay = -1; 
  if (1 == iM || 2 == iM) 
  {   
    iM += 12; 
    iY--;
  }   
  iWeekDay = (iD + 1 + 2 * iM + 3 * (iM + 1) / 5 + iY + iY / 4 - iY / 100 + iY / 400) % 7;
  return iWeekDay;
}

/*********************************************************************************************
* 名称: PCF8563_settime()
* 功能: 设置时间,星期自动计算
* 参数: 
* 返回: 
* 修改:
* 注释: 
*********************************************************************************************/
int PCF8563_settime(calendar_t *time)
{
  char timeBuf[7] = {0};
  if( time->year>99 ||
      time->month<1 || time->month>12 ||
      time->week>6 ||
      time->day<1 || time->day>31 ||
      time->hour>23 ||
      time->minute>59 ||
      time->second>59) return 0;

  time->week = CountWeek(time->year+2000, time->month, time->day);
  timeBuf[0] = hex_to_bcd(time->second);
  timeBuf[1] = hex_to_bcd(time->minute); 
  timeBuf[2] = hex_to_bcd(time->hour); 
  timeBuf[3] = hex_to_bcd(time->day);
  timeBuf[4] = hex_to_bcd(time->week); 
  timeBuf[5] = hex_to_bcd(time->month);
  timeBuf[6] = hex_to_bcd(time->year); 
  PCF8563_Write_NByte(0x02, 7, (unsigned char*)timeBuf);
  return 1;
}
