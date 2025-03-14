#include "text.h"
#include "font_user.h"
#include "string.h"
#include "stdio.h"

int GB_40_SIZE = sizeof(GB_40) / sizeof(struct typFNT_GB40);
font_info_t font_info;
 
/*********************************************************************************************
* 名称：LCD_Write_GRAM_BlitLine
* 功能：LCD 使用自动填充 绘制一条两色线
* 参数：x,y 起点  x,y 起点  
           dot 单个字符的行指针 len 字节数
           color 前景色  bc 背景色
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
static void LCD_Write_GRAM_BlitLine(u16 x, u16 y, u8* dot, u16 len, u16 color ,u16 bc)
{
	LCD_WR_REG(0X2A);		  /* column address control set */
	LCD_WR_DATA(x>>8);	  /* 先高8位，然后低8位 */
	LCD_WR_DATA(x&0xff);	/* 设置横向 起始点和结束点*/
	LCD_WR_DATA((lcd_dev.width) >>8);
	LCD_WR_DATA((lcd_dev.width) &0xff);
  
	LCD_WR_REG(0X2B);		  /* page address control set */
	LCD_WR_DATA(y>>8);
	LCD_WR_DATA(y&0xff);	
	LCD_WR_DATA((lcd_dev.height) >>8);
	LCD_WR_DATA((lcd_dev.height) &0xff);
  
  LCD_WR_REG(0x2C);
  for(int i=0;i<len;i++){
    for(int t=0;t<8;t++){
      if (*dot & 1<<(7-t)) LCD_WR_DATA(color);
      else if (bc > 0) LCD_WR_DATA(bc);
    }
    dot ++;
  }
}

/*********************************************************************************************
* 名称：font_lib_init
* 功能：初始化字库标志
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void font_lib_init(void)
{
  W25QXX_Init();
  READ_FON_INFO;
  if(font_info.font_flag!=FONT_OK){
    printf("Font library error\r\n");
    while(1);
  }
}

/*********************************************************************************************
* 名称：Get_HzMat
* 功能：code 字符指针开始
* 参数：code 字符串的开始地址,GBK码
            mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小
            size:字体大小	
* 返回：无
* 修改：无
* 注释：从 GB2312 字库中查找出字模
*********************************************************************************************/
static void Get_HzMat(u8 *code,u8 *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
  if(size!=32){
    printf("Get_HzMat no size\r\n");
    return;
  }
  
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
  qh=*code;
  ql=*(++code);
	if(qh<0xa1||ql<0xa1||ql==0xff||qh==0xff)//非 GB2312 常用汉字
	{   		    
	    for(i=0;i<csize;i++)*mat++=0x00;    //填充空白
	    return; //结束访问
	}          
  ql -= 0xa1;
  qh -= 0xa1;
  foffset = ((unsigned long)94*qh + ql)*csize;
	switch(size)
	{
		case 32:
      W25QXX_Read(mat,FONT_LIB_STORE_ADD+foffset,csize);
			break;
	}
}

/*********************************************************************************************
* 名称：LCDDrawGB2312
* 功能：绘制一个GB2312汉字，暂仅支持32号字体
* 参数：font 具体的文字指针 size 字号
* 返回：无
* 修改：无
* 注释：具体能够绘制的汉字查询 GB2312 字库表
*********************************************************************************************/
static void LCDDrawGB2312(int x, int y, char* font, u8 size, int color, int bc)
{
  u8 dzk[128];
  
  if(size!=32){
    printf("LCDDrawGB2312_32 no size\r\n");
    return;
  }
  Get_HzMat((u8*)font,dzk,size);	//得到相应大小的点阵数据 
  
  /* 取字体 */
	for(int t=0;t<size;t++)
	{   												   
    LCD_Write_GRAM_BlitLine(x,y+t,dzk+4*t,4,color,bc);
	}
}

/*********************************************************************************************
* 名称：LCDShowFont32
* 功能：lcd显示32x32汉字（文件字库版）
* 参数：x1,y1:起始坐标值
            p 字符  width绘制宽度
            color：字体颜色，Bcolor：背景颜色
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void LCDShowFont32(u16 x,u16 y,char* str,u16 width,u16 color,u16 Bcolor)
{
  u8 size=32;         // 定义字符栅格大小
  u16 x0=x;
  
  while(*str!=0)    // 数据未结束
  { 
    /* 字符 宽度占用一半size */
    if(*str<0x80)
    {
      // 当前字符为换行字符
      if(*str=='\r'){
        str++; 
        continue;
      }
      if(*str=='\n'){   //换行符号
        x=x0;
        y=y+size+10;    // 增加了10的行间距
        //y+=size;
        str++; 
        continue;
      }  
      // 当前字符超出,继续显示
      if(x>(x0+width-size/2)){
        y=y+size+10;    // 增加了10的行间距
        //y+=size;
        x=x0;	   
      }							    
      // if(y>(y0+height-size))break;   //越界返回    
      LCDDrawChar(x, y, *str,size, color, Bcolor);//有效部分写入 
      str++; 
      x+=size/2;      //字符,为全字的一半 
    }
    /* 中文 */
    else
    {     
      if(x>(x0+width-size))//换行
      {	    
        y=y+size+10;
        //y+=size;
        x=x0;		  
      }
      // if(y>(y0+height-size))break;//越界返回  						     
      LCDDrawGB2312(x,y,str,size,color,Bcolor);
      str+=2; 
      x+=size;//下一个汉字偏移	    
    }						 
  }
}

/*********************************************************************************************
* 名称：LCDDrawFont32_Mid
* 功能：在指定宽度的中间显示文本
* 参数：len:指定要显示的宽度
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void LCDDrawFont32_Mid(u16 x,u16 y,char *str,u16 len,int color, int Bcolor)
{
  u8 str_size=16;         // 定义字符串栅格大小
  u8 hz_size=32;          // 定义汉字栅格大小
  u16 size;
  
  // 获取多少个汉字和字符
  char *p=str;
  u8 str_num=0;
  u8 hz_num=0;
  while(*p!=0){ 
    if(*p<0x80){
      p++;
      str_num++;
    }
    /* 中文 */
    else{
      p+=2; 
      hz_num++;
    }						 
  }
  size = str_size*str_num + hz_size*hz_num;     // 当前显示文本占用像素
 
  if(size>len){
    printf("LCDDrawFont32_Mid parameter error!\r\n");
    return;
  }
  size=(len-size)/2;                            // 起点偏移量
  LCDShowFont32(size+x,y,str,len,color,Bcolor);
} 

/*********************************************************************************************
* 名称：LCDDraw_User40
* 功能：绘制一个自定义的40号字体
* 参数：无
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void LCDDraw_User40(u16 x, u16 y, char *gb2, u16 color, u16 Bcolor)
{
  u8 size=40;         // 定义字符栅格大小
  char ok=0;
  
  for (int i=0; i<GB_40_SIZE; i++){
    /* 取字体 */
    if (gb2[0] == GB_40[i].Index[0] && gb2[1] == GB_40[i].Index[1]){
      /* 取字体 */
      char* dot = GB_40[i].Msk;
      ok=1;
      for(int t=0;t<size;t++)// 多少行
      {   												   
        LCD_Write_GRAM_BlitLine(x,y+t,(u8*)dot+size/8*t,size/8,color,Bcolor);
      }
    }
  }
  // 没有在自定义字库中找到，填充背景
  if(ok==0){
    LCD_Fill(x,y,size,size,Bcolor);
  }
}

/*********************************************************************************************
* 名称：LCDDrawFont40
* 功能：lcd显示40x40汉字（自定义字库）  字符使用字符库
* 参数：x1,y1:起始坐标值
            gb2 字符
            color：字体颜色，Bcolor：背景颜色
* 返回：无
* 修改：无
* 注释：仅绘制自定义字库内容
            不具备换行功能
*********************************************************************************************/
void LCDDrawFont40(u16 x, u16 y, char *str, u16 color, u16 Bcolor)
{
  u8 size=40;         // 定义字符栅格大小
  
  while(*str!=0)    // 数据未结束
  { 
    /* 字符 宽度占用一半size */
    if(*str<0x80)
    {
      LCDDrawChar(x, y, *str,size, color, Bcolor);//有效部分写入 
      str++; 
      x+=size/2;      //字符,为全字的一半 
    }
    /* 中文 */
    else
    {
      LCDDraw_User40(x,y,str,color,Bcolor);
      str+=2; 
      x+=size;//下一个汉字偏移	    
    }						 
  }
}

/*********************************************************************************************
* 名称：LCDDrawFont40_Mid
* 功能：在指定宽度的中间显示文本
* 参数：len:指定要显示的宽度
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void LCDDrawFont40_Mid(u16 x,u16 y,char *str,u16 len,int color, int Bcolor)
{
  u8 str_size=20;         // 定义字符串栅格大小
  u8 hz_size=40;          // 定义汉字栅格大小
  u16 size;
  
  // 获取多少个汉字和字符
  char *p=str;
  u8 str_num=0;
  u8 hz_num=0;
  while(*p!=0){ 
    if(*p<0x80){
      p++;
      str_num++;
    }
    /* 中文 */
    else{
      p+=2; 
      hz_num++;
    }						 
  }
  size = str_size*str_num + hz_size*hz_num;     // 当前显示文本占用像素
 
  if(size>len){
    printf("LCDDrawFont40_Mid parameter error!\r\n");
    return;
  }
  size=(len-size)/2;                            // 起点偏移量
  LCDDrawFont40(size+x,y,str,color,Bcolor);
}   
