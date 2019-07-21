#include "../../../include/WRIS/product/vwp_pro.h"
#include "math.h"

VWPPro::VWPPro(void)
{
	maxVh = 0;  // 最大风速
	maxWind = 0;  // 最大风速对应的风向
	maxVhHeight = 0;  // 最大风速所在的高度

	m_iDisVWP = 12; // 最多显示12个体扫的VWP数据

	m_pMakeFileBuff = NULL;
}

VWPPro::~VWPPro(void)
{
	if( m_pMakeFileBuff ) {
		delete []m_pMakeFileBuff;
		m_pMakeFileBuff = NULL;
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

int VWPPro::Open(string szPath)
{
	RVWPro::Open(szPath);
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_VWPHeader, m_pData, sizeof(tagVWPHead));
	
	HorRan = m_VWPHeader.HorRan;
	fRMS = m_VWPHeader.fRMS;
	fSYM = m_VWPHeader.fSYM;
	if (!m_lsVWP.empty())
	{
		m_lsVWP.clear();
	}

	int sizePart = 30;  // 不同类数据间隔 30个float类型，体扫数据最多包含30个Cut
	int sizeVOL = 30 * sizeof(float); // 每个体扫的数据之间的间隔
	int i = 0;
	int ioff = sizeof(tagVWPHead);
	for( i=0; i<m_VWPHeader.VOLCnt; i++)
	{
		tagVWPData data;
		memcpy( &data.lTime, m_pData + ioff, sizeof(long));
		ioff += sizeof(long);

		float *pHei = (float *)(m_pData + ioff);
		float *pVh =  pHei + sizePart;
		float *pWind = pVh + sizePart;
		float *pFC = pWind + sizePart;

		memcpy( data.fHei, pHei, 30 * sizeof(float));
		memcpy( data.fVh, pVh, 30 * sizeof(float));
		memcpy( data.fWind, pWind, 30 * sizeof(float));
		memcpy( data.fFC, pFC, 30 * sizeof(float));
		m_lsVWP.push_back( data );
		ioff += sizeVOL*4;
	}
	if( fabs(m_VWPHeader.maxVh) > fabs(maxVh) )
	{
		maxVh = m_VWPHeader.maxVh;
		maxWind = m_VWPHeader.maxWind;
		maxVhHeight = m_VWPHeader.maxVhHeight;
	}

	int iCntPop = m_lsVWP.size() - m_iDisVWP;
	if( iCntPop > 0)
	{
		for(i=iCntPop; i>0; i--) {
			if( m_lsVWP.size() > 0 )
				m_lsVWP.pop_front();
		}
	}

	return 0;
}

int VWPPro::OpenBuff( char *pBuff, int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen);
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_VWPHeader, m_pData, sizeof(tagVWPHead));

	HorRan = m_VWPHeader.HorRan;
	fRMS = m_VWPHeader.fRMS;
	fSYM = m_VWPHeader.fSYM;
	if (!m_lsVWP.empty())
	{
		m_lsVWP.clear();
	}
	int sizePart = 30;  // 不同类数据间隔 30个float类型，体扫数据最多包含30个Cut
	int sizeVOL = 30 * sizeof(float); // 每个体扫的数据之间的间隔 , 4类数据，每类30个，float 类型
	int i = 0;
	int ioff = sizeof(tagVWPHead);
	for( i=0; i<m_VWPHeader.VOLCnt; i++)
	{
		tagVWPData data;
		memcpy( &data.lTime, m_pData + ioff, sizeof(long));
		ioff += sizeof(long);

		float *pHei = (float *)(m_pData + ioff);
		float *pVh =  pHei + sizePart;
		float *pWind = pVh + sizePart;
		float *pFC = pWind + sizePart;

		memcpy( data.fHei, pHei, 30 * sizeof(float));
		memcpy( data.fVh, pVh, 30 * sizeof(float));
		memcpy( data.fWind, pWind, 30 * sizeof(float));
		memcpy( data.fFC, pFC, 30 * sizeof(float));
		m_lsVWP.push_back( data );
		ioff += sizeVOL*4;
	}
	if( fabs(m_VWPHeader.maxVh) > fabs(maxVh) )
	{
		maxVh = m_VWPHeader.maxVh;
		maxWind = m_VWPHeader.maxWind;
		maxVhHeight = m_VWPHeader.maxVhHeight;
	}

	int iCntPop = m_lsVWP.size() - m_iDisVWP;
	if( iCntPop > 0)
	{
		for(i=iCntPop; i>0; i--) {
			if( m_lsVWP.size() > 0 )
				m_lsVWP.pop_front();
		}
	}


	//"20100914_172743.00.003.004_0.00"
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;

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
		m_VWPHeader.HorRan,
		m_VWPHeader.fRMS,
		m_VWPHeader.fSYM
		);

	m_szPath = string(cName);

	return 0;
}

int VWPPro::MakeVWPFile( unsigned char **ppData )
{
	if( m_pMakeFileBuff ) {
		delete []m_pMakeFileBuff;
		m_pMakeFileBuff = NULL;
	}
	
	size_t sizePart = 30;  // 不同类数据间隔 30个float类型，体扫数据最多包含30个Cut
	size_t sizeVOL = 4 *30 * sizeof(float); // 每个体扫的数据之间的间隔 , 4类数据，每类30个，float 类型

	size_t cntVWP = m_lsVWP.size();
	size_t i = 0;
	size_t lenBuf = 
		sizeof(tagRealFile) + 
		sizeof(tagVWPHead) +
		sizeof(long) * cntVWP +
		sizeVOL * cntVWP;
	
	m_pMakeFileBuff = new unsigned char[ lenBuf ];
	unsigned char *pBuf = m_pMakeFileBuff;

	if( m_pMakeFileBuff )
	{
		memcpy( pBuf, &m_Header, sizeof(tagRealFile) );  pBuf += sizeof(tagRealFile);
		memcpy( pBuf, &m_VWPHeader, sizeof(tagVWPHead)); pBuf += sizeof(tagVWPHead);
		
		long *pTime = (long *)pBuf;
		
		std::list< tagVWPData >::iterator it,itE;
		it = m_lsVWP.begin();
		itE = m_lsVWP.end();
		i = 0;
		while( it != itE )
		{
			pTime[i] = it->lTime;
			/////
			it++;i++;
		}

		pBuf += sizeof(long) * cntVWP;

		it = m_lsVWP.begin();
		itE = m_lsVWP.end();
		i = 0;
		while( it != itE )
		{
			memcpy( pBuf, it->fHei, sizeVOL );
			pBuf += sizeVOL;
			/////
			it++;
		}

		(*ppData) = m_pMakeFileBuff;
		return lenBuf;
	}

	return -1;
}

