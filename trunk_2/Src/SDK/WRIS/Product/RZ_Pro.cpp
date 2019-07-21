
#include "../../../include/WRIS/product/rz_pro.h"

#define LEN 5

RZPro::RZPro(void):
	m_pFData(NULL)
{


}

RZPro::~RZPro(void)
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
int RZPro::Open(string szPath)
{
	RVWPro::Open( szPath );
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	////////////////////////
	memcpy( &m_RZHeader, m_pData, sizeof(tagRZHead) );

	m_pFData = (float *)(m_pData + sizeof(tagRZHead));

	return 0;
}

int RZPro::OpenBuff( char *pBuff, int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen);
	//////////////////////////////////////////////////////////////////////////
	
	memcpy( &m_RZHeader, m_pData, sizeof(tagRZHead) );
	m_pFData = (float *)(m_pData + sizeof(tagRZHead));

	//"20100914_172743.00.003.004_0.00"
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;

	if( ob.iProductNumber == 39 )
	{
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
			(int)m_RZHeader.LenOfWin,
			m_RZHeader.A,
			m_RZHeader.b
			);
	}else
	{
		sprintf( cName,
			"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%d-%.2f-%.2f",
			ob.iObsEndTimeYear,
			ob.iObsEndTimeMonth,
			ob.iObsEndTimeDay,
			ob.iObsEndTimeHour,
			ob.iObsEndTimeMinute,
			ob.iObsEndTimeSecond,
			0,
			ob.iProductNumber,
			ob.batch.scanmode,
			(int)m_RZHeader.LenOfWin,
			m_RZHeader.A,
			m_RZHeader.b
			);
	}

	m_szPath = string(cName);
	return 0;
}





