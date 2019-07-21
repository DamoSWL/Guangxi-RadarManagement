
#include "../../../include/WRIS/product/rvd_pro.h"

RVDPro::RVDPro(void)
{
	;
}

RVDPro::~RVDPro(void)
{
	;
}

/*! 打开数据文件
\param szPath 文件的绝对路径
\param bPhase 是否解析文件 
\return
\arg 1 文件名为空
\arg 2 文件打开错误，文件不存在或路径不正确
\arg 3 文件读取错误
*/

int RVDPro::Open(string szPath)
{
	RVWPro::Open(szPath);
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_RVDHeader, m_pData, sizeof(tagRVDHead));
	m_pRVDData = (float *)(m_pData + sizeof(tagRVDHead));

	return 0;
}

int RVDPro::OpenBuff( char *pBuff, int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen);

	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_RVDHeader, m_pData, sizeof(tagRVDHead));

	m_pRVDData = (float *)(m_pData + sizeof(tagRVDHead));

	//"20100914_172743.00.044.004_0.04-3.60"
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;
	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%d-%.2f",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		0,
		ob.iProductNumber,
		ob.batch.scanmode,
		m_RVDHeader.Ele,
		m_RVDHeader.LenOfWin
		);

	m_szPath = string(cName);
	return 0;
}

