/************************************************************************
CVADFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef _MAX_FILE_110310
#define _MAX_FILE_110310


#include "rvw_pro.h"

#pragma pack(4)

//!MAX产品头
typedef struct _MAX_HEADER { 
	float fLenOfWin;         //!< MAX窗口大小
	float H;                 //!< N-S、W-E的起始高度
	float fThreshold;        //!< 阈值
	int  iCellNum;           //!< TOP图中窗口数
	int iNSwinnumv;          //!< N_S图垂直方向上的窗口数
	float flongitude;        //!< 起始窗口的经度
	float flatitude;         //!< 起始窗口的纬度
}tagMAXHead;

#pragma pack()


//! 基数据及其类似数据解析类
/*!
*	此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
\速度方位显示（MAX）

* \author fanzhaoni
* \ingroup group_UT
*/
class MAXPro:
	public RVWPro
{

public:
	tagMAXHead m_MAXHeader;

	float *m_pMAXData;



public:
	MAXPro(void);
	~MAXPro(void);

	int Open(string szPath);
	int OpenBuff( char *pBuff, int iLen);
	void Close();
};

#endif