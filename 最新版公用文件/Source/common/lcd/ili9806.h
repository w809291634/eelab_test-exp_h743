#ifndef __ILI9806_H
#define __ILI9806_H
#include "extend.h"

typedef struct  
{
	u16		width;		  //LCD 宽度
	u16		height;		  //LCD 高度
	u16		id;			    //LCD ID
	u8 		dir;			  //横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16		touch_x;		//触摸点横坐标
	u16		touch_y;		//触摸点纵坐标
}_lcd_dev; 

extern u16  POINT_COLOR;    
extern u16  BACK_COLOR;    
extern _lcd_dev lcd_dev;	

/* LCD MPU保护参数 */
#define LCD_REGION_NUMBER       MPU_REGION_NUMBER15      /* LCD使用region0 */
#define LCD_ADDRESS_START       (0X60000000)            /* LCD区的首地址 */
#define LCD_REGION_SIZE         MPU_REGION_SIZE_256MB   /* LCD区大小 */

/* LCD 引脚配置 */
// PD5---LCD-WR---FSMC_NWE
#define LCD_WR_GPIO_PORT                GPIOD
#define LCD_WR_GPIO_PIN                 GPIO_PIN_5
#define LCD_WR_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* 所在IO口时钟使能 */

// PD4---LCD-RD---FSMC_NOE
#define LCD_RD_GPIO_PORT                GPIOD
#define LCD_RD_GPIO_PIN                 GPIO_PIN_4
#define LCD_RD_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* 所在IO口时钟使能 */

// PF5--BL背光
#define LCD_BL_GPIO_PORT                GPIOF
#define LCD_BL_GPIO_PIN                 GPIO_PIN_5
#define LCD_BL_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)     /* 背光所在IO口时钟使能 */

/* LCD背光控制 */
#define LCD_BL(x)   do{ x ? \
                      HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_RESET); \
                     }while(0)

/* LCD_CS(需要根据LCD_FMC_NEX设置正确的IO口) 和 LCD_RS(需要根据LCD_FMC_AX设置正确的IO口) 引脚 定义 */
/* PD7-FSMC_NE1  :LCD-CS */
#define LCD_CS_GPIO_PORT                GPIOD
#define LCD_CS_GPIO_PIN                 GPIO_PIN_7
#define LCD_CS_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* 所在IO口时钟使能 */
/* PD13-FSMC_A18 :LCD-RS */
#define LCD_RS_GPIO_PORT                GPIOD
#define LCD_RS_GPIO_PIN                 GPIO_PIN_13
#define LCD_RS_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* 所在IO口时钟使能 */

/* LCD RS片选控制（命令和数据控制） */
#define Bank1_LCD_C    ((u32)0x60000000)	 	//命令地址
#define Bank1_LCD_D    ((u32)0x60080000)		//数据地址
#define LCD_WIDTH       lcd_dev.width
#define LCD_HEIGHT      lcd_dev.height

//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左	 

#define DFT_SCAN_DIR      L2R_U2D  //默认的扫描方向

//画笔颜色
#define WHITE         	  0xFFFF
#define BLACK             0x0000	  
#define BLUE              0x001F  
#define BRED              0XF81F
#define GRED              0XFFE0
#define GBLUE             0X07FF
#define RED               0xF800
#define MAGENTA           0xF81F
#define GREEN             0x07E0
#define CYAN              0x7FFF
#define YELLOW            0xFFE0
#define BROWN             0XBC40 //棕色
#define BRRED             0XFC07 //棕红色
#define GRAY              0X8430 //灰色
//GUI颜色

#define DARKBLUE          0X01CF	//深蓝色
#define LIGHTBLUE         0X7D7C	//浅蓝色  
#define GRAYBLUE          0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN        0X841F //浅绿色
#define LIGHTGRAY         0XEF5B //浅灰色(PANNEL)
#define LGRAY             0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE         0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE            0X2B12 //浅棕蓝色(选择条目的反色)
	    															  
void TFTLCD_Init(void);													   	//初始化
u16 LCD_ReadPoint(u16 x,u16 y);

void LCD_DisplayOn(void);                                       //开显示
void LCD_DisplayOff(void);                                      //关显示
void LCD_Clear(u16 Color);                                      //清屏
void LCD_SetBackLight(u32 pwm);
void LCD_WriteRAM_Prepare(void);
void LCD_SetCursor(u16 Xpos, u16 Ypos);                         //设置光标
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height);
void LCD_DrawPoint(u16 x,u16 y);                                //画点
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);                 //快速画点
u16  LCD_ReadPoint(u16 x,u16 y);                                //读点 
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);                       //画圆
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);              //画线
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);         //画矩形
void LCD_Fill(u16 x, u16 y, u16 width, u16 height, u16 color);  //填充单色
void LCD_Fill_s(u16 sx, u16 sy, u16 ex, u16 ey, u16 color);
void LCD_FillColor(short x1,short y1,short x2,short y2,uint16_t color);
void LCD_FillData(short x1,short y1,short width,short height,unsigned short* dat);
void LCD_FillData_ex(short x1,short y1,short width,short height,unsigned short* dat,unsigned char multiple);
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);    //填充指定颜色
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);          //显示一个字符
void LCDDrawChar(int x, int y, char num, u8 size,int color, int bc);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);           //显示一个数字
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);  //显示 数字
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);  //显示一个字符串,12/16字体
void LCD_ShowString_Mid(u16 x,u16 y,u16 len,u8 size,char *str);

void LCD_WR_REG(vu16 regval);
void LCD_WR_DATA(vu16 data);
u16 LCD_RD_DATA(void);
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
void LCD_Scan_Dir(u8 dir);                                      //设置屏扫描方向
void LCD_Display_Dir(u8 dir);                                   //设置屏幕显示方向

#endif
