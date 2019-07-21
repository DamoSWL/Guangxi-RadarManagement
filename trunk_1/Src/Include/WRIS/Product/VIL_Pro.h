/************************************************************************
CVILFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __VIL_FILE_080612 
#define __VIL_FILE_080612 

#include "rvw_pro.h"

//! VIL产品头
struct __VIL_HEADER { 
	float CloudType;  //!< 云的性质
	float a;          //!< M-Z关系式中对应的A
	float b;          //!< M-Z关系式中对应的b
	float maxRan;     //!< 某次体扫数据文件所对应的最大探测距离
	float LenOfWin;   //!< 计算时选取的VIL窗口大小，float 型
	float maxLayCell; //!< 径向上最大窗口数
	float maxVIL;     //!< 当前探测范围内最大的垂直累积液态含水量
};


//! 本类用于解析VIL产品
/*!
 * 此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
   \arg 垂直累积液态含水量（VIL）
  
 * \author lava_sdb
 * \ingroup group_UT
*/
class VILPro:
	public RVWPro
{
public:
	__VIL_HEADER m_VILHeader;  //!< VIL 产品头

	float *m_pFData;
public:
	VILPro(void);
	~VILPro(void);

	int Open(string szPath);	
	int OpenBuff( char *pBuff,int iLen);

};


#endif
