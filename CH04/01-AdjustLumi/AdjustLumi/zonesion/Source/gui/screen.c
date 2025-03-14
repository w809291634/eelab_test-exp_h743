/*********************************************************************************************
* 文件: screen.c
* 作者：zonesion 2016.12.22
* 说明：
* 功能：屏幕驱动代码
* 修改：wanghao  增加相关宏定义，方便移植，增加数字枚举，增加图像放大接口      2024.2.2
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "screen.h"
#include "ili9806.h"

/*********************************************************************************************
* 屏幕显示存储数组全局变量
* 以下图片C文件使用emwin BmpCvtST.exe软件生成，打开后保存为C格式，
* 注意:
*       1、选择输出格式类型为：High color (565] red and blue swapped
*       2、需要修改输出的生成的C文件，做部分修改，每个图片数据数据数组中
*       前两个元素表示图片像素宽度和高度，需要自己手动补充。
*********************************************************************************************/
extern uint16_t _aclibrary_logo[];
extern uint16_t _acmode_handler_on[];
extern uint16_t _acmode_handler_off[];
extern uint16_t _acmode_smart_on[];
extern uint16_t _acmode_smart_off[];
extern uint16_t _acnum_0[];
extern uint16_t _acnum_1[];
extern uint16_t _acnum_2[];
extern uint16_t _acnum_3[];
extern uint16_t _acnum_4[];
extern uint16_t _acnum_5[];
extern uint16_t _acnum_6[];
extern uint16_t _acnum_7[];
extern uint16_t _acnum_8[];
extern uint16_t _acnum_9[];
extern uint16_t _acLX[]; 
extern uint16_t _acKLX[];
extern uint16_t _acfill_light_0[];
extern uint16_t _acfill_light_1[];
extern uint16_t _acfill_light_2[];
extern uint16_t _acfill_light_3[];
extern uint16_t _acfill_light_4[];

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define LIBRARY_LOGO_X            ((int)(LCD_WIDTH*0.8f))       // 图书馆图标起始坐标X
#define LIBRARY_LOGO_Y            ((int)(LCD_HEIGHT*0.1f))
  
#define LX_UNIT_X                 ((int)(LCD_WIDTH*0.85f))      // 光照度单位图标起始坐标X
#define LX_UNIT_Y                 ((int)(LCD_HEIGHT*0.55f))
  
#define FILL_LIGHT_X              ((int)(LCD_WIDTH*0.1f))       // 亮度图标起始坐标X
#define FILL_LIGHT_Y              ((int)(LCD_HEIGHT*0.1f))
  
#define POSTNUM_X                 ((int)(LCD_WIDTH*0.6f))       // 数字显示起始坐标X
#define POSTNUM_Y                 ((int)(LCD_HEIGHT*0.5f))
#define MAGNIFICATION             2                             // 放大倍数
#define POST_NUM_WIDTH            (21*MAGNIFICATION)            // 一个数字占用的大致像素间隔，包含数字间的间隔

#define MODE_MANUAL_X             ((int)(LCD_WIDTH*0.15f))      // 手动模式起始坐标X
#define MODE_MANUAL_Y             ((int)(LCD_HEIGHT*0.8f))
#define MODE_AUTO_X               ((int)(LCD_WIDTH*0.65f))      // 自动模式起始坐标X
#define MODE_AUTO_Y               ((int)(LCD_HEIGHT*0.8f))

/*********************************************************************************************
* 枚举
*********************************************************************************************/
typedef enum {                                                  // 数值
  POST_NUM_0 = 0,
  POST_NUM_1,
  POST_NUM_2,
  POST_NUM_3,
  POST_NUM_4,
  POST_NUM_5,
  POST_NUM_6,
  POST_NUM_7,
  POST_NUM_8,
  POST_NUM_9,
  POST_NONE,
}post_num;

typedef enum {                                                  // 计量单位
  INDIVIDUAL = 1,
  TEN,
  HUNDRED,
  THOUSAND,
  MYRIAD,
}post_unit;

