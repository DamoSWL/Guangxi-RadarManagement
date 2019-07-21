/************************************************************************
CLTAFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __LRA_FILE_080612 
#define __LRA_FILE_080612 

#include "rvw_pro.h"

//! LTA 产品头
struct __LTA_HEADER { 
	float LenOfWin;	//!< 窗口宽度
	float LayCellCou; //!<  径向上最大窗口数
	float HeightL;  //!<  底层分层高度
	float HeightM;  //!<  中层分层高度
	float HeightH;  //!<  高层分层高度
	float MaxLTA1;  //!<  底层平均值
	float MaxLTA2;  //!<  中层平均值
	float MaxLTA3;  //!<  高层平均值
};

//! LTM产品头
struct __LTM_HEADER { 
	float LenOfWin; //!< 窗口宽度
	float LayCellCou; //!<  径向上最大窗口数
	float HeightL;  //!<  底层分层高度
	float HeightM;  //!<  中层分层高度
	float HeightH;  //!<  高层分层高度
	float MaxLTM1;  //!<  底层最大值
	float MaxLTM2;  //!<  中层最大值
	float MaxLTM3;  //!<  高层最大值
};



//! 基数据及其类似数据解析类
/*!
 * 此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
   \arg 分层组合反射率平均值（LTA）
   \arg 分层组合反射率最大值（LTM）
  
 * \author lava_sdb
 * \ingroup group_UT
*/
class LTAPro:
	public RVWPro
{
public:
	__LTA_HEADER m_LTAHeader;

	float *m_pDataL;
	float *m_pDataM;
	float *m_pDataH;
public:
	LTAPro(void);
	~LTAPro(void);

	int Open(string szPath);	
	int OpenBuff( char *pBuff,int iLen);
};


#endif
