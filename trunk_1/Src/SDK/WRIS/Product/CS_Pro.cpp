
#include "../../../include/WRIS/product/cs_pro.h"

CSPro::CSPro(void)
{
	;
}

CSPro::~CSPro(void)
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

int CSPro::Open(string szPath)
{
	RVWPro::Open(szPath);
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_RACSHeader, m_pData, sizeof(tagRACSHead));
	m_pRACSData = (float *)(m_pData + sizeof(tagRACSHead));

	return 0;
}

int CSPro::OpenBuff( char *pBuff, int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen);
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_RACSHeader, m_pData, sizeof(tagRACSHead));
	m_pRACSData = (float *)(m_pData + sizeof(tagRACSHead));

	//"20100914_172743.00.042.004_0.04-3.60"
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;

	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%d-%.2f-%.2f",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		0,
		ob.iProductNumber,
		ob.batch.scanmode,
		m_RACSHeader.Ele,
		m_RACSHeader.LenOfWin,
		m_RACSHeader.AziOfWin
		);

	m_szPath = string(cName);
	return 0;
}