
#include "stdio.h"

#include <WRIS/product/rvw_pro.h>
#include "../../../include/WRIS/product/productid.h"


#define LEN 5

RVWPro::RVWPro(void):
	m_uiDataLen(0),m_pData(NULL)
{
	m_bDataIsCopy = false;
	m_pAData = NULL;
	m_szPath = "";
	memset(&m_Header, 0, sizeof(tagRealFile));
}

RVWPro::~RVWPro(void)
{
	if( m_bDataIsCopy && (m_pAData != NULL))
	{
		delete []m_pAData;
		m_pAData = NULL;
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
int RVWPro::Open(string szPath)
{
	if(szPath == "")
		return 1;

	m_szPath = szPath;

	FILE *fp = fopen(szPath.c_str(), "rb");
	if( fp != NULL )
	{
		long lSize = 0;
		fseek( fp, 0, SEEK_END);
		lSize = ftell( fp );
		fseek( fp, 0 ,SEEK_SET);

		size_t lRead = fread( &m_Header, sizeof(tagRealFile), 1, fp );
		if(lRead != 1)	
			return 3;		

		lSize = lSize - sizeof(tagRealFile);

		if(m_pData != NULL) {
			delete []m_pData;
			m_pData = NULL;
		}

		m_pData = new unsigned char[lSize];
		lRead = fread( m_pData, lSize, 1, fp);
		if( lRead != 1)	
			return 3;

		fclose(fp);
	}

	/////////////////////////
	// 兼容老格式的数据  == 0 表示FL的数据， < 表示老数据
	if( (m_Header.addSec.Type[4]=='F' && m_Header.addSec.Type[5]=='L') || 
		(m_Header.addSec.Type[4]==0 && m_Header.addSec.Type[5]==0))
	{
		if( m_Header.obserSec.usRefBinNumber[0] < m_Header.obserSec.iBinNumber[0] )
		{
			// 转换为新的格式
			switch( m_Header.obserSec.iProductNumber )
			{
			case 4:
			case 5:
				{
					m_Header.obserSec.batch.usDopBinNumber[0] = m_Header.obserSec.iBinNumber[0];
					m_Header.obserSec.iDopBinLen[0] = m_Header.obserSec.iRefBinLen[0];
					m_Header.obserSec.usRefBinNumber[0] = 0;
					m_Header.obserSec.iRefBinLen[0] = 0;
				}
				break;
			default:
				{
					m_Header.obserSec.usRefBinNumber[0] = m_Header.obserSec.iBinNumber[0];
					m_Header.obserSec.iRefBinLen[0] = m_Header.obserSec.iRefBinLen[0];
					m_Header.obserSec.batch.usDopBinNumber[0] = 0;
					m_Header.obserSec.iDopBinLen[0] = 0;
				}
				break;
			}
		}
	}
	return 0;
}

int RVWPro::OpenBuff( char *pBuff,int iLen, bool bCopy )
{
	m_uiDataLen = iLen;

	if( m_bDataIsCopy && (m_pAData != NULL))
	{
		delete []m_pAData;
		m_pAData = NULL;
	}

	if( !bCopy )
	{
		m_pAData = (unsigned char*)pBuff;
		m_bDataIsCopy = false;
	}
	else
	{
		m_pAData = new unsigned char[iLen];
		memcpy( m_pAData, pBuff, iLen);
		m_bDataIsCopy = true;
	}

	if( iLen <= sizeof(tagRealFile) )  return 3;
	memcpy( &m_Header, pBuff, sizeof(tagRealFile) );

	m_pData = m_pAData + sizeof(tagRealFile);

	/////////////////////////
	// 兼容老格式的数据
	// 兼容老格式的数据 < 表示老数据
	if( (m_Header.addSec.Type[4]=='F' && m_Header.addSec.Type[5]=='L') || 
		(m_Header.addSec.Type[4]==0 && m_Header.addSec.Type[5]==0))
	{
		if( m_Header.obserSec.usRefBinNumber[0] < m_Header.obserSec.iBinNumber[0] )
		{
			switch( m_Header.obserSec.iProductNumber )
			{
			case 4:
			case 5:
				{
					m_Header.obserSec.batch.usDopBinNumber[0] = m_Header.obserSec.iBinNumber[0];
					m_Header.obserSec.iDopBinLen[0] = m_Header.obserSec.iRefBinLen[0];
					m_Header.obserSec.usRefBinNumber[0] = 0;
					m_Header.obserSec.iRefBinLen[0] = 0;
				}
				break;
			default:
				{
					m_Header.obserSec.usRefBinNumber[0] = m_Header.obserSec.iBinNumber[0];
					m_Header.obserSec.iRefBinLen[0] = m_Header.obserSec.iRefBinLen[0];
					m_Header.obserSec.batch.usDopBinNumber[0] = 0;
					m_Header.obserSec.iDopBinLen[0] = 0;
				}
				break;
			}
		}
	}

	////////////////////////
	//"20100914_172743.00.003.004_0.00"
	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;


	int iLayer = 0;

	switch( ob.iProductNumber )
	{
	case 3:
	case 4:
	case 5:
	case 52:
		{
			iLayer = ob.iElevation[1];
		}
		break;
	}


	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%.2f",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		iLayer,
		ob.iProductNumber,
		ob.batch.scanmode,
		(float)ob.iElevation[0]/100.f
		);

	m_szPath = string(cName);

	return 0;
}


short int RVWPro::GetMaxD( )
{
	switch( m_Header.obserSec.iProductNumber )
	{
	case 4:
	case 5:
		return m_Header.obserSec.batch.usDopBinNumber[0] * m_Header.obserSec.iRefBinLen[0] / 1000;
	default:
		return m_Header.obserSec.usRefBinNumber[0] * m_Header.obserSec.iRefBinLen[0] / 1000;
	}
}

unsigned short int RVWPro::GetBinWidth()
{	
	switch( m_Header.obserSec.iProductNumber )
	{
		case 4:
		case 5:
			return m_Header.obserSec.iDopBinLen[0];
		default:
			return m_Header.obserSec.iRefBinLen[0];
	}
}

unsigned int RVWPro::GetSizeOfRadial( )
{
	switch( m_Header.obserSec.iProductNumber )
	{
	case 4:
	case 5:
		return m_Header.obserSec.batch.usDopBinNumber[0] + sizeof(BASE_DATA_HEAD);
		break;
	case PID_PPDP:
		return m_Header.obserSec.batch.usDopBinNumber[0]*2 + sizeof(BASE_DATA_HEAD);
		break;
	default:
		return m_Header.obserSec.usRefBinNumber[0] + sizeof(BASE_DATA_HEAD);
	}
}


unsigned int RVWPro::GetRangeBinOfRad()
{
	switch( m_Header.obserSec.iProductNumber )
	{
	case 4:
	case 5:
		return m_Header.obserSec.batch.usDopBinNumber[0];
		break;
	default:
		return m_Header.obserSec.usRefBinNumber[0];
	}
}

//! 获取指定Radial的数据
unsigned char* RVWPro::GetRadialData( unsigned int iRadNum )
{
	return m_pData + GetSizeOfRadial( ) * iRadNum + sizeof(BASE_DATA_HEAD);
}

//! 获取指定径向的径向头
BASE_DATA_HEAD *RVWPro::GetRadialHead( unsigned int iRadNum )
{
	return (BASE_DATA_HEAD *)(m_pData + GetSizeOfRadial( ) * iRadNum);
}