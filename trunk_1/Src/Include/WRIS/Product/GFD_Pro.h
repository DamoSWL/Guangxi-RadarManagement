/************************************************************************
CLRAFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __GFD_FILE_20120924 
#define __GFD_FILE_20120924 

#include "rvw_pro.h"

#pragma pack(push,4)

//! GFD产品头
//struct tagGFDHead { 
//	int iHeight;		//!< 切割高度
//};
typedef struct _GFDHEAD
{
	int iGFNum;  //!<一个体扫内阵风锋的个数
}tagGFDHead;
#pragma pack(pop)



//! \class GFDPro
/*!
	\brief 本类用于解析GFD产品文件

 * 此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
   \arg 平面高度显示（LRA）

 * \author lava_sdb
 * \ingroup group_UT
*/
class GFDPro:
	public RVWPro
{
public:
	tagGFDHead m_GFDHeader;

	/*! 从 缓冲打开产品
		\param pBuff 缓冲指针
		\param iLen 缓冲数据长度
		\param bCopy 是否使用传入的数据
		             - true 在内存里复制传入数据的一个备份
					 - false 直接使用传入的数据
	*/
	int OpenBuff( char *pBuff,int iLen, bool bCopy = true )
	{
		RVWPro::OpenBuff( pBuff, iLen, bCopy );
		memcpy( &m_GFDHeader, m_pData, sizeof( tagGFDHead ) );
		m_pData += sizeof( tagGFDHead );

		//"20100914_172743.00.008.004_1000"
		char cName[256] = {0};
		tagRealObserSec &ob = m_Header.obserSec;

		sprintf( cName,
			"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d",
			ob.iObsStartTimeYear,
			ob.iObsStartTimeMonth,
			ob.iObsStartTimeDay,
			ob.iObsStartTimeHour,
			ob.iObsStartTimeMinute,
			ob.iObsStartTimeSecond,
			0,
			ob.iProductNumber,
			ob.batch.scanmode
			);

		m_szPath = cName;
		return 0;
	}
};


#endif


