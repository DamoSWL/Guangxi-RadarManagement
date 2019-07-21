#include "../../../include/WRIS/product/vol_pro.h"
#include "../../../include/WRIS/product/productnamephrase.h"
#include "../../../include/WRIS/product/productid.h"

#define LEN 5

VOLPro::VOLPro(void)
{
	m_pData = NULL;
	m_pAData = NULL;
	m_szPath = "";

	memset(&m_Header, 0, sizeof(tagRealFile));
}

VOLPro::~VOLPro(void)
{
	if(m_pAData != NULL)
	{
		delete []m_pAData;
		m_pAData = NULL;
	}
}

//************************************
// Method:    Open
// FullName:  VOLPro::Open
// Access:    public 
// Returns:   int
// Qualifier: 打开数据文件
// Parameter: string szPath
//************************************
int VOLPro::Open(string szPath)
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

//************************************
// Method:    OpenBuff
// FullName:  VOLPro::OpenBuff
// Access:    public 
// Returns:   int
// Qualifier: 打开文件缓存
// Parameter: char * pBuff
// Parameter: int iLen
//************************************
int VOLPro::OpenBuff( char *pBuff,int iLen)
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
			case FILE_WF_Z_0: // R
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
			case FILE_WF_VW_1: // V W
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
			case FILE_WF_ZVW_2:// R V W
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
			case FILE_WF_ZVW_4: // R V W
			case FILE_WF_ZVW_5:
			case FILE_WF_ZVW_6:
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
			default://其它格式老风灵未生成，不需要转换
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

//************************************
// Method:    Close
// FullName:  VOLPro::Close
// Access:    public 
// Returns:   void
// Qualifier: 释放内存
//************************************
void VOLPro::Close()
{
	if (m_pData != NULL) {
		delete[]m_pData; m_pData = NULL;
	}

}

// 得到指定类型数据在径向数据中的偏移
unsigned int VOLPro::GetProductRadialOffset( unsigned int cut, unsigned int type)
{	
	char wavForm = m_Header.obserSec.batch.wavForm[cut];
	
	switch( wavForm)
	{
	case FILE_WF_ZVWH_7:
		{
			if (type == PID_REF)
				return sizeof(BASE_DATA_HEAD);
			else if (type == PID_VEL)
				return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
			else if (type == PID_WID)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut];
			else if (type == PID_HCL)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
		}
		break;
	case FILE_WF_Z_0:// LCS,R
		{
			if (type == PID_REF)
				return sizeof(BASE_DATA_HEAD);
		}		
		break;
	case FILE_WF_VW_1: // LCD, V,W
		{
			if (type == PID_VEL)
				return sizeof(BASE_DATA_HEAD);
			else if (type == PID_WID)
				return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.batch.usDopBinNumber[cut];
		}
		break;
	case FILE_WF_ZVW_2: //HCD, R, V,W
	case FILE_WF_ZVW_4:
	case FILE_WF_ZVW_5:
	case FILE_WF_ZVW_6:
		{
			if (type == PID_REF)
				return sizeof(BASE_DATA_HEAD);
			else if (type == PID_VEL)
				return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
			else if (type == PID_WID)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut];
		}
		break;	
	case FILE_WF_ZVWHZKRP_8:
		{
			if (type == PID_REF)
				return sizeof(BASE_DATA_HEAD);
			else if (type == PID_VEL)
				return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
			else if (type == PID_WID)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut];
			else if (type == PID_HCL)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (type == PID_PZDR)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 2 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (type == PID_PKDP)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 3 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (type == PID_PRHV)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 4 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (type == PID_PPDP)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 5 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
		}
		break;
	case FILE_WF_ZVWC_9:
		{
			if (type == PID_REF)
				return sizeof(BASE_DATA_HEAD);
			else if (type == PID_VEL)
				return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
			else if (type == PID_WID)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut];
			else if (type == PID_CORZ)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
		}
		break;
	case FILE_WF_ZVWHZKRPC_10:
		{
			if (type == PID_REF)
				return sizeof(BASE_DATA_HEAD);
			else if (type == PID_VEL)
				return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
			else if (type == PID_WID)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut];
			else if (type == PID_HCL)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (type == PID_PZDR)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 2 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (type == PID_PKDP)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 3 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (type == PID_PRHV)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 4 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (type == PID_PPDP)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 5 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (type == PID_CORZ)
				return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 5 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2 +
				m_Header.obserSec.usRefBinNumber[cut] * 2;//差分相位占两字节
		}
		break;
	}

	return -1;
}


