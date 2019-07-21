/************************************************************************
CLRAFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __LRA_FILE_080612 
#define __LRA_FILE_080612 

#include "rvw_pro.h"


//! LRA 产品头
struct __LRA_HEADER { 
	float LenOfWin;	//!< 窗口宽度
	float LayCellCou; //!< 底层分层高度
	float HeightL;//!< 底层分层高度
	float HeightM; //!< 中层分层高度
	float HeightH; //!< 高层分层高度
	float MaxLRA1; //!< 底层平均值
	float MaxLRA2; //!< 中层平均值
	float MaxLRA3; //!< 高层平均值
};

//! LRM产品头
struct __LRM_HEADER { 
	float LenOfWin;//!< 窗口宽度
	float LayCellCou;//!< 径向上最大窗口数
	float HeightL;//!< 底层分层高度
	float HeightM;//!< 中层分层高度
	float HeightH;//!< 高层分层高度
	float MaxLRM1;//!< 底层最大值
	float MaxLRM2;//!< 中层最大值
	float MaxLRM3;//!< 高层最大值
};


//! 基数据及其类似数据解析类
/*!
 * 此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
   \arg 分层组合反射率平均值（LRA）
   \arg 分层组合反射率最大值（LRM）
  
 * \author lava_sdb
 * \ingroup group_UT
*/
class LRAPro:
	public RVWPro
{
public:
	__LRA_HEADER m_LRAHeader;

	float *m_pDataL;
	float *m_pDataM;
	float *m_pDataH;
public:
	LRAPro(void);
	~LRAPro(void);

	int Open(string szPath);	
	int OpenBuff( char *pBuff, int iLen );
};


#endif
