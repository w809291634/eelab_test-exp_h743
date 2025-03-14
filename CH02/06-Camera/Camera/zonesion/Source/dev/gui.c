#include "gui.h"
#include "ili9806.h"
#include "text.h"

/*********************************************************************************************
* 名称：gui_draw_hline
* 功能：使用填充法画水平线
* 参数：x0,y0:坐标
*          len:线长度
*          color:颜色
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill_s(x0,y0,x0+len-1,y0,color);	
}

/*********************************************************************************************
* 名称：gui_fill_circle
* 功能：画实心圆
* 参数：x0,y0:坐标
*          r:半径
*          color:颜色
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  

/*********************************************************************************************
* 名称：gui_draw_bline
* 功能：画一条粗线
* 参数：(x1,y1),(x2,y2):线条的起始坐标
*          size：线条的粗细程度
*          color：线条的颜色
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void gui_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0; //垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0; //水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ ) //画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance){ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance){ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   

/*********************************************************************************************
* 名称：gui_draw_btu
* 功能：画一个矩形按钮
* 参数：(x1,y1),(x2,y2):按钮的起始坐标和结束坐标
*          size：线条的粗细程度
*          color：线条的颜色
* 返回：无
* 修改：无
* 注释：无
*********************************************************************************************/
void gui_draw_btu(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
  gui_draw_bline(x1,y1,x2,y1,size,color);
  gui_draw_bline(x1,y2,x2,y2,size,color);     // 平行横线
  gui_draw_bline(x1,y1,x1,y2,size,color);
  gui_draw_bline(x2,y1,x2,y2,size,color);     // 平行竖线
}  