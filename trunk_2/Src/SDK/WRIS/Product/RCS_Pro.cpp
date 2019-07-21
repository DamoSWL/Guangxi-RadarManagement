#include "../../../include/WRIS/product/rcs_pro.h"

#define LEN 5

RCSPro::RCSPro(void)
{
}

RCSPro::~RCSPro(void)
{
}

/*! 打开数据文件
\param szPath 文件的绝对路径
\param bPhase 是否解析文件 
\return
\arg 1 文件名为空
\arg 2 文件打开错误，文件不存在或路径不正确
\arg 3 文件读取错误
*/
int RCSPro::Open(string szPath)
{
	RVWPro::Open( szPath );
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	////////////////////////
	memcpy( &m_RCSHeader, m_pData, sizeof(__RCS_HEADER) );
	m_pVData = m_pData + sizeof( __RCS_HEADER );

	return 0;
}

int RCSPro::OpenBuff( char *pBuff, int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen);
	//////////////////////////////////////////////////////////////////////////

	memcpy( &m_RCSHeader, m_pData, sizeof(__RCS_HEADER) );
	m_pVData = m_pData + sizeof( __RCS_HEADER );

	//"20100914_172743.00.003.004_0.00"
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;

	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%d-%.2f-%d-%.2f-%.2f",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		0,
		ob.iProductNumber,
		ob.batch.scanmode,
		(int)m_RCSHeader.sDis,
		m_RCSHeader.sAzi,
		(int)m_RCSHeader.eDis,
		m_RCSHeader.eAzi,
		m_RCSHeader.ele
		);

	m_szPath = cName;
	if (m_RCSHeader.sDis>GetMaxD()*1000)
	{
		m_RCSHeader.sDis = GetMaxD()*1000;
	}
	if (m_RCSHeader.eDis>GetMaxD()*1000)
	{
		m_RCSHeader.eDis = GetMaxD()*1000;
	}
	return 0;
}

