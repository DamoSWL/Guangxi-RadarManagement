/*! \file HPF_Pro.h
  本文件用于解析HPF产品                                                               
*/  

#ifndef __HPF_FILE_080612 
#define __HPF_FILE_080612 

#include "rvw_pro.h"

//! HPF 产品头
typedef struct __HPF_HEADER { 
	float VILLenOfWin;	//!< VIL 窗口宽度
	float maxRan;		//!< 最大测距
	float LenOfWin;		//!< HPF 窗口宽度
	float maxLayCell;	//!< 径向上的最大窗口数	
	float maxHPF;	    //!< 最大的 HPF
	int cloudType;	    //!< 云的性质
}tagHPFHeader;


//! 本类用于解析HPF产品
/*! \class HPFPro
 此类用于解析HPF数据及其类似格式的数据
  
  \author lava_sdb
  \ingroup group_UT
*/
class HPFPro:
	public RVWPro
{
public:
	__HPF_HEADER m_HPFHeader;	//!< HPF 产品头

	float *m_pFData;			//!< HPF 产品数据指针
public:
	HPFPro(void);
	~HPFPro(void);

	int Open(string szPath);	
	int OpenBuff( char *pBuff,int iLen);

};


#endif
