
#include "../../../include/WRIS/product/vad_pro.h"

VADPro::VADPro(void)
{
	m_pDataR = NULL;
	m_pDataV = NULL;
	m_pVAD = NULL;
}

VADPro::~VADPro(void)
{

	if( m_pDataR != NULL ) 
	{
		delete []m_pDataR;
		m_pDataR = NULL;
	}
	if( m_pDataV != NULL )
	{
		delete []m_pDataV;
		m_pDataR = NULL;
	}
	if( m_pVAD != NULL )
	{
		delete []m_pVAD;
		m_pDataR = NULL;
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

int VADPro::Open(string szPath)
{
	RVWPro::Open(szPath);
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
		
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_VADHeader, m_pData, sizeof(tagVADHead));

	m_iNumOfRadial = m_Header.obserSec.iRadialNum[0];
	if( m_pDataR != NULL ) delete []m_pDataR;
	m_pDataR = new float[m_iNumOfRadial];
	if( m_pDataV != NULL ) delete []m_pDataV;
	m_pDataV = new float[m_iNumOfRadial];
	if( m_pVAD != NULL ) delete []m_pVAD;
	m_pVAD = new float[m_iNumOfRadial];

	memcpy( m_pDataR, m_pData+sizeof(tagVADHead),m_iNumOfRadial*sizeof(float) );
	memcpy( m_pDataV, m_pData+sizeof(tagVADHead)+m_iNumOfRadial*sizeof(float),m_iNumOfRadial*sizeof(float) );
	memcpy( m_pVAD, m_pData+sizeof(tagVADHead)+m_iNumOfRadial*sizeof(float)+m_iNumOfRadial*sizeof(float),m_iNumOfRadial*sizeof(float) );

	return 0;
}

int VADPro::OpenBuff(char *pBuff, int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen);
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_VADHeader, m_pData, sizeof(tagVADHead));

	m_iNumOfRadial = m_Header.obserSec.iRadialNum[0];
	if( m_pDataR != NULL ) delete []m_pDataR;
	m_pDataR = new float[m_iNumOfRadial];
	if( m_pDataV != NULL ) delete []m_pDataV;
	m_pDataV = new float[m_iNumOfRadial];
	if( m_pVAD != NULL ) delete []m_pVAD;
	m_pVAD = new float[m_iNumOfRadial];

	memcpy( m_pDataR, m_pData+sizeof(tagVADHead),m_iNumOfRadial*sizeof(float) );
	memcpy( m_pDataV, m_pData+sizeof(tagVADHead)+m_iNumOfRadial*sizeof(float),m_iNumOfRadial*sizeof(float) );
	memcpy( m_pVAD, m_pData+sizeof(tagVADHead)+m_iNumOfRadial*sizeof(float)+m_iNumOfRadial*sizeof(float),m_iNumOfRadial*sizeof(float) );

	//"20100914_172743.00.003.004_0.00"
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;

	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%.2f-%.2f",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		0,
		ob.iProductNumber,
		ob.batch.scanmode,
		m_VADHeader.VerRan,
		m_VADHeader.HorRan
		);

	m_szPath = string(cName);

	return 0;
}