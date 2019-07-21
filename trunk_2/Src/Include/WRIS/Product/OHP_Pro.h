/************************************************************************
CRZFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __LRA_FILE_080612 
#define __LRA_FILE_080612 

#include "rvw_pro.h"


//! N小时降水量产品头
typedef struct _OHPHEAD {
	int   VOLCnt;	  //!< 体扫个数
	float A;          //!< Z-R参数
	float b;          //!< Z-R参数
	float LenOfWin;   //!< 窗口大小
	int   maxLayCell; //!< 径向上所包含的最大窗口数 
	float maxOHP;     //!< 最大1小时降水量
}tagOHPHead;



#endif
