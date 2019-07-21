/************************************************************************
CLRAFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __ETC_FILE_080612 
#define __ETC_FILE_080612 

#include "rvw_pro.h"
#define Grade 10
#pragma pack(push,4)

typedef struct _ETC_head //
{
	float iContourValue[Grade]; //等值线值；
	unsigned short int  iContourGradeNum; //等值线的的等级数 10个等级
	unsigned short int iOneCoutourNum[Grade];//每个等值线值对应的该等值线的条数
}tagContourHead;


struct CGeoPoint
{
	float x;
	float y;

	CGeoPoint(){x=y=0;}
	CGeoPoint(float _x,float _y):x(_x),y(_y){}
	BOOL operator==(const CGeoPoint& rhs){return ( x==rhs.x && y==rhs.y );}
};

#pragma pack(pop)

class ETCPro:
	public RVWPro
{
public:
	int m_LineNum;						//所有线的总和
	int *m_pDataIndex;					//记录数据的索引
	short int *m_pPointNumofLine;		//记录每条线上的点数
	float *m_pLineHeight;					//记录每条线的高度值
	tagContourHead m_EtcHeader;

	ETCPro()
	{
		m_pDataIndex = NULL;
		m_pPointNumofLine = NULL;
		m_pLineHeight = NULL;
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
			memcpy( &m_EtcHeader, m_pData, sizeof( tagContourHead ) );
			m_pData += sizeof( tagContourHead );

			//"20100914_172743.00.008.004_1000"
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

			m_szPath = cName;
			//
			m_LineNum = 0;
			for (int i=0;i<Grade;i++)
			{
				m_LineNum += m_EtcHeader.iOneCoutourNum[i];
			}
			if (m_LineNum <= 0)
			{
				return iRet;
			}
			m_pDataIndex = new int[m_LineNum];
			memset(m_pDataIndex,0,sizeof(int)*m_LineNum);
			m_pLineHeight = new float[m_LineNum];
			memset(m_pLineHeight,0,sizeof(float)*m_LineNum);

			int Index = 0;
			for (int i=0;i<Grade;i++)
			{
				for(int j=Index;j<Index+m_EtcHeader.iOneCoutourNum[i];j++)
				{
					m_pLineHeight[j] = m_EtcHeader.iContourValue[i];
				}
				Index += m_EtcHeader.iOneCoutourNum[i];
			}
			m_pPointNumofLine = new short int[m_LineNum];
			//
			memcpy( m_pPointNumofLine, m_pData,sizeof(short int)*m_LineNum);
			m_pData += sizeof(short int)*m_LineNum;
			int Block = sizeof(CGeoPoint);
			for (int i=1;i<m_LineNum;i++)
			{
				m_pDataIndex[i] = (Block*m_pPointNumofLine[i-1])+m_pDataIndex[i-1];
			}
		}

		return iRet;
	}
	~ETCPro()
	{
		delete m_pDataIndex;
		m_pDataIndex = NULL;

		delete m_pPointNumofLine;
		m_pPointNumofLine = NULL;

		delete m_pLineHeight;
		m_pLineHeight = NULL;
	}
};


#endif


