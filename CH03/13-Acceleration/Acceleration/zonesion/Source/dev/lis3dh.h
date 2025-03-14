#ifndef __LIS3DH_H_
#define __LIS3DH_H_
#include "extend.h"

#define	LIS3DH_ADDR             0x32		                    //三轴的IIC地址
#define	LIS3DH_IDADDR           0x0F		                    //三轴的ID地址
#define	LIS3DH_ID               0x33		                    //三轴的ID
#define LIS3DH_CTRL_REG1        0x20
#define LIS3DH_CTRL_REG2        0x21
#define LIS3DH_CTRL_REG3        0x22
#define LIS3DH_CTRL_REG4        0x23
#define LIS3DH_OUT_X_L		    0x28
#define LIS3DH_OUT_X_H		    0x29
#define LIS3DH_OUT_Y_L		    0x2A
#define LIS3DH_OUT_Y_H		    0x2B
#define LIS3DH_OUT_Z_L		    0x2C
#define LIS3DH_OUT_Z_H		    0x2D

int lis3dh_init(void);
void get_lis3dhInfo(float *accX, float *accY, float *accZ);


#endif    // end __LIS3DH_H_
