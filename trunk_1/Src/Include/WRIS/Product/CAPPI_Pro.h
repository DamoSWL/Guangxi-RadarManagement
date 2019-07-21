/************************************************************************
CLRAFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __CAPPI_FILE_080612 
#define __CAPPI_FILE_080612 

#include "rvw_pro.h"

#pragma pack(push,4)

//! CAPPI产品头
struct __CAPPI_HEADER { 
	int iHeight;		//!< 切割高度
};
#pragma pack(pop)



//! \class CAPPIPro
/*!
	\brief 本类用于解析CAPPI产品文件

 * 此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
   \arg 平面高度显示（LRA）

 * \author lava_sdb
 * \ingroup group_UT
*/
class CAPPIPro:
	public RVWPro
{
public:
	__CAPPI_HEADER m_cappiHeader;

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
		memcpy( &m_cappiHeader, m_pData, sizeof( __CAPPI_HEADER ) );
		m_pData += sizeof( __CAPPI_HEADER );

		//"20100914_172743.00.008.004_1000"
		char cName[256] = {0};
		tagRealObserSec &ob = m_Header.obserSec;

		sprintf( cName,
			"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%d",
			ob.iObsStartTimeYear,
			ob.iObsStartTimeMonth,
			ob.iObsStartTimeDay,
			ob.iObsStartTimeHour,
			ob.iObsStartTimeMinute,
			ob.iObsStartTimeSecond,
			0,
			ob.iProductNumber,
			ob.batch.scanmode,
			m_cappiHeader.iHeight
			);

		m_szPath = cName;
		return 0;
	}
};


#endif


