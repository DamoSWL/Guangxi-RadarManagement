
#include "stdio.h"

#include "../../../include/WRIS/product/rvw_pro.h"
#include "../../../include/WRIS/product/rfc_pro.h"

RFCPro::RFCPro()
{
	memset( &m_Header, 0, sizeof(tagRealFile) );
	memset( &m_rfcH, 0, sizeof(RFCTBH_H) );
	memset( &m_pData, 0, sizeof(RFCTB) );
	m_pAzi = NULL;
	m_pData = NULL;
}

RFCPro::~RFCPro()
{
	if( m_pAzi != NULL ) 
	{
		delete []m_pAzi;
		m_pAzi = NULL;
	}

	if( m_pData != NULL )
	{
		delete []m_pData;
		m_pData = NULL;
	}

}


int RFCPro::Open(string szPath)
{
	char *pData = NULL;
	int iRet = 0;

	FILE *fp = fopen( szPath.c_str(), "rb" );
	if( fp ) {
		fseek( fp, 0, SEEK_END );
		int size = ftell( fp );
		fseek( fp, 0, SEEK_SET );

		pData = new char[ size ];

		fread( pData, size, 1, fp );
		fclose( fp );

		iRet = OpenBuff( pData, size );
	}

	if( pData ) delete []pData;

	return iRet;
}

int RFCPro::OpenBuff( char *pBuff,int iLen)
{
	char *pB = pBuff;
	memcpy( &m_Header, pB,  sizeof(tagRealFile) );  
	pB += sizeof(tagRealFile);
	
	memcpy( &m_rfcH, pB, sizeof(RFCTBH_H) );
	pB += sizeof(RFCTBH_H);

	/// 获取方位
	if( m_pAzi != NULL )  delete []m_pAzi;
	int sizeAzi = sizeof( RFCTBH_RAD_AZ )*m_rfcH.RecodeNumber;
	m_pAzi = new RFCTBH_RAD_AZ[ m_rfcH.RecodeNumber ];
	memcpy( m_pAzi, pB, sizeAzi );

	pB += (sizeof( RFCTBH_RAD_AZ )*m_rfcH.RecodeNumber);

	/// 获取数据
	int dataLen = iLen -  sizeof(tagRealFile) - sizeof(RFCTBH_H) - sizeAzi;
	if( m_pData != NULL ) delete []m_pData;
	m_pData = new char[ dataLen ];
	
	memcpy( m_pData, pB, dataLen );

	int ETSize = m_rfcH.RangeBinNum * m_rfcH.RecodeNumber ;
	int MSize =  m_rfcH.RecodeNumber;
	int RSize = m_rfcH.HNum * m_rfcH.VNum * m_rfcH.RecodeNumber;
	m_RFC.pET = (unsigned short int *)m_pData;
	m_RFC.pEB = (unsigned short int *)(m_RFC.pET + ETSize); 
	m_RFC.pThick = (unsigned short int *)( m_RFC.pEB + ETSize );
	m_RFC.pM = (unsigned short int *)( m_RFC.pThick + MSize );
	m_RFC.pRef = (unsigned short int *)( m_RFC.pM + MSize );
	m_RFC.pPFac = (short int *)( m_RFC.pRef + RSize);

	return 0;
}

void RFCPro::Close()
{

}


bool RFCPro::HasData() 
{ 
	return (m_pData)?true:false; 
}
