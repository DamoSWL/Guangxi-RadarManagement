﻿
#include "../../../include/WRIS/product/hpf_pro.h"

#define LEN 5

HPFPro::HPFPro(void)
:m_pFData(NULL)
{


}

HPFPro::~HPFPro(void)
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
int HPFPro::Open(string szPath)
{
	RVWPro::Open( szPath );
	//////////////////////////////////////////////////////////////////////////

	if(szPath == "")
		return 1;
	////////////////////////
	memcpy( &m_HPFHeader, m_pData, sizeof(__HPF_HEADER) );

	m_pFData = (float *)(m_pData + sizeof(__HPF_HEADER));


	return 0;
}

int HPFPro::OpenBuff( char *pBuff,int iLen)
{
	RVWPro::OpenBuff( pBuff, iLen);
	//////////////////////////////////////////////////////////////////////////
	memcpy( &m_HPFHeader, m_pData, sizeof(__HPF_HEADER) );
	m_pFData = (float *)(m_pData + sizeof(__HPF_HEADER));


	//"20100914_172743.00.003.004_0.00"
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;

	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%d-%d-%d",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		0,
		ob.iProductNumber,
		ob.batch.scanmode,
		(int)m_HPFHeader.VILLenOfWin,
		m_HPFHeader.cloudType,
		(int)m_HPFHeader.LenOfWin
		);

	m_szPath = string(cName);

	return 0;
}




