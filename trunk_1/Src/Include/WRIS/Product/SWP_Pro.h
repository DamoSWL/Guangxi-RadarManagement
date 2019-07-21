/************************************************************************
CSWPFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __SWP_FILE_080612 
#define __SWP_FILE_080612 

#include "rvw_pro.h"

//! SWP 产品头
struct __SWP_HEADER { 
	float VILLenOfWin; //!< 垂直累积液态含水量窗口宽度
	float maxRan;      //!<该VIL数据文件所对应的最大探测距离
	float LenOfWin;    //!< 强天气概率窗口宽度
	float maxLayCell;  //!< 径向上最大SWP网格数
	float maxSWP;      //!< 根据当前VIL数据文件计算出的最大强天气概率
	int cloudType;	   //!< 云的性质
};

//!本类用于解析SWP产品
/*! \class SWPPro
	SWP 产品文件解析类。
*/
class SWPPro:
	public RVWPro
{
public:
	__SWP_HEADER m_SWPHeader;
	float *m_pFData;
public:
	SWPPro(void);
	~SWPPro(void);

	int Open(string szPath);

	int OpenBuff( char *pBuff,int iLen);

};

#endif
