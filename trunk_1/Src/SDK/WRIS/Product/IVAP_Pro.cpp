
#include <WRIS/product/IVAP_pro.h>
#include <WRIS/product/productid.h>

IVAPPro::IVAPPro(void)
{

}

IVAPPro::~IVAPPro(void)
{

}

int IVAPPro::OpenBuff( char *pBuff,int iLen )
{
	int iRet = RVWPro::OpenBuff( pBuff, iLen, true );

	if( iRet == 0 )
	{
		memcpy( &m_IVAPHeader, m_pData, sizeof( tagIVAPHead ) );
		m_pData += sizeof( tagIVAPHead );
		m_IVAPData = m_pData;

// 		int iRadNum = m_IVAPHeader.m_RadNum;
// 		int iVbNum = m_IVAPHeader.m_VbNum;
		int iRangeNum = m_IVAPHeader.RangeNum;
		int AzNum = (int)(360/m_IVAPHeader.AzSpace);
		//int iVbNum = get
// 		m_pVData = new unsigned char [iRadNum*iVbNum];
//  		memcpy(m_pVData,m_IVAPData,iRadNum*iVbNum);
		m_pIVAP = new float[2*iRangeNum*AzNum];
		memcpy(m_pIVAP,m_IVAPData,sizeof(float)*2*iRangeNum*AzNum);

		//"20100914_172743.00.008.004_1000"
		char cName[256] = {0};
		tagRealObserSec &ob = m_Header.obserSec;

		sprintf( cName,
			"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%d-%d",
			ob.iObsStartTimeYear,
			ob.iObsStartTimeMonth,
			ob.iObsStartTimeDay,
			ob.iObsStartTimeHour,
			ob.iObsStartTimeMinute,
			ob.iObsStartTimeSecond,
			0,
			ob.iProductNumber,
			ob.batch.scanmode,
			m_IVAPHeader.cut,
			m_IVAPHeader.AzSpace
			);

		m_szPath = string(cName);
	}

	return iRet;
}