//************************************
// Method:    GetProductTypeOfCut
// FullName:  VOLPro::GetProductTypeOfCut
// Access:    public 
// Returns:   unsigned int
// Qualifier: 获取指定仰角层第一个产品数据类型
// Parameter: unsigned int cut
//************************************
unsigned int VOLPro::GetProductTypeOfCut(unsigned int cut)
{
	char wavForm = m_Header.obserSec.batch.wavForm[cut];

	switch( wavForm)
	{
	case FILE_WF_Z_0:// LCS,R
		return PID_REF;
		break;
	case FILE_WF_VW_1: // LCD, V,W
		return PID_VEL;			
		break;
	case FILE_WF_ZVW_2: //R, V,W
	case FILE_WF_ZVW_4:
	case FILE_WF_ZVW_5:
	case FILE_WF_ZVW_6:
	case FILE_WF_ZVWH_7:
	case FILE_WF_ZVWHZKRP_8:
	case FILE_WF_ZVWC_9:
	case FILE_WF_ZVWHZKRPC_10:
		return PID_REF;
		break;
	}

	return 999;
}

//************************************
// Method:    GetSizeOfRadial
// FullName:  VOLPro::GetSizeOfRadial
// Access:    public 
// Returns:   unsigned int
// Qualifier: 获取指定仰角层径向数据大小（包括径向头以及径向数据）
// Parameter: unsigned int cut
//************************************
unsigned int VOLPro::GetSizeOfRadial( unsigned int cut )
{
	char wavForm = m_Header.obserSec.batch.wavForm[cut];

	switch (wavForm)
	{
	case FILE_WF_ZVWH_7:
		{
			return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2 +
				m_Header.obserSec.usRefBinNumber[cut];
		}
		break;
	case FILE_WF_Z_0:// LCS,R
		{
			return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
		}
		break;
	case FILE_WF_VW_1: // LCD, V,W
		{
			return sizeof(BASE_DATA_HEAD) + m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
		}
		break;
	case FILE_WF_ZVW_2: //HCD, R, V,W
	case FILE_WF_ZVW_4:
	case FILE_WF_ZVW_5:
	case FILE_WF_ZVW_6:
		{
			return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
		}
		break;
	case FILE_WF_ZVWHZKRP_8:
		{
			return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 7 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;//PDP数据占16位
		}
		break;
	case FILE_WF_ZVWC_9:
		{
			return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2 +
				m_Header.obserSec.usRefBinNumber[cut];
		}
		break;
	case FILE_WF_ZVWHZKRPC_10:
		{
			return sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 7 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2 +//PDP数据占16位
				m_Header.obserSec.usRefBinNumber[cut];
		}
		break;
	}
	return 0;
}


//************************************
// Method:    GetRangeBinOfRad
// FullName:  VOLPro::GetRangeBinOfRad
// Access:    public 
// Returns:   unsigned int
// Qualifier: 获取指定仰角层指定产品距离库数
// Parameter: unsigned int cut
// Parameter: unsigned int type
//************************************
unsigned int VOLPro::GetRangeBinOfRad(unsigned int cut, unsigned int type)
{
	char wavForm = m_Header.obserSec.batch.wavForm[cut];

	switch( wavForm)
	{
	case FILE_WF_Z_0:// LCS,R
		{
			if (type == PID_REF)
				return m_Header.obserSec.usRefBinNumber[cut];
		}
		break;
	case FILE_WF_VW_1: // LCD, V,W
		{
			if (type == PID_VEL || type == PID_WID)
				return m_Header.obserSec.batch.usDopBinNumber[cut];
		}
		break;
	case FILE_WF_ZVW_2: //HCD, R, V,W
	case FILE_WF_ZVW_4:
	case FILE_WF_ZVW_5:
	case FILE_WF_ZVW_6:
	case FILE_WF_ZVWH_7:
	case FILE_WF_ZVWHZKRP_8:
	case FILE_WF_ZVWC_9:
	case FILE_WF_ZVWHZKRPC_10:
		{
			if (type == PID_VEL || type == PID_WID)
				return m_Header.obserSec.batch.usDopBinNumber[cut];
			else
				return m_Header.obserSec.usRefBinNumber[cut];
		}
		break;
	}

	return 0;
}

//************************************
// Method:    GetRadialData
// FullName:  VOLPro::GetRadialData
// Access:    public 
// Returns:   unsigned char *
// Qualifier: 获取指定仰角、指定径向、指定产品数据指针,若从未经衰减订正的老数据中获取反射率，返回老数据指针（保证算法兼容）
// Parameter: int cutIndex
// Parameter: int radNum
// Parameter: int proType
//************************************
unsigned char * VOLPro::GetRadialData( int cutIndex, int radNum, int proType )
{
	int cut = cutIndex;
	unsigned char *pData = m_pData;
	unsigned long offset = GetCutPos( cut) - sizeof(tagRealFile);
	unsigned short wavForm = m_Header.obserSec.batch.wavForm[cut];
	unsigned long sizeOfRadail = GetSizeOfRadial( cut );
	
	switch( wavForm)
	{
	case FILE_WF_Z_0: // lcs
		{
			if (proType == PID_REF || proType == PID_CORZ)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
		}
		break;
	case FILE_WF_VW_1: // lcd
		{
			if (proType == PID_VEL)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
			else if (proType == PID_WID)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + m_Header.obserSec.batch.usDopBinNumber[cut];
		}
		break;
	case FILE_WF_ZVW_2: // hcd
	case FILE_WF_ZVW_4: // batch
	case FILE_WF_ZVW_5:
	case FILE_WF_ZVW_6:
		{
			if (proType == PID_REF || proType == PID_CORZ)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
			else if (proType == PID_VEL)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
			else if (proType == PID_WID)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut];
		}
		break;
	case FILE_WF_ZVWH_7:
		{
			if (proType == PID_REF || proType == PID_CORZ)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
			else if (proType == PID_VEL)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
			else if (proType == PID_WID)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut];
			else if (proType == PID_HCL)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
		}
		break;
	case FILE_WF_ZVWHZKRP_8:
		{
			if (proType == PID_REF || proType == PID_CORZ)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
			else if (proType == PID_VEL)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
			else if (proType == PID_WID)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut];
			else if (proType == PID_HCL)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (proType == PID_PZDR)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 2 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (proType == PID_PKDP)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 3 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (proType == PID_PRHV)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 4 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (proType == PID_PPDP)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 5 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
		}
		break;
	case FILE_WF_ZVWC_9:
		{
			if (proType == PID_REF)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
			else if (proType == PID_VEL)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
			else if (proType == PID_WID)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut];
			else if (proType == PID_CORZ)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
		}
		break;
	case FILE_WF_ZVWHZKRPC_10:
		{
			if (proType == PID_REF)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD);
			else if (proType == PID_VEL)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) + m_Header.obserSec.usRefBinNumber[cut];
			else if (proType == PID_WID)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut];
			else if (proType == PID_HCL)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (proType == PID_PZDR)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 2 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (proType == PID_PKDP)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 3 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (proType == PID_PRHV)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 4 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (proType == PID_PPDP)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 5 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2;
			else if (proType == PID_CORZ)
				return pData += offset + sizeOfRadail * radNum + sizeof(BASE_DATA_HEAD) +
				m_Header.obserSec.usRefBinNumber[cut] * 5 +
				m_Header.obserSec.batch.usDopBinNumber[cut] * 2 +
				m_Header.obserSec.usRefBinNumber[cut] * 2;//差分相位占两个字节
		}
		break;
	}

	return NULL;
}

//************************************
// Method:    GetRadialHeader
// FullName:  VOLPro::GetRadialHeader
// Access:    public 
// Returns:   BASE_DATA_HEAD *
// Qualifier: 获取指定仰角层、指定径向的径向头
// Parameter: int cutIndex
// Parameter: int radNum
//************************************
BASE_DATA_HEAD *VOLPro::GetRadialHeader(int cutIndex, int radNum)
{
	int cut = cutIndex;
	unsigned char *pData = m_pData;
	unsigned long offset = GetCutPos( cut) - sizeof(tagRealFile);
	unsigned long sizeOfRadail = GetSizeOfRadial( cut );
	return (BASE_DATA_HEAD *)(pData + offset + sizeOfRadail * radNum);
}

//************************************
// Method:    GetMaxD
// FullName:  VOLPro::GetMaxD
// Access:    public 
// Returns:   short int
// Qualifier: 获取指定仰角、指定数据的最大测距(km)
// Parameter: unsigned int cut
// Parameter: int proType
//************************************
short int VOLPro::GetMaxD(unsigned int cut, int proType)
{
	unsigned short wavForm = m_Header.obserSec.batch.wavForm[cut];
	switch (wavForm)
	{
	case FILE_WF_VW_1: // lcd
		if ((proType == PID_VEL) || (proType == PID_WID))
			return (short int)(m_Header.obserSec.batch.usDopBinNumber[cut] * m_Header.obserSec.iDopBinLen[cut] / 1000.0);
		break;
	}
	if( (proType == PID_VEL) || (proType == PID_WID) )
		return (short int)(m_Header.obserSec.batch.usDopBinNumber[cut] * m_Header.obserSec.iDopBinLen[cut] / 1000.0);
	else
		return (short int)(m_Header.obserSec.usRefBinNumber[cut] * m_Header.obserSec.iRefBinLen[cut] / 1000.0);
}

//************************************
// Method:    GetBinWidth
// FullName:  VOLPro::GetBinWidth
// Access:    public 
// Returns:   unsigned short int
// Qualifier: 获取库长，以米为单位
// Parameter: unsigned int cut
// Parameter: unsigned int iProType
//************************************
unsigned short int VOLPro::GetBinWidth(unsigned int cut, unsigned int iProType)
{
	if ( iProType == PID_VEL || iProType == PID_WID )
		return m_Header.obserSec.iDopBinLen[cut];
	else
		return m_Header.obserSec.iRefBinLen[cut];

}