/************************************************************************
CRVDFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef _RVD_FILE_080617
#define _RVD_FILE_080617

#include "../../../include/WRIS/product/rvw_pro.h"

//! RVD产品头
typedef struct _RVDHEAD {
	union{
		float fEle;		//大船历史数据为浮点数
		int   Ele;     //!< 计算径向散度的仰角编号
	};
	float LenOfWin;  //!< 计算径向散度的窗口长度，单位km
	int   CalNum;    //!< 一个径向上径向散度的个数
	float MaxRVD;	 //!< 该PPI最大的径向散度
}tagRVDHead;

//! 基数据及其类似数据解析类
/*!
 *	此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
	\速度径向散度（RVD）

 * \author lava_sdb
 * \ingroup group_UT
*/

class RVDPro:
	public RVWPro
{

public:
	tagRVDHead m_RVDHeader;

	float *m_pRVDData;
	int iLen ;

public:
	RVDPro(void);
	~RVDPro(void);

	int Open(string szPath);
	int OpenBuff( char *pBuff, int iLen);

};




#endif