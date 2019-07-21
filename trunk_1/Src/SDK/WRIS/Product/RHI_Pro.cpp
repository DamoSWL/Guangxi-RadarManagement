#include "../../../include/WRIS/product/RHI_Pro.h"
#include "../../../include/WRIS/product/productnamephrase.h"
#include "../../../include/WRIS/product/productid.h"

RHIPro::RHIPro(void)
{
	m_pData = NULL;
	m_pAData = NULL;
	m_szPath = "";

	memset(&m_Header, 0, sizeof(tagRealFile));
}

RHIPro::~RHIPro(void)
{
	if(m_pAData != NULL)
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
int RHIPro::Open(string szPath)
{
	if(szPath == "")
		return 1;

	m_szPath = szPath;

	FILE *fp = fopen( szPath.c_str(), "rb");
	if( fp == NULL ) return 2;

	fseek(fp , 0, SEEK_END);
	long lSize = ftell( fp );
	if( lSize == 0 ) {
		fclose( fp);
		return 2;
	}	

	fseek( fp, 0, SEEK_SET );

	unsigned char *pData = new unsigned char[lSize];
	fread( pData, lSize, 1, fp );
	fclose( fp );

	int iRet = OpenBuff( (char*)pData, lSize );

	delete []pData;

	if( iRet == 0 ){
		CProductName name( szPath );
		m_Header.obserSec.iObsStartTimeYear = name.GetYear();
		m_Header.obserSec.iObsStartTimeMonth = name.GetMonth();
		m_Header.obserSec.iObsStartTimeDay = name.GetDay();
		m_Header.obserSec.iObsStartTimeHour = name.GetHour();
		m_Header.obserSec.iObsStartTimeMinute = name.GetMin();
		m_Header.obserSec.iObsStartTimeSecond = name.GetSec();
	}

	return iRet;
}
//
int RHIPro::OpenBuff( char *pBuff,int iLen)
{

	m_dataSize = iLen;

	if(m_pAData != NULL) {
		delete []m_pAData;
		m_pAData = NULL;
	}

	m_pAData = new unsigned char[iLen];
	memcpy( m_pAData, pBuff, iLen);

	if( iLen <= sizeof(tagRealFile) )  return 3;
	memcpy( &m_Header, pBuff, sizeof(tagRealFile) );

	m_pData = m_pAData + sizeof(tagRealFile);

	//// 兼容老格式
	//! 新格式的usRefBinNumber在老格式中的为iZuiDaCeJu,
	   //! 因为绵阳的数据是一公里八个点，所以在老格式中， iZuiDaCeJu < m_Header.obserSec.iBinNumber
	   //! 格式的最大测距是大于径向库数的
	//////////////////////////////////////////////////////////////////////////
	// 替换为新的格式
	// 兼容老格式的数据
	if( (m_Header.addSec.Type[4]=='F' && m_Header.addSec.Type[5]=='L') || 
		(m_Header.addSec.Type[4]==0 && m_Header.addSec.Type[5]==0))
	{
		int i = 0;
		for(i=0;i<m_Header.obserSec.iScanCutNum;i++)
		{
			switch( m_Header.obserSec.batch.wavForm[i] )
			{
			case 0: // R
				{
					if( m_Header.obserSec.usRefBinNumber[i] < m_Header.obserSec.iBinNumber[i] )
					{
						m_Header.obserSec.usRefBinNumber[i] = m_Header.obserSec.usRefBinNumber[i] * 8;
						m_Header.obserSec.iRefBinLen[i] = m_Header.obserSec.iRefBinLen[i];
						m_Header.obserSec.batch.usDopBinNumber[i] = 0;
						m_Header.obserSec.iDopBinLen[i] = 0;
					}
				}
				break;
			case 1: // V W
				{
					if( m_Header.obserSec.batch.usDopBinNumber[i] < m_Header.obserSec.iBinNumber[i] )
					{
						m_Header.obserSec.batch.usDopBinNumber[i] = m_Header.obserSec.batch.usDopBinNumber[i]*8;
						m_Header.obserSec.iDopBinLen[i] = m_Header.obserSec.iRefBinLen[i];
						m_Header.obserSec.usRefBinNumber[i] = 0;
						m_Header.obserSec.iRefBinLen[i] = 0;
					}
				}
				break;
			case 2:// R V W
				{
					if( m_Header.obserSec.usRefBinNumber[i] < m_Header.obserSec.iBinNumber[i] )
					{
						m_Header.obserSec.usRefBinNumber[i] = m_Header.obserSec.usRefBinNumber[i] * 8;
						m_Header.obserSec.iRefBinLen[i] = m_Header.obserSec.iRefBinLen[i];
						m_Header.obserSec.batch.usDopBinNumber[i] = m_Header.obserSec.batch.usDopBinNumber[i] * 8;
						m_Header.obserSec.iDopBinLen[i] = m_Header.obserSec.iRefBinLen[i];
					}
				}
				break;
			case 4: // R V W
			case 5:
			case 6:
			case 8:
				{
					if( m_Header.obserSec.usRefBinNumber[i] < m_Header.obserSec.iBinNumber[i] )
					{
						m_Header.obserSec.usRefBinNumber[i] = m_Header.obserSec.usRefBinNumber[i] * 8;
						m_Header.obserSec.iRefBinLen[i] = m_Header.obserSec.iRefBinLen[i];
						m_Header.obserSec.batch.usDopBinNumber[i] = m_Header.obserSec.batch.usDopBinNumber[i] * 8;
						m_Header.obserSec.iDopBinLen[i] = m_Header.obserSec.iRefBinLen[i];;
					}
				}
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//"20100914_172743.00.003.004_0.00"
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

void RHIPro::Close()
{
	if(m_pData != NULL)
		delete []m_pData;

}

 //得到指定类型数据在径向数据中的偏移
unsigned int RHIPro::GetProductRadialOffset(unsigned int type)
{	
	char wavForm = m_Header.obserSec.batch.wavForm[0];
	
	switch( wavForm)
	{
	case 7:
		{
			if( type == PID_REF )
				return sizeof( BASE_DATA_HEAD);
			else if( type == PID_VEL )
				return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[0];
			else if( type == PID_WID )
				return sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0] + 
				m_Header.obserSec.batch.usDopBinNumber[0];
		}
		break;
	case 0:// LCS,R
		{
			if( type == PID_REF)
				return sizeof(BASE_DATA_HEAD);
		}
		break;
	case 1: // LCD, V,W
		{
			if( type == PID_VEL )
				return sizeof(BASE_DATA_HEAD);
			else if( type == PID_WID )
				return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.batch.usDopBinNumber[0];
		}
		break;
	case 2: //HCD, R, V,W
	case 4:
	case 5:
	case 6:
		{
			if( type == PID_REF )
				return sizeof( BASE_DATA_HEAD);
			else if( type == PID_VEL )
				return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[0];
			else if( type == PID_WID )
				return sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0] + 
				m_Header.obserSec.batch.usDopBinNumber[0];
		}
		break;
	case 8:
		{
			if( type == PID_REF )
				return sizeof( BASE_DATA_HEAD);
			else if( type == PID_VEL )
				return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[0];
			else if( type == PID_WID )
				return sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0] + 
				m_Header.obserSec.batch.usDopBinNumber[0];
			else if( type == PID_PZDR )
				return sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0]*2 + 
				m_Header.obserSec.batch.usDopBinNumber[0]*2;
			else if( type == PID_PKDP )
				return sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0]*3 + 
				m_Header.obserSec.batch.usDopBinNumber[0]*2;
			else if( type == PID_PRHV )
				return sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0]*4 + 
				m_Header.obserSec.batch.usDopBinNumber[0]*2;
			else if( type == PID_PPDP )
				return sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0]*5 + 
				m_Header.obserSec.batch.usDopBinNumber[0]*2;
		}
		break;
	}

	return -1;
}
////
////
unsigned int RHIPro::GetProductTypeOfCut()
{
	char wavForm = m_Header.obserSec.batch.wavForm[0];

	switch( wavForm)
	{
	case 0:// LCS,R
		return PID_REF;
		break;
	case 1: // LCD, V,W
			return PID_VEL;			
		break;
	case 2: //R, V,W
	case 4:
	case 5:
	case 6:
	case 8:
		return PID_REF;
		break;
	}

	return 999;
}

unsigned int RHIPro::GetSizeOfRadial( )
{
	char wavForm = m_Header.obserSec.batch.wavForm[0];

	switch( wavForm)
	{
	case 7:
		{
			return sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0] + 
				m_Header.obserSec.batch.usDopBinNumber[0] * 2 +
				m_Header.obserSec.usRefBinNumber[0];
		}
		break;
	case 0:// LCS,R
		{
			return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[0];
		}
		break;
	case 1: // LCD, V,W
		{
			return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.batch.usDopBinNumber[0] *2;
		}
		break;
	case 2: //HCD, R, V,W
	case 4:
	case 5:
	case 6:
		{
			return sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0] + 
				m_Header.obserSec.batch.usDopBinNumber[0] * 2;
		}
		break;
	case 8:
		{
			return sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0]*7 + 
				m_Header.obserSec.batch.usDopBinNumber[0] * 2;
		}
		break;
	}

	return 0;
}
//
//
unsigned int RHIPro::GetRangeBinOfRad( unsigned int type )
{
	char wavForm = m_Header.obserSec.batch.wavForm[0];

	switch( wavForm)
	{
	case 0:// LCS,R
		{
			if( type == PID_REF )
				return m_Header.obserSec.usRefBinNumber[0];
		}
		break;
	case 1: // LCD, V,W
		{
			if( type == PID_VEL || type == PID_WID )
				return m_Header.obserSec.batch.usDopBinNumber[0];
		}
		break;
	case 2: //HCD, R, V,W
	case 4:
	case 5:
	case 6:
		{
			if( type == PID_REF )
				return m_Header.obserSec.usRefBinNumber[0];
			else
				return m_Header.obserSec.batch.usDopBinNumber[0];
		}
		break;
	case 7:
		{
			if( type == PID_REF )
				return m_Header.obserSec.usRefBinNumber[0];
			else
				return m_Header.obserSec.batch.usDopBinNumber[0];
		}
	case 8:
		{
			if( (type == PID_REF)
				|| (type == PID_PZDR)|| (type == PID_PPDP)
				|| (type == PID_PKDP)|| (type == PID_PRHV) )
				return m_Header.obserSec.usRefBinNumber[0];
			else
				return m_Header.obserSec.batch.usDopBinNumber[0];
		}
		break;
	}

	return 0;
}
//
unsigned char * RHIPro::GetRadialData( int cutIndex, int radNum, int proType )
{
	int cut = cutIndex;
	unsigned char *pData = m_pData;
	unsigned long offset = GetCutPos() - sizeof(tagRealFile);

	unsigned short wavForm = m_Header.obserSec.batch.wavForm[0];
	unsigned long sizeOfRadail = GetSizeOfRadial();
	
	switch( wavForm)
	{
	case 0: // lcs
		{
			if( proType == PID_REF ) 
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
		}
		break;
	case 1: // lcd
		{
			if( proType == PID_VEL )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
			else if(proType == PID_WID)
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + m_Header.obserSec.batch.usDopBinNumber[0];				
		}
		break;
	case 2: // hcd
	case 4: // batch
	case 5:
	case 6:
		{
			if( proType == PID_REF )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
			else if( proType == PID_VEL )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[0];
			else if( proType == PID_WID )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + 
					m_Header.obserSec.usRefBinNumber[0] +
					m_Header.obserSec.batch.usDopBinNumber[0];
		}
		break;
	case 8:
		{
			if( proType == PID_REF )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
			else if( proType == PID_VEL )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[0];
			else if( proType == PID_WID )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0] +
				m_Header.obserSec.batch.usDopBinNumber[0];
			else if( proType == PID_PZDR )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0]*2 +
				m_Header.obserSec.batch.usDopBinNumber[0]*2;
			else if( proType == PID_PKDP )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0]*3 +
				m_Header.obserSec.batch.usDopBinNumber[0]*2;
			else if( proType == PID_PRHV )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0]*4 +
				m_Header.obserSec.batch.usDopBinNumber[0]*2;
			else if( proType == PID_PPDP )
				pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + 
				m_Header.obserSec.usRefBinNumber[0]*5 +
				m_Header.obserSec.batch.usDopBinNumber[0]*2;
		}
		break;
	}

	return pData;
}

BASE_DATA_HEAD *RHIPro::GetRadialHeader(int cutIndex, int radNum)
{
	unsigned char *pData = m_pData;
	unsigned long offset = GetCutPos() - sizeof(tagRealFile);
	unsigned long sizeOfRadail = GetSizeOfRadial();

	return (BASE_DATA_HEAD *)(pData + offset + sizeOfRadail * radNum);
}
//
short int RHIPro::GetMaxD( int proType )
{
	unsigned short wavForm = m_Header.obserSec.batch.wavForm[0];
	switch( wavForm)
	{
	case 0: // lcs
	case 2: // hcd
		return m_Header.obserSec.usRefBinNumber[0] * m_Header.obserSec.iRefBinLen[0] / 1000;
	case 1: // lcd
		return m_Header.obserSec.batch.usDopBinNumber[0] * m_Header.obserSec.iDopBinLen[0]/1000;
	case 3: // batch
	case 8:
		{
			if( (proType == PID_VEL) || (proType == PID_WID) )
				return m_Header.obserSec.batch.usDopBinNumber[0] * m_Header.obserSec.iDopBinLen[0]/1000;	
			else
				return m_Header.obserSec.usRefBinNumber[0] * m_Header.obserSec.iRefBinLen[0] / 1000;		
		}
		break;
	}

	return m_Header.obserSec.usRefBinNumber[0] * m_Header.obserSec.iRefBinLen[0] / 1000;		
}

unsigned short int RHIPro::GetBinWidth(unsigned int iProType)
{
	if( iProType == PID_REF )
		return m_Header.obserSec.iRefBinLen[0];
	else
		return m_Header.obserSec.iDopBinLen[0];

}