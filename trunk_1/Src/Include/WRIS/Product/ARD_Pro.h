/************************************************************************
CARDFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef _ARD_FILE_080617
#define _ARD_FILE_080617

#include "rvw_pro.h"

//! ARD产品头
typedef struct _ARDHEAD {
	union{
		float fEle;		//大船历史数据为浮点数
		int   Ele;     //!< 计算径向散度的仰角编号
	};
	float AziOfWin;  ///计算径向散度的窗口长度，单位 千米 （km）
	int   CalNum;    ////一个径向上径向散度的个数
	float MaxARD;	 ////该PPI最大的径向散度
}tagARDHead;
/*
tagFileHeader + tagARDHeader + 径向0 + 径向1 + ......；
200个径向每PPI ；
usRefBinNumber个库数每径向 ；
*/

//! 基数据及其类似数据解析类
/*!
 *	此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
	\方位涡度（ARD）

 * \author lava_sdb
 * \ingroup group_UT
*/
class ARDPro:
	public RVWPro
{

public:
	tagARDHead m_ARDHeader;

	float *m_pARDData;
	unsigned int m_DataLen ;

public:
	ARDPro(void);
	~ARDPro(void);

	int Open(string szPath);
	int OpenBuff( char *pBuff, int iLen);
	void Close();

};


#endif