/************************************************************************
CLRAFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __MCAPPI_FILE_080612 
#define __MCAPPI_FILE_080612 

#include "rvw_pro.h"


#pragma pack(push,4)
#define RYMCAPPICUT 10				//10层
#define CUT 4				//10层
#define SIZE 300
#define ANQLE 30
//! MCAPPI产品头
typedef struct __MCAPPI_HEADER { 
//	int iHCutNum;		//!< 切割层数
	unsigned int iHCutNum:4;		//!< 切割层数
	unsigned int NumberofWin:16;	
	unsigned int LenofWin:4;
	unsigned int spa:6;
	unsigned int Ver:2;	
	int iHeight[12];  //!<切割高度,由于可以存放12层，WEB需要同时修改
}tagMCAPPIHead;
#pragma pack(pop)


//! \class MCAPPIPro
/*!
	\brief 本类用于解析MCAPPI产品文件

 * 此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
   \arg 平面高度显示（LRA）

 * \author lava_sdb
 * \ingroup group_UT
*/
class MCAPPIPro:
	public RVWPro
{
public:
	tagMCAPPIHead m_MCAPPIHeader;

	int OpenBuff( char *pBuff,int iLen, bool bCopy = true )
	{
		int iRet = RVWPro::OpenBuff( pBuff, iLen, bCopy );

		if( iRet == 0 )
		{
			memcpy( &m_MCAPPIHeader, m_pData, sizeof( tagMCAPPIHead ) );
			m_pData += sizeof( tagMCAPPIHead );

			//"20100914_172743.00.008.004_1000"
			char cName[256] = {0};
			tagRealObserSec &ob = m_Header.obserSec;

			sprintf( cName,
				"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%d-%d-%d-%d",
				ob.iObsStartTimeYear,
				ob.iObsStartTimeMonth,
				ob.iObsStartTimeDay,
				ob.iObsStartTimeHour,
				ob.iObsStartTimeMinute,
				ob.iObsStartTimeSecond,
				0,
				ob.iProductNumber,
				ob.batch.scanmode,
				(int)m_MCAPPIHeader.iHeight[0],
				(int)m_MCAPPIHeader.iHeight[1],
				(int)m_MCAPPIHeader.iHeight[2],
				(int)m_MCAPPIHeader.iHeight[3]
				);

			m_szPath = cName;
		}

		return iRet;
	}
};



#endif


