
/*!
File: YW_Radar Data Struct
note: 
author: lava_sdb
date: 2008/01/29
*/

#ifndef _HEADER_YW_RADAR_DATA_STRUCT_080129
#define _HEADER_YW_RADAR_DATA_STRUCT_080129

#pragma pack(4)

//点的结构体
typedef struct _PolarPoint
{
	int X;	// X
	int Y;	// Y
	float fDis;	// 点的径向距离( 单位:km )
	int iSeta;	// 点方位( 度  )
	unsigned char val;
}tagPolarPoint;

#pragma pack()


#endif
