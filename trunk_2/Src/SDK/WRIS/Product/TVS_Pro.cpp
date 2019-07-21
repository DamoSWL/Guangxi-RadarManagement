

#include "../../../include/WRIS/product/TVS_pro.h"

CTVSPro::CTVSPro(void)
{
	;
}

CTVSPro::~CTVSPro(void)
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

int CTVSPro::Open(string szPath)
{
	RVWPro::Open(szPath);
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_TVSHeader, m_pData, sizeof(tagTVSHead));
	m_pTVSData = (unsigned char *)(m_pData + sizeof(tagTVSHead));

	return 0;
}

int CTVSPro::OpenBuff( char *pBuff, int iLen)
{

	m_DataLen = iLen;
	RVWPro::OpenBuff( pBuff, iLen);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_TVSHeader, m_pData, sizeof(tagTVSHead));
	m_pTVSData = (unsigned char *)(m_pData + sizeof(tagTVSHead));

	//"20100914_172743.00.043.004_0.04-3.60"
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
		m_TVSHeader.TTS
		);

	m_szPath = string(cName);
	return 0;
}