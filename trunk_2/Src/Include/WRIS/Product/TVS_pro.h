/************************************************************************
CTVSFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef _TVS_FILE_120312
#define _TVS_FILE_120312

#include "rvw_pro.h"

//! TVS产品头
typedef struct _TVSHEAD {
	int TTS; //最小切变阈值
	int TVSNum; //所识别出的龙卷涡旋的个数
	int ETVSNum;//
}tagTVSHead;


//!龙卷涡旋属性 attribute
typedef struct _TVSATTRI
{
	int STORMID;//风暴单体标识号:离它最近的风暴单体标识号
	float AZ;//TVS的方位
	float RAN;//TVS的距离
	float HEIGHT;//最大切变出现的高度
	float SHEAR;//每秒千分之多少
	float ORI;//最大、最小速度连线相对于雷达径向的取向
	float ROT;//最大、最小速度连线相对于雷达径向的旋转速度的大小
}tagTVSAttri;
/*
tagFileHeader + tagTVSHead + 中尺度气旋0 + 中尺度气旋1 + ......；
*/

//! 基数据及其类似数据解析类
/*!
 *	此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
	\龙卷涡旋识别（TVS）

 * \author 樊兆妮
 * \ingroup group_UT
*/
class CTVSPro:
	public RVWPro
{

public:
	tagTVSHead m_TVSHeader;

	unsigned char *m_pTVSData;
	unsigned int m_DataLen ;

public:
	CTVSPro(void);
	~CTVSPro(void);

	int Open(string szPath);
	int OpenBuff( char *pBuff, int iLen);
	void Close();

};


#endif