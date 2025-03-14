/*********************************************************************************************
* 文件：screen.c
* 作者：Liyw 2018.1.22
* 说明：
* 功能：屏幕驱动代码
* 修改：wanghao 增加注释；增加图标位置的宏，让其跟LCD尺寸相关，增加图标放大的计算方法 2024.2.5
* 注释：
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
extern uint16_t _actotal_switch_on[];
extern uint16_t _actotal_switch_off[];
extern uint16_t _acwashing_logo[];
extern uint16_t _acsoaking[];
extern uint16_t _acwashing[];
extern uint16_t _acrinseing[];
extern uint16_t _acdehydration[];
extern uint16_t _acall_mode[];
extern uint16_t _acs_num_0[];
extern uint16_t _acs_num_1[];
extern uint16_t _acs_num_2[];
extern uint16_t _acs_num_3[];
extern uint16_t _acs_num_4[];
extern uint16_t _acs_num_5[];
extern uint16_t _acs_num_6[];
extern uint16_t _acs_num_7[];
extern uint16_t _acs_num_8[];
extern uint16_t _acs_num_9[];
extern uint16_t _acb_num_0[];
extern uint16_t _acb_num_1[];
extern uint16_t _acb_num_2[];
extern uint16_t _acb_num_3[];
extern uint16_t _acb_num_4[];
extern uint16_t _acb_num_5[];
extern uint16_t _acb_num_6[];
extern uint16_t _acb_num_7[];
extern uint16_t _acb_num_8[];
extern uint16_t _acb_num_9[];
extern uint16_t _acb_point[]; 
extern uint16_t _acsoaking_1[];
extern uint16_t _acwashing_1[];
extern uint16_t _acrinseing_1[];
extern uint16_t _acdehydration_1[];
extern uint16_t _acall_mode_1[];
extern uint16_t _acamount_of_water[];
extern uint16_t _acunit[];

/*********************************************************************************************
* 定义图标显示位置
*********************************************************************************************/
/* 放大倍数 */
#define MAGNIFICATION       2                                   //图标的放大倍数

/* 固定图标显示位置 */
#define WASHING_LOGO_X      ((int)(LCD_WIDTH*0.8f))             //配置应用LOGO图标X坐标
#define WASHING_LOGO_Y      ((int)(LCD_HEIGHT*0.0625f))

#define ICON_WIDTH          ((int)(28*MAGNIFICATION))           //28表示图标的像素宽度
#define ICON_SPACING        ((int)((LCD_WIDTH-5*ICON_WIDTH)/6)) //自动计算功能图标之间的间隔，5个图标，6个间隔
#define SOAKING_1_X         ((int)(ICON_WIDTH*0+ICON_SPACING*1))//配置"浸泡"中文图标X坐标
#define SOAKING_1_Y         ((int)(LCD_HEIGHT*0.73f))

#define WASHING_1_X         ((int)(ICON_WIDTH*1+ICON_SPACING*2))//配置"洗涤"中文图标X坐标
#define WASHING_1_Y         SOAKING_1_Y

#define RINSEING_1_X        ((int)(ICON_WIDTH*2+ICON_SPACING*3))//配置"清洗"中文图标X坐标
#define RINSEING_1_Y        SOAKING_1_Y

#define DEHYDRATION_1_X     ((int)(ICON_WIDTH*3+ICON_SPACING*4))//配置"脱水"中文图标X坐标
#define DEHYDRATION_1_Y     SOAKING_1_Y

#define ALLMODE_1_X         ((int)(ICON_WIDTH*4+ICON_SPACING*5-26))//配置"全应用"中文图标X坐标
#define ALLMODE_1_Y         SOAKING_1_Y

#define AMOUNT_OF_WATER_X   ((int)(LCD_WIDTH*0.73f))            //配置"水量"中文图标X坐标
#define AMOUNT_OF_WATER_Y   ((int)(LCD_HEIGHT*0.86f))

#define UNIT_X              ((int)(LCD_WIDTH*0.91f))            //配置单位"L"图标X坐标
#define UNIT_Y              AMOUNT_OF_WATER_Y

/* 模式动态图标显示位置 */
#define TOTAL_SWITCH_X      ((int)(LCD_WIDTH*0.03f))            //配置开关图标X坐标
#define TOTAL_SWITCH_Y      ((int)(LCD_HEIGHT*0.03f))

#define SOAKING_X           SOAKING_1_X                         //配置"浸泡"图标X坐标
#define SOAKING_Y           (SOAKING_1_Y-35*MAGNIFICATION)

#define WASHING_X           WASHING_1_X                         //配置"洗涤"图标X坐标
#define WASHING_Y           SOAKING_Y-15

#define RINSEING_X          RINSEING_1_X                        //配置"清洗"图标X坐标
#define RINSEING_Y          SOAKING_Y

#define DEHYDRATION_X       DEHYDRATION_1_X                     //配置"脱水"图标X坐标
#define DEHYDRATION_Y       SOAKING_Y+10

#define ALLMODE_X           ((int)(ICON_WIDTH*4+ICON_SPACING*5))//配置"全功能"图标X坐标
#define ALLMODE_Y           SOAKING_Y

/* 时间和液位动态图标显示位置 */
#define TIMER_H_X           ((int)(LCD_WIDTH*0.3f))             //配置时间"小时"图标X坐标
#define TIMER_H_Y           ((int)(LCD_HEIGHT*0.241f))

#define TIMER_MH_X          ((int)(LCD_WIDTH*0.49f))            //配置时间"分钟"的高位图标X坐标
#define TIMER_MH_Y          TIMER_H_Y

#define TIMER_ML_X          ((int)(LCD_WIDTH*0.625f))           //配置时间"分钟"的低位图标X坐标
#define TIMER_ML_Y          TIMER_H_Y

#define TIMER_POINT_X       ((int)(LCD_WIDTH*0.43f))            //配置时间"点"的图标X坐标
#define TIMER_POINT_Y       ((int)(LCD_HEIGHT*0.275f))

#define LEVEL_NUM_H_X       ((int)(LCD_WIDTH*0.82f))            //配置水量高位图标X坐标
#define LEVEL_NUM_H_Y       AMOUNT_OF_WATER_Y

#define LEVEL_NUM_L_X       ((int)(LCD_WIDTH*0.87f))            //配置水量低位图标X坐标
#define LEVEL_NUM_L_Y       LEVEL_NUM_H_Y

/*********************************************************************************************
* 名称：show_logo
* 功能：显示登录界面
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void show_logo(void)
{
  /* 显示logo图标 */
  LCD_FillData_ex(WASHING_LOGO_X, WASHING_LOGO_Y, _acwashing_logo[0], _acwashing_logo[1], _acwashing_logo ,MAGNIFICATION);

  /* 显示功能中文 */
  LCD_FillData_ex(SOAKING_1_X, SOAKING_1_Y, _acsoaking_1[0], _acsoaking_1[1], _acsoaking_1 ,MAGNIFICATION);
  LCD_FillData_ex(WASHING_1_X, WASHING_1_Y, _acwashing_1[0], _acwashing_1[1], _acwashing_1 ,MAGNIFICATION);
  LCD_FillData_ex(RINSEING_1_X, RINSEING_1_Y, _acrinseing_1[0], _acrinseing_1[1], _acrinseing_1 ,MAGNIFICATION);
  LCD_FillData_ex(DEHYDRATION_1_X, DEHYDRATION_1_Y, _acdehydration_1[0], _acdehydration_1[1], _acdehydration_1 ,MAGNIFICATION);
  LCD_FillData_ex(ALLMODE_1_X, ALLMODE_1_Y, _acall_mode_1[0], _acall_mode_1[1], _acall_mode_1 ,MAGNIFICATION);

  /* 显示水量和单位图标 */
  LCD_FillData_ex(AMOUNT_OF_WATER_X, AMOUNT_OF_WATER_Y, _acamount_of_water[0], _acamount_of_water[1], _acamount_of_water ,MAGNIFICATION);
  LCD_FillData_ex(UNIT_X, UNIT_Y, _acunit[0], _acunit[1], _acunit ,MAGNIFICATION);
}

