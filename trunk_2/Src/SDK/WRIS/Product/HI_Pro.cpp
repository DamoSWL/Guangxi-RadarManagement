

#include "../../../include/WRIS/product/HI_pro.h"

#include <math.h>

HIPro::HIPro(void)
{
	m_pDataT = NULL;
}

HIPro::~HIPro(void)
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
int HIPro::Open(string szPath)
{
	RVWPro::Open( szPath );

	///////////////

	if(szPath == "")
		return 1;

	m_szPath = szPath;

	memcpy( &m_HIHeader, m_pData, sizeof(tagHIHead));
	m_pDataT = m_pData + sizeof(tagHIHead);
	if (!m_bVisible.empty())
	{
		m_bVisible.clear();
	}
	for (int i=0;i<m_HIHeader.StormNum;i++)
	{
		m_bVisible.push_back(false);
	}

	return 0;
}

int HIPro::OpenBuff( char *pBuff,int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen );
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_HIHeader, m_pData, sizeof(tagHIHead));
	m_pDataT = m_pData + sizeof(tagHIHead);

	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;
	if (!m_bVisible.empty())
	{
		m_bVisible.clear();
	}
	for (int i=0;i<m_HIHeader.StormNum;i++)
	{
		m_bVisible.push_back(false);
	}
	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%.2f-%.2f-%.2f",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		0,
		ob.iProductNumber,
		ob.batch.scanmode,
		m_HIHeader.Area,
		m_HIHeader.H0,
		m_HIHeader.Hm20);

	m_szPath = string(cName);
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//************************************
// Method:    GetStormCount
// FullName:  HIPro::GetStormCount
// Access:    public 
// Returns:   unsigned int
// Qualifier: 获取风暴体个数
//************************************
unsigned int HIPro::GetStormCount( )
{
	return m_HIHeader.StormNum;
}

//************************************
// Method:    GetStormFeature
// FullName:  HIPro::GetStormFeature
// Access:    public 
// Returns:   tagSCFEATURE *
// Qualifier: 获取风暴体特征量
// Parameter: unsigned int iIndex 风暴体编号
//************************************
tagSCFEATURE * HIPro::GetStormFeature( unsigned int iIndex )
{
	tagSCFEATURE *pSCfeature = (tagSCFEATURE *)m_pDataT;
	tagSCFEATURE *pSCf = pSCfeature+iIndex;
	return pSCf;
}


