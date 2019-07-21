#include "../../../include/WRIS/product/puzzle_pro.h"
#include "../../../include/WRIS/product/ProductID.h"

PuzzlePro::PuzzlePro(void)
{
	m_CAPPIPZHeader = new tagCAPPIHead;
	memset(m_CAPPIPZHeader,0,sizeof(tagCAPPIHead));
	m_CRPZHeader = new tagCRHead;
	memset(m_CRPZHeader,0,sizeof(tagCRHead));
	m_VILPZHeader = new tagVILPZHead;
	memset(m_VILPZHeader,0,sizeof(tagVILPZHead));
	m_OHPPZHeader = new tagOHPPZHead;
	memset(m_OHPPZHeader,0,sizeof(tagOHPPZHead));
	m_ETEBPZHeader = new tagETEBPZHead;
	memset(m_ETEBPZHeader,0,sizeof(tagETEBPZHead));

	m_pVData = NULL;
}

PuzzlePro::~PuzzlePro(void)
{
	if (m_CAPPIPZHeader != NULL)
	{
		delete m_CAPPIPZHeader;
		m_CAPPIPZHeader = NULL;
	}
	if (m_CRPZHeader != NULL)
	{
		delete m_CRPZHeader;
		m_CRPZHeader = NULL;
	}
	if (m_VILPZHeader != NULL)
	{
		delete m_VILPZHeader;
		m_VILPZHeader = NULL;
	}
	if (m_OHPPZHeader != NULL)
	{
		delete m_OHPPZHeader;
		m_OHPPZHeader = NULL;
	}
	if (m_ETEBPZHeader != NULL)
	{
		delete m_ETEBPZHeader;
		m_ETEBPZHeader = NULL;
	}
}

/*! 打开数据文件
\param szPath 文件的绝对路径
\param bPhase 是否解析文件 
\return
\arg 1 文件名为空
\arg 2 文件打开错误，文件不存在或路径不正确
\arg 3 文件读取错误
*/
int PuzzlePro::Open(string szPath)
{
	//PuzzlePro::Open( szPath );
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	////////////////////////
// 	memcpy( &m_CRPZHeader, m_pData, sizeof(tagCRHead) );
// 	m_pVData = m_pData + sizeof( tagCRHead );

	return 0;
}

int PuzzlePro::OpenBuff( char *pBuff, int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen,false);
	//////////////////////////////////////////////////////////////////////////
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;
	if (ob.iProductNumber == PID_CAPPIPZ || ob.iProductNumber == PID_NWCAPPI)
	{
		memcpy( m_CAPPIPZHeader, m_pData, sizeof(tagCAPPIHead) );
		m_pVData = m_pData + sizeof( tagCAPPIHead ) + sizeof(tagPzRadarInf)*m_CAPPIPZHeader->RadarsCount;//数据开始的位置
	}
	else if (ob.iProductNumber == PID_CRPZ|| ob.iProductNumber == PID_NWCR)
	{
		memcpy( m_CRPZHeader, m_pData, sizeof(tagCRHead) );
		m_pVData = m_pData + sizeof( tagCRHead ) + sizeof(tagPzRadarInf)*m_CRPZHeader->RadarsCount;
	}
	else if (ob.iProductNumber == PID_VILPZ)
	{
		memcpy( m_VILPZHeader, m_pData, sizeof(tagVILPZHead) );
		m_pVData = m_pData + sizeof( tagVILPZHead ) + sizeof(tagPzRadarInf)*m_VILPZHeader->RadarsCount;
	}
	else if (ob.iProductNumber == PID_RZPZ || ob.iProductNumber == PID_OHPPZ || ob.iProductNumber == PID_THPPZ || ob.iProductNumber == PID_STPPZ)
	{
		memcpy( m_OHPPZHeader, m_pData, sizeof(tagOHPPZHead) );
		m_pVData = m_pData + sizeof( tagOHPPZHead ) + sizeof(tagPzRadarInf)*m_OHPPZHeader->RadarsCount;
	}
	else if (ob.iProductNumber == PID_ETPZ || ob.iProductNumber == PID_EBPZ
		|| ob.iProductNumber == PID_NWET || ob.iProductNumber == PID_NWEB)
	{
		memcpy( m_ETEBPZHeader, m_pData, sizeof(tagETEBPZHead) );
		m_pVData = m_pData + sizeof( tagETEBPZHead ) + sizeof(tagPzRadarInf)*m_ETEBPZHeader->RadarsCount;
	}

	return 0;
}

