

#include "../../../include/WRIS/product/M_pro.h"

CMPro::CMPro(void)
{
	;
}

CMPro::~CMPro(void)
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

int CMPro::Open(string szPath)
{
	RVWPro::Open(szPath);
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_MHeader, m_pData, sizeof(tagMHead));
	m_pMData = (unsigned char *)(m_pData + sizeof(tagMHead));

	return 0;
}

int CMPro::OpenBuff( char *pBuff, int iLen)
{

	m_DataLen = iLen;
	RVWPro::OpenBuff( pBuff, iLen);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_MHeader, m_pData, sizeof(tagMHead));
	m_pMData = (unsigned char *)(m_pData + sizeof(tagMHead));

	//"20100914_172743.00.043.004_0.04-3.60"
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
		m_MHeader.TPV,
		m_MHeader.TFM
		);

	m_szPath = string(cName);
	return 0;
}
//************************************
// Method:    GetMNum
// FullName:  CMPro::GetMNum
// Access:    public 
// Returns:   unsigned int
// Qualifier: 获得中气旋或三维切变的总数
//************************************
unsigned int CMPro::GetMNum()
{
	return m_MHeader.Num;
}
//************************************
// Method:    GetDem3Attri
// FullName:  CMPro::GetDem3Attri
// Access:    public 
// Returns:   tagDem3Attri *
// Qualifier: 获得指定的中气旋或三维切变的指针
// Parameter: int iIndex 三维特征序号
//************************************
tagDem3Attri *CMPro::GetDem3Attri(int iIndex)
{
	tagDem3Attri *pDem3Attri = (tagDem3Attri *)m_pMData;
	tagDem3Attri *pDem3 = pDem3Attri+iIndex;
	return pDem3;
}