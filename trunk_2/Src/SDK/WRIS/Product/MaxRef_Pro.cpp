
#include <WRIS/product/maxref_pro.h>
#include <WRIS/product/productid.h>

MaxRefPro::MaxRefPro(void)
{


}

MaxRefPro::~MaxRefPro(void)
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
int MaxRefPro::Open(string szPath)
{
	RVWPro::Open( szPath );
	//////////////////////////////////////////////////////////////////////////
	m_pDataV = m_pData + GetSizeOfRadial() * m_Header.obserSec.iRadialNum[0];
	return 0;
}

int MaxRefPro::OpenBuff( char *pBuff,int iLen, bool bCopy )
{
	RVWPro::OpenBuff( pBuff,iLen, bCopy );
	//////////////////////////////////////////////////////////////////////////
	m_pDataV = m_pData + GetSizeOfRadial() * m_Header.obserSec.iRadialNum[0];
	
	/// 生成产品名称，含区别码
	//// 20101115_172857.00.010.004
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;

	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		0,
		ob.iProductNumber,
		ob.batch.scanmode
		);

	m_szPath = string(cName);
	
	return 0;
}

unsigned char* MaxRefPro::GetHeightData( unsigned int iRadNum )
{
	return m_pDataV + GetSizeOfData( ) * iRadNum + sizeof(BASE_DATA_HEAD);
}

int MaxRefPro::GetSizeOfRadial()
{
	switch ( m_Header.obserSec.iProductNumber )
	{
	case PID_ET:
	case PID_EB:
		return m_Header.obserSec.usRefBinNumber[0] * sizeof(unsigned short int) + sizeof(BASE_DATA_HEAD);
	case PID_MAXREF:
		return m_Header.obserSec.usRefBinNumber[0] * sizeof(unsigned char) + sizeof(BASE_DATA_HEAD);
	}
	
	return 0;
}

int MaxRefPro::GetSizeOfData()
{
	switch ( m_Header.obserSec.iProductNumber )
	{
	case PID_ET:
	case PID_EB:
		return m_Header.obserSec.usRefBinNumber[0] * sizeof(unsigned char) + sizeof(BASE_DATA_HEAD);
	case PID_MAXREF:
		return m_Header.obserSec.usRefBinNumber[0] * sizeof(unsigned short int) + sizeof(BASE_DATA_HEAD);
	}

	return 0;
}

