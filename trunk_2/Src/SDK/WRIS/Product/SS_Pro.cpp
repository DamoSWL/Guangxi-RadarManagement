

#include "../../../include/WRIS/product/ss_pro.h"

#include <math.h>

SSPro::SSPro(void)
{
	m_pDataT = NULL;
}

SSPro::~SSPro(void)
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
int SSPro::Open(string szPath)
{
	RVWPro::Open( szPath );

	///////////////

	if(szPath == "")
		return 1;

	m_szPath = szPath;

	memcpy( &m_SSHeader, m_pData, sizeof(tagSSHead));
	m_pDataT = m_pData + sizeof(tagSSHead);

	return 0;
}

int SSPro::OpenBuff( char *pBuff,int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen );
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_SSHeader, m_pData, sizeof(tagSSHead));
	m_pDataT = m_pData + sizeof(tagSSHead);

	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;

	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%.2f",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		0,
		ob.iProductNumber,
		ob.batch.scanmode,
		m_SSHeader.Area);

	m_szPath = string(cName);
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//! 获取风暴体个数
unsigned int SSPro::GetStormCount( )
{
	return m_SSHeader.StormNum;
}

//! 获取指定风暴体的风暴组件个数
unsigned int SSPro::GetStormComponentCount(int iIndex)
{
	STORMCELL *pStorm = (STORMCELL *)m_pDataT;

	if( iIndex < m_SSHeader.StormNum )
	{
		STORMCELL *pI = pStorm + iIndex;
		return pI->countOfComponent;
	}

	return 0; 
}

//! 获取指定风暴体的相关数据
STORMCELL *SSPro::GetStormCell(int iIndex)
{
	STORMCELL *pStorm = (STORMCELL *)m_pDataT;

	if( iIndex < m_SSHeader.StormNum )
		return (pStorm + iIndex);

	return NULL; 

}

//! 获取制定风暴体的相关风暴组件
STORMCOMPONENT *SSPro::GetStormComponentOfCell( int iIndex, float EL )
{
	STORMCELL *pStorm = (STORMCELL *)m_pDataT;

	unsigned char *pComponent = m_pDataT + sizeof(STORMCELL) * m_SSHeader.StormNum;

	if( iIndex < m_SSHeader.StormNum )
	{
		STORMCELL *pCell = pStorm + iIndex;

		unsigned int offset = pCell->offset;

		STORMCOMPONENT *pComp = (STORMCOMPONENT *)(pComponent + offset);

		unsigned int i = 0;

		for( i=0; i<pCell->countOfComponent; i++,pComp++ )
		{
			if( fabs(pComp->EL - EL) < 0.05 )
				return pComp;
		}

	}
	return NULL;
}

//! 获取制定风暴体的相关风暴组件
STORMCOMPONENT *SSPro::GetStormComponentOfCell( int iIndex)
{
	STORMCELL *pStorm = (STORMCELL *)m_pDataT;

	unsigned char *pComponent = m_pDataT + sizeof(STORMCELL) * m_SSHeader.StormNum;

	if( iIndex < m_SSHeader.StormNum )
	{
		STORMCELL *pCell = pStorm + iIndex;

		unsigned int offset = pCell->offset;

		STORMCOMPONENT *pComp = (STORMCOMPONENT *)(pComponent + offset);
		return pComp;
	}
	return NULL;
}