
/*!
File: YW_Radar Data Struct
note: 
author: lava_sdb
date: 2008/01/29
*/

#ifndef _HEADER_YW_RADAR_DATA_STRUCT_080129
#define _HEADER_YW_RADAR_DATA_STRUCT_080129

#pragma pack(4)

//��Ľṹ��
typedef struct _PolarPoint
{
	int X;	// X
	int Y;	// Y
	float fDis;	// ��ľ������( ��λ:km )
	int iSeta;	// �㷽λ( ��  )
	unsigned char val;
}tagPolarPoint;

#pragma pack()


#endif
