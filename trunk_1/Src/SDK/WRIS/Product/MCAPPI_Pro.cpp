

#include "../../../include/WRIS/product/ard_pro.h"

ARDPro::ARDPro(void)
{
	;
}

ARDPro::~ARDPro(void)
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

int ARDPro::Open(string szPath)
{
	RVWPro::Open(szPath);
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_ARDHeader, m_pData, sizeof(tagARDHead));
	m_Header.obserSec.iCaiYangJinXiangShu[0] = m_ARDHeader.CalNum;
	m_pARDData = (float *)(m_pData + sizeof(tagARDHead));

	return 0;
}

int ARDPro::OpenBuff( char *pBuff, int iLen)
{

	m_DataLen = iLen;
	RVWPro::OpenBuff( pBuff, iLen);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_ARDHeader, m_pData, sizeof(tagARDHead));
	m_Header.obserSec.iCaiYangJinXiangShu[0] = m_ARDHeader.CalNum;

	m_pARDData = (float *)(m_pData + sizeof(tagARDHead));

	//"20100914_172743.00.043.004_0.04-3.60"
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;

	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%.2f-%.2f",
		ob.iGuanCeKaiShiNian,
		ob.iGuanCeKaiShiYue,
		ob.iGuanCeKaiShiRi,
		ob.iGuanCeKaiShiShi,
		ob.iGuanCeKaiShiFen,
		ob.iGuanCeKaiShiMiao,
		0,
		ob.iProductNumber,
		ob.batch.scanmode,
		(float)m_ARDHeader.Ele,
		m_ARDHeader.AziOfWin
		);

	m_szPath = string(cName);
	return 0;
}