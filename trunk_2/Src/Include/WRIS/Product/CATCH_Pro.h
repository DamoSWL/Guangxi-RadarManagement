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

class CATCHPro:
	public RVWPro
{
public:
	tagRZHead m_RZHeader;

	float *m_pFData;
public:
	CATCHPro(void);
	~CATCHPro(void);

	int Open(string szPath);	
	int OpenBuff( char *pBuff,int iLen);

};


#endif
