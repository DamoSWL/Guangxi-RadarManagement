/************************************************************************
CRZFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __LRA_FILE_080612 
#define __LRA_FILE_080612 

#include "rvw_pro.h"

//! RZ产品头
typedef struct _RZHEAD {
	int Cnt;		  //!< 没有使用,保留
	float A;          //!< Z-R参数
	float b;          //!<Z-R参数
	float LenOfWin;   //!<窗口大小
	int   maxLayCell; //!<径向上所包含的最大窗口数 
	float maxRZ;      //!<最大雨强
}tagRZHead;


//! 基数据及其类似数据解析类
/*!
 * 此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
   \arg 分层组合反射率平均值（LRA）
   \arg 分层组合反射率最大值（LRM）
  
 * \author lava_sdb
 * \ingroup group_UT
*/
class RZPro:
	public RVWPro
{
public:
	tagRZHead m_RZHeader;

	float *m_pFData;
public:
	RZPro(void);
	~RZPro(void);

	int Open(string szPath);	
	int OpenBuff( char *pBuff,int iLen);

};


#endif
