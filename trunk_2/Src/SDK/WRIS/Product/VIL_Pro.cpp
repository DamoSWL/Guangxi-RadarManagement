
#include "../../../include/WRIS/product/vil_pro.h"

#define LEN 5

VILPro::VILPro(void):
	m_pFData(NULL)
{


}

VILPro::~VILPro(void)
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
int VILPro::Open(string szPath)
{
	RVWPro::Open( szPath );
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	////////////////////////
	memcpy( &m_VILHeader, m_pData, sizeof(__VIL_HEADER) );

	m_pFData = (float *)(m_pData + sizeof(__VIL_HEADER));

	return 0;
}

int VILPro::OpenBuff( char *pBuff,int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen);
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_VILHeader, m_pData, sizeof(__VIL_HEADER) );
	m_pFData = (float *)(m_pData + sizeof(__VIL_HEADER));

	//"20100914_172743.00.003.004_0.00"
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;

	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%d-%d",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		0,
		ob.iProductNumber,
		ob.batch.scanmode,
		(int)m_VILHeader.LenOfWin,
		(int)m_VILHeader.CloudType
		);

	m_szPath = string(cName);

	return 0;
}




