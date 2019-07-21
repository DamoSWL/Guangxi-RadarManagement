/************************************************************************
CLRAFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __SEI_FILE_20120924 
#define __SEI_FILE_20120924 

#include "rvw_pro.h"
#define SIZE 400
#define Psize 160000
#pragma pack(push,4)

//!暴雨回波识别产品文件头
typedef struct _SEIHEAD
{
	int iStormEcroNum;  //!<一个体扫内暴雨回波的个数
	float fZT;            //!<弱回波门限 单位为dBZ
	float fA;            //!计算降雨量的Z-R关系中的参数A
	float fb;            //!计算降雨量的Z-R关系中的参数b
	float WinofGrid;     //!网格大小 单位：m
}tagSEIHead;

#pragma pack(pop)

class SEIPro:
	public RVWPro
{
public:
	tagSEIHead m_SEIHeader;
	float *m_pfRZ;
	int *m_pClass;

	SEIPro()
	{
		m_pfRZ = NULL;
		m_pClass = NULL;
	}
	/*! 从 缓冲打开产品
		\param pBuff 缓冲指针
		\param iLen 缓冲数据长度
		\param bCopy 是否使用传入的数据
		             - true 在内存里复制传入数据的一个备份
					 - false 直接使用传入的数据
	*/
	int OpenBuff( char *pBuff,int iLen, bool bCopy = true )
	{
		int iRet = RVWPro::OpenBuff( pBuff, iLen, bCopy );

		if( iRet == 0 )
		{
			memcpy( &m_SEIHeader, m_pData, sizeof( tagSEIHead ) );
			m_pData += sizeof( tagSEIHead );

			//"20100914_172743.00.008.004_1000"
			char cName[256] = {0};
			tagRealObserSec &ob = m_Header.obserSec;

			sprintf( cName,
				"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d-%.2f_%.2f_%.2f",
				ob.iObsStartTimeYear,
				ob.iObsStartTimeMonth,
				ob.iObsStartTimeDay,
				ob.iObsStartTimeHour,
				ob.iObsStartTimeMinute,
				ob.iObsStartTimeSecond,
				0,
				ob.iProductNumber,
				ob.batch.scanmode,
				m_SEIHeader.fZT,
				m_SEIHeader.fA,
				m_SEIHeader.fb
				);
			m_szPath = cName;
			
			m_pfRZ = new float[m_SEIHeader.iStormEcroNum];
			memcpy(m_pfRZ,m_pData,sizeof(float)*m_SEIHeader.iStormEcroNum);
			m_pData +=sizeof(float)*m_SEIHeader.iStormEcroNum;
			if (m_SEIHeader.iStormEcroNum>0)
			{
				m_pClass = new int [Psize];
				memcpy(m_pClass,m_pData,sizeof(int)*Psize);
			}

		}

		return iRet;
	}
	~SEIPro()
	{
		if (m_pfRZ)
		{
			delete []m_pfRZ;
			m_pfRZ = NULL;
		}
		if (m_pClass)
		{
			delete []m_pClass;
			m_pClass = NULL;
		}
	}
};


#endif