/*********************************************************************************************
* 名称:show_logo
* 功能:显示画面中的静态图标
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void show_logo(void)
{
  LCD_FillData_ex(LIBRARY_LOGO_X, LIBRARY_LOGO_Y, _aclibrary_logo[0], _aclibrary_logo[1], _aclibrary_logo,2);
  LCD_FillData_ex(LX_UNIT_X, LX_UNIT_Y, _acLX[0], _acLX[1], _acLX,2);
}

/*********************************************************************************************
* 名称:show_fill_light
* 功能:显示画面中的fill_light图标
* 参数:status 图标的索引
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void show_fill_light(uint8_t status)
{
  switch(status){
  case 0:
    LCD_FillData_ex(FILL_LIGHT_X,FILL_LIGHT_Y,_acfill_light_0[0],_acfill_light_0[1], _acfill_light_0,2);
    break;
  case 1:
    LCD_FillData_ex(FILL_LIGHT_X,FILL_LIGHT_Y,_acfill_light_1[0],_acfill_light_1[1], _acfill_light_1,2);
    break;
  case 2:
    LCD_FillData_ex(FILL_LIGHT_X,FILL_LIGHT_Y,_acfill_light_2[0],_acfill_light_2[1], _acfill_light_2,2);
    break;
  case 3:
    LCD_FillData_ex(FILL_LIGHT_X,FILL_LIGHT_Y,_acfill_light_3[0],_acfill_light_3[1], _acfill_light_3,2);
    break;
  case 4:
    LCD_FillData_ex(FILL_LIGHT_X,FILL_LIGHT_Y,_acfill_light_4[0],_acfill_light_4[1], _acfill_light_4,2);
    break;
  }
}

/*********************************************************************************************
* 名称:show_post_num
* 功能:在指定计量单位位置上显示一个数值
* 参数:post     显示位置，比如个位，十位
*      x        显示起始位置
*      y        显示起始位置
*      num      具体数值
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
static void show_post_num(post_unit post, uint16_t x, uint16_t y, post_num num)
{
  /* 计算当前数字在LCD的显示位置 */
  uint16_t dx = x + (5 - post) * POST_NUM_WIDTH;
  uint16_t dy = y;
  /* 根据图像的像素值和放大倍数，清理数字显示区域 */
  LCD_FillColor(dx, dy-1, dx + 19*MAGNIFICATION, dy + 32*MAGNIFICATION, BLACK);
  switch(num){
  case POST_NUM_0:
    LCD_FillData_ex(dx, dy,_acnum_0[0],_acnum_0[1], _acnum_0,MAGNIFICATION);
    break;
  case POST_NUM_1:
    LCD_FillData_ex(dx + 15*MAGNIFICATION, y, _acnum_1[0],_acnum_1[1], _acnum_1,MAGNIFICATION);
    break;
  case POST_NUM_2:
    LCD_FillData_ex(dx, dy, _acnum_2[0],_acnum_2[1], _acnum_2,MAGNIFICATION);
    break;
  case POST_NUM_3:
    LCD_FillData_ex(dx + 1*MAGNIFICATION, dy, _acnum_3[0],_acnum_3[1], _acnum_3,MAGNIFICATION);
    break;
  case POST_NUM_4:
    LCD_FillData_ex(dx, dy, _acnum_4[0],_acnum_4[1], _acnum_4,MAGNIFICATION);
    break;
  case POST_NUM_5:
    LCD_FillData_ex(dx + 1*MAGNIFICATION, dy,_acnum_5[0],_acnum_5[1], _acnum_5,MAGNIFICATION);
    break;
  case POST_NUM_6:
    LCD_FillData_ex(dx, dy,_acnum_6[0],_acnum_6[1], _acnum_6,MAGNIFICATION);
    break;
  case POST_NUM_7:
    LCD_FillData_ex(dx, dy,_acnum_7[0],_acnum_7[1], _acnum_7,MAGNIFICATION);
    break;
  case POST_NUM_8:
    LCD_FillData_ex(dx, dy,_acnum_8[0],_acnum_8[1], _acnum_8,MAGNIFICATION);
    break;
  case POST_NUM_9:
    LCD_FillData_ex(dx, dy,_acnum_9[0],_acnum_9[1], _acnum_9,MAGNIFICATION);
    break;
  /* 错误数字不进行显示 */
  default:
    break;
  }
}

/*********************************************************************************************
* 名称:show_value
* 功能:在LCD上显示一串数字
* 参数:value    要显示的具体数值
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void show_value(uint16_t value)
{
  /* 实现为9，个位显示0，其他不显示 */
  if(value == 0){
    show_post_num(INDIVIDUAL, POSTNUM_X, POSTNUM_Y, POST_NUM_0);
    show_post_num(TEN, POSTNUM_X, POSTNUM_Y, POST_NONE);
    show_post_num(HUNDRED, POSTNUM_X, POSTNUM_Y, POST_NONE);
    show_post_num(THOUSAND, POSTNUM_X, POSTNUM_Y, POST_NONE);
    show_post_num(MYRIAD, POSTNUM_X, POSTNUM_Y, POST_NONE);
    return;
  }
  /* 个位 */
  if(value) show_post_num(INDIVIDUAL, POSTNUM_X, POSTNUM_Y, (post_num)(value % 10));
  else show_post_num(INDIVIDUAL, POSTNUM_X, POSTNUM_Y, POST_NONE);
  /* 十位 */
  if(value / 10) show_post_num(TEN, POSTNUM_X, POSTNUM_Y, (post_num)((value % 100)/10));
  else show_post_num(TEN, POSTNUM_X, POSTNUM_Y, POST_NONE);
  /* 百位 */
  if(value / 100) show_post_num(HUNDRED, POSTNUM_X, POSTNUM_Y, (post_num)((value % 1000)/100));
  else show_post_num(HUNDRED, POSTNUM_X, POSTNUM_Y, POST_NONE);
  /* 千位 */
  if(value / 1000) show_post_num(THOUSAND, POSTNUM_X, POSTNUM_Y, (post_num)((value % 10000)/1000));
  else show_post_num(THOUSAND, POSTNUM_X, POSTNUM_Y, POST_NONE);
  /* 万位 */
  if(value / 10000) show_post_num(MYRIAD, POSTNUM_X, POSTNUM_Y, (post_num)(value / 10000));
  else show_post_num(MYRIAD, POSTNUM_X, POSTNUM_Y, POST_NONE);
}

/*********************************************************************************************
* 名称:show_mode
* 功能:在LCD上显示当前模式
* 参数:status     当前模式
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void show_mode(uint8_t status)
{
  if(status){
    /* 显示手动模式 */
    LCD_FillData_ex(MODE_MANUAL_X, MODE_MANUAL_Y, _acmode_handler_on[0], _acmode_handler_on[1], _acmode_handler_on,2);
    LCD_FillData_ex(MODE_AUTO_X, MODE_AUTO_Y,_acmode_smart_off[0], _acmode_smart_off[1], _acmode_smart_off,2);
  }else{
    /* 显示自动模式 */
    LCD_FillData_ex(MODE_MANUAL_X, MODE_MANUAL_Y, _acmode_handler_off[0], _acmode_handler_off[1], _acmode_handler_off,2);
    LCD_FillData_ex(MODE_AUTO_X, MODE_AUTO_Y,_acmode_smart_on[0], _acmode_smart_on[1], _acmode_smart_on,2);
  }
}