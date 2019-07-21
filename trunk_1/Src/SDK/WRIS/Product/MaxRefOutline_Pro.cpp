
#include <WRIS/product/MaxRefOutline_pro.h>
#include <WRIS/product/productid.h>

MaxRefOutlinePro::MaxRefOutlinePro(void)
{


}

MaxRefOutlinePro::~MaxRefOutlinePro(void)
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
int MaxRefOutlinePro::Open(string szPath)
{
	RVWPro::Open( szPath );
	//////////////////////////////////////////////////////////////////////////
	m_pDataV = m_pData + GetSizeOfRadial() * m_Header.obserSec.iRadialNum[0];
	unsigned char * pOutline;
	pOutline = m_pData+(GetSizeOfRadial()+GetSizeOfData())*m_Header.obserSec.iRadialNum[0];
	memcpy(&m_OutLine.iNum,pOutline,sizeof(int));
	memcpy(&m_OutLine.iPointNum,pOutline+sizeof(int),sizeof(int));
	int i=0;
	for (i=0;i<m_OutLine.iPointNum;i++)
	{
		tagPosition point;
		memcpy(&point,pOutline+sizeof(int)*2+i*sizeof(tagPosition),sizeof(tagPosition));
		m_OutLine.mPoit.push_back(point);
	}
	return 0;
}

int MaxRefOutlinePro::OpenBuff( char *pBuff,int iLen, bool bCopy )
{
	RVWPro::OpenBuff( pBuff,iLen, bCopy );
	//////////////////////////////////////////////////////////////////////////
	m_pDataV = m_pData + GetSizeOfRadial() * m_Header.obserSec.iRadialNum[0];
	unsigned char * pOutline;
	pOutline = m_pData+(GetSizeOfRadial()+GetSizeOfData())*m_Header.obserSec.iRadialNum[0];
	memcpy(&m_OutLine.iNum,pOutline,sizeof(int));
	memcpy(&m_OutLine.iPointNum,pOutline+sizeof(int),sizeof(int));
	int i=0;
	for (i=0;i<m_OutLine.iPointNum;i++)
	{
		tagPosition point;
		memcpy(&point,pOutline+sizeof(int)*2+i*sizeof(tagPosition),sizeof(tagPosition));
		m_OutLine.mPoit.push_back(point);
	}
	
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

unsigned char* MaxRefOutlinePro::GetHeightData( unsigned int iRadNum )
{
	return m_pDataV + GetSizeOfData( ) * iRadNum + sizeof(BASE_DATA_HEAD);
}

int MaxRefOutlinePro::GetSizeOfRadial()
{
	
	return m_Header.obserSec.usRefBinNumber[0] * sizeof(unsigned char) + sizeof(BASE_DATA_HEAD);

}

int MaxRefOutlinePro::GetSizeOfData()
{
	
	return m_Header.obserSec.usRefBinNumber[0] * sizeof(unsigned short int) + sizeof(BASE_DATA_HEAD);

}