/*********************************************************************************************
* 名称：show_switch
* 功能：显示开关状态
* 参数：status 状态
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void show_switch(uint8_t status)
{
  if(!status) LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y, _actotal_switch_on[0], _actotal_switch_on[1], _actotal_switch_on, MAGNIFICATION);
  else LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y,  _actotal_switch_off[0], _actotal_switch_off[1], _actotal_switch_off, MAGNIFICATION);
}

/*********************************************************************************************
* 名称：show_soak()
* 功能：显示浸泡图标
* 参数：status 显示状态
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void show_soak(uint8_t status)
{
  uint16_t x, y;
  x = SOAKING_X;
  y = SOAKING_Y;
  if(!status) LCD_FillData_ex(x, y, _acsoaking[0], _acsoaking[1], _acsoaking, MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acsoaking[0]*MAGNIFICATION, y + _acsoaking[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* 名称：show_wash()
* 功能：显示洗涤图标
* 参数：status 显示状态
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void show_wash(uint8_t status)
{
  uint16_t x, y;
  x = WASHING_X;
  y = WASHING_Y;
  if(!status) LCD_FillData_ex(x, y, _acwashing[0], _acwashing[1], _acwashing, MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acwashing[0]*MAGNIFICATION, y + _acwashing[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* 名称：show_rinse()
* 功能：显示清洗图标
* 参数：status 显示状态
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void show_rinse(uint8_t status)
{
  uint16_t x, y;
  x = RINSEING_X;
  y = RINSEING_Y;
  if(!status) LCD_FillData_ex(x, y, _acrinseing[0], _acrinseing[1], _acrinseing, MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acrinseing[0]*MAGNIFICATION, y + _acrinseing[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* 名称：show_dehyd()
* 功能：显示脱水图标
* 参数：status 显示状态
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void show_dehyd(uint8_t status)
{
  uint16_t x, y;
  x = DEHYDRATION_X;
  y = DEHYDRATION_Y;
  if(!status) LCD_FillData_ex(x, y, _acdehydration[0], _acdehydration[1], _acdehydration, MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acdehydration[0]*MAGNIFICATION, y + _acdehydration[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* 名称：show_all_mode()
* 功能：显示全功能图标
* 参数：status 显示状态
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void show_all_mode(uint8_t status)
{
  uint16_t x, y;
  x = ALLMODE_X;
  y = ALLMODE_Y;
  if(!status) LCD_FillData_ex(x, y, _acall_mode[0], _acall_mode[1], _acall_mode, MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acall_mode[0]*MAGNIFICATION, y + _acall_mode[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* 名称：show_b_num
* 功能：显示大号的数字
* 参数：x：显示的X起始坐标 y：显示的Y起始坐标 num:要显示的数值
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void show_b_num(uint16_t x, uint16_t y, uint8_t num)
{
  LCD_FillColor(x, y, x + 31*MAGNIFICATION, y + 52*MAGNIFICATION, BLACK);
  switch(num){
  case 0:
    LCD_FillData_ex(x, y, _acb_num_0[0], _acb_num_0[1], _acb_num_0 ,MAGNIFICATION);
    break;
  case 1:
    LCD_FillData_ex(x + 23*MAGNIFICATION, y, _acb_num_1[0], _acb_num_1[1], _acb_num_1 ,MAGNIFICATION);
    break;
  case 2:
    LCD_FillData_ex(x, y, _acb_num_2[0], _acb_num_2[1], _acb_num_2 ,MAGNIFICATION);
    break;
  case 3:
    LCD_FillData_ex(x, y, _acb_num_3[0], _acb_num_3[1], _acb_num_3 ,MAGNIFICATION);
    break;
  case 4:
    LCD_FillData_ex(x, y, _acb_num_4[0], _acb_num_4[1], _acb_num_4 ,MAGNIFICATION);
    break;
  case 5:
    LCD_FillData_ex(x, y, _acb_num_5[0], _acb_num_5[1], _acb_num_5 ,MAGNIFICATION);
    break;
  case 6:
    LCD_FillData_ex(x, y, _acb_num_6[0], _acb_num_6[1], _acb_num_6 ,MAGNIFICATION);
    break;
  case 7:
    LCD_FillData_ex(x, y, _acb_num_7[0], _acb_num_7[1], _acb_num_7 ,MAGNIFICATION);
    break;
  case 8:
    LCD_FillData_ex(x, y, _acb_num_8[0], _acb_num_8[1], _acb_num_8 ,MAGNIFICATION);
    break;
  case 9:
    LCD_FillData_ex(x, y, _acb_num_9[0], _acb_num_9[1], _acb_num_9 ,MAGNIFICATION);
    break;
  }
}

/*********************************************************************************************
* 名称：show_s_num
* 功能：显示小号的数字
* 参数：x：显示的X起始坐标 y：显示的Y起始坐标 num:要显示的数值
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void show_s_num(uint16_t x, uint16_t y, uint8_t num)
{
  LCD_FillColor(x, y, x + 13*MAGNIFICATION, y + 20*MAGNIFICATION, BLACK);
  switch(num){
  case 0:
    LCD_FillData_ex(x, y, _acs_num_0[0], _acs_num_0[1], _acs_num_0 ,MAGNIFICATION);
    break;
  case 1:
    LCD_FillData_ex(x + 9*MAGNIFICATION, y, _acs_num_1[0], _acs_num_1[1], _acs_num_1 ,MAGNIFICATION);
    break;
  case 2:
    LCD_FillData_ex(x, y, _acs_num_2[0], _acs_num_2[1], _acs_num_2 ,MAGNIFICATION);
    break;
  case 3:
    LCD_FillData_ex(x, y, _acs_num_3[0], _acs_num_3[1], _acs_num_3 ,MAGNIFICATION);
    break;
  case 4:
    LCD_FillData_ex(x, y, _acs_num_4[0], _acs_num_4[1], _acs_num_4 ,MAGNIFICATION);
    break;
  case 5:
    LCD_FillData_ex(x, y, _acs_num_5[0], _acs_num_5[1], _acs_num_5 ,MAGNIFICATION);
    break;
  case 6:
    LCD_FillData_ex(x, y, _acs_num_6[0], _acs_num_6[1], _acs_num_6 ,MAGNIFICATION);
    break;
  case 7:
    LCD_FillData_ex(x, y, _acs_num_7[0], _acs_num_7[1], _acs_num_7 ,MAGNIFICATION);
    break;
  case 8:
    LCD_FillData_ex(x, y, _acs_num_8[0], _acs_num_8[1], _acs_num_8 ,MAGNIFICATION);
    break;
  case 9:
    LCD_FillData_ex(x, y, _acs_num_9[0], _acs_num_9[1], _acs_num_9 ,MAGNIFICATION);
    break;
  }
}

/*********************************************************************************************
* 名称：show_timer
* 功能：显示剩余时间
* 参数：hours：时 minute：分
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void show_timer(uint8_t hours, uint8_t minute)
{
  uint8_t mh, ml;
  static uint8_t l_h, l_mh, l_ml;
  
  mh = minute / 10;
  ml = minute % 10;
  
  if((l_h != hours) || (hours == 0)) show_b_num(TIMER_H_X, TIMER_H_Y, hours);
  if((l_mh != mh) || (mh == 0))show_b_num(TIMER_MH_X, TIMER_MH_Y, mh);
  if((l_ml != ml) || (ml == 0))show_b_num(TIMER_ML_X, TIMER_ML_Y, ml);
  
  l_h = hours;
  l_mh = mh;
  l_ml = ml;
}

/*********************************************************************************************
* 名称：show_point
* 功能：实现时和分之间的点
* 参数：status 显示有无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void show_point(uint8_t status)
{
  uint16_t x, y;
  x = TIMER_POINT_X;
  y = TIMER_POINT_Y;
  if(!status) LCD_FillData_ex(x, y, _acb_point[0], _acb_point[1], _acb_point ,MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acb_point[0]*MAGNIFICATION, y + _acb_point[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* 名称：show_level
* 功能：显示液位
* 参数：level 实时液位
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void show_level(uint8_t level)
{
  uint8_t lh, ll;
  static uint8_t l_lh, l_ll;
  
  lh = level / 10;
  ll = level % 10;
  
  if((l_lh != lh) || (lh == 0)) show_s_num(LEVEL_NUM_H_X, LEVEL_NUM_H_Y, lh);
  if((l_ll != ll) || (lh == 0)) show_s_num(LEVEL_NUM_L_X, LEVEL_NUM_L_Y, ll);
  
  l_lh = lh;
  l_ll = ll;
}
