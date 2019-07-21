
#include "../../../include/WRIS/product/sti_pro.h"
#include "../../../include/WRIS/product/productid.h"
char g_cIdx[5]={0};

STIPro::STIPro(void)
{
	m_bInitGetData = true;
	m_pSerialData = NULL;

	g_cIdx1 = 'A';
	g_cIdx0 = 0;
}

STIPro::~STIPro(void)
{
	if( m_pSerialData )
	{
		delete m_pSerialData;
		m_pSerialData = NULL;
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
int STIPro::Open(string szPath )
{
	char *pData = NULL;

	FILE *fp = fopen( szPath.c_str(), "rb" );
	if( fp )
	{
		fseek( fp, 0, SEEK_END );
		long size = ftell( fp );
		fseek( fp, 0, SEEK_SET );

		pData = new char[ size ];
		fread( pData, 1, size, fp);

		fclose( fp );

		PhraseVol( (unsigned char*)pData, size );
	}


	if( pData ) delete []pData;

	return 0;
}

int STIPro::OpenBuff( char *pBuff,int iLen )
{
//	ALLSTI *pNew = (ALLSTI *)pBuff;
	char *pNew = NULL;
	unsigned int VerCur = ((tagALLSTI *)pBuff)->Ver;
	unsigned int DataCountCur = ((tagALLSTI *)pBuff)->DataCount;
	
	if (VerCur == 1)
	{
		//第1版本
		if (!m_vecStorms.empty())
		{
			g_cIdx1='A';
			g_cIdx0=0;
			m_vecStorms.clear();
		}
		unsigned int iDatains = sizeof(tagALLSTI);
 		pNew = pBuff+iDatains;

		iDatains = 0;
		for (int i=0;i<DataCountCur;i++)
		{
			RVWPro::OpenBuff( pNew+iDatains, ((tagALLSTI *)pBuff)->DataSize[i] );
			PhraseVol( (unsigned char*)pNew+iDatains, ((tagALLSTI *)pBuff)->DataSize[i] );
			iDatains += ((tagALLSTI *)pBuff)->DataSize[i];
			
		}		
	}
	else//无版本和第2版本
	{
		VerCur = ((tagALLSTI *)(pBuff+sizeof(tagRealFile)))->Ver;
		DataCountCur = ((tagALLSTI *)(pBuff+sizeof(tagRealFile)))->DataCount;
		tagRealFile * RealFile = (tagRealFile*)(pBuff+sizeof(tagRealFile) + sizeof(tagALLSTI));
		if (VerCur == 2 && DataCountCur>0 && DataCountCur < ALLSTI_DATASIZE && RealFile->obserSec.iProductNumber == PID_SCTI)
		{
			char* pBuffData = pBuff+sizeof(tagRealFile);
			//第2版本
			//第1版本
			if (!m_vecStorms.empty())
			{
				g_cIdx1='A';
				g_cIdx0=0;
				m_vecStorms.clear();
			}
			unsigned int iDatains = sizeof(tagALLSTI);
			pNew = pBuffData+iDatains;

			iDatains = 0;
			for (int i=0;i<DataCountCur;i++)
			{
				RVWPro::OpenBuff( pNew+iDatains, ((tagALLSTI *)pBuffData)->DataSize[i] );
				PhraseVol( (unsigned char*)pNew+iDatains, ((tagALLSTI *)pBuffData)->DataSize[i] );
				iDatains += ((tagALLSTI *)pBuffData)->DataSize[i];

			}		
		}
		else
		{
			//无版本
			RVWPro::OpenBuff( pBuff, iLen );
			PhraseVol( (unsigned char*)pBuff, iLen );
		}

	}

	char cName[256] = {0};
	tagRealObserSec &ob = m_Header.obserSec;
	
	memcpy( &m_StiHeader, m_pData, sizeof(tagStiHeader) );

	int mm = m_vecStorms.size();
	
	sprintf( cName,
		"%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d_%.2f",
		ob.iObsStartTimeYear,
		ob.iObsStartTimeMonth,
		ob.iObsStartTimeDay,
		ob.iObsStartTimeHour,
		ob.iObsStartTimeMinute,
		ob.iObsStartTimeSecond,
		0,
		ob.iProductNumber,
		ob.batch.scanmode,
		m_StiHeader.Area);

	m_szPath = string(cName);

	return 0;
}



//////////////////////////////////////////////////////////////////////////
//! 获取风暴体个数
unsigned int STIPro::GetVolCount( )
{
	return m_StiHeader.nVol;
}

tagStormTrakHeader *STIPro::GetVolTarkInfo( int iVolIndex )
{
	if( iVolIndex >=0 && iVolIndex<(m_StiHeader.nVol-1) )
	{
		unsigned long lPos = m_Header.obserSec.iPPIStartPos[iVolIndex] - sizeof(tagRealFile) - sizeof(tagStiHeader);

		return (tagStormTrakHeader*)(m_pDataT + lPos);
	}
	return NULL;
}


tagStormCellTrak *STIPro::GetTrakInfo( int iVolIndex, int iTrak )
{
	
	if( iVolIndex >=0 && iVolIndex<(m_StiHeader.nVol) )
	{
		
		unsigned long lPos = m_Header.obserSec.iPPIStartPos[iVolIndex] - sizeof(tagRealFile) - sizeof(tagStiHeader);
		unsigned char *pData = m_pDataT + lPos;

		//tagStormTrakHeader* pTH = GetVolTarkInfo(iVolIndex);
		
		//int cntTrck = pTH->nStormTrak;
		int cntTrck = m_StiHeader.nVol;
		if( iTrak >=0 && iTrak < cntTrck )
		{
			int i = 0;
			pData += sizeof(tagStormTrakHeader);
			for(i = 0; i<cntTrck; i++)
			{
				if( i == iTrak ) {
					return (tagStormCellTrak *)pData;
				}

				tagStormCellTrak *pT = (tagStormCellTrak *)(pData);
				pData += sizeof(tagStormCellTrak);
				pData += pT->cntFort * sizeof(tagStormPos);
			}
		}

	}

	return NULL;
}


tagStormPos *STIPro::GetFortInfo(int iVolIndex, int iTrak, int &cnt )
{	
	if( iVolIndex >=0 && iVolIndex<(m_StiHeader.nVol) )
	{

		unsigned long lPos = m_Header.obserSec.iPPIStartPos[iVolIndex] - sizeof(tagRealFile) - sizeof(tagStiHeader);
		unsigned char *pData = m_pDataT + lPos;

		//tagStormTrakHeader* pTH = GetVolTarkInfo( iVolIndex );

		//int cntTrck = pTH->nStormTrak;

		int cntTrck = m_StiHeader.nVol;
		if( iTrak >=0 && iTrak < cntTrck )
		{
			int i = 0;
			
			pData += sizeof(tagStormTrakHeader);
			for(i = 0; i<cntTrck; i++)
			{
				if( i == iTrak ) {
					tagStormCellTrak *pT = (tagStormCellTrak *)pData;
					cnt = pT->cntFort;
					
					return (tagStormPos *)(pData + sizeof(tagStormCellTrak));
				}

				tagStormCellTrak *pT = (tagStormCellTrak *)pData;
				pData += sizeof(tagStormCellTrak);
				pData += pT->cntFort * sizeof(tagStormPos);
			}
		}

	}
	return 0;
}



void STIPro::PhraseFile()
{
	unsigned int cntVol = GetVolCount();
	unsigned int i=0,j=0;

	m_cntVol = cntVol;

	for( i=0; i< cntVol-1; i++ )
	{
		///////////
		tagStormTrakHeader *pVolHead = GetVolTarkInfo( i );
		if (pVolHead == NULL)
		{
			continue;
		}
		unsigned int cntTrak = pVolHead->nStormTrak;

		if( i == 0 ) {
			m_timeStart = m_timeEnd = pVolHead->time;
		}
		else {
			m_timeEnd = pVolHead->time;
		}

		for( j=0; j<cntTrak; j++ )
		{
			tagStormCellTrak *pTrak = GetTrakInfo(i, j);
			if(pTrak != NULL)
			{
				if( pTrak->bNewStorm )
				{
					/// 是新生的风暴体
					////// 判断新生风暴体是否第一次匹配
					tagStormTrak storm;
					
					if( pTrak->IndPre != -1 )
					{
						tagStormCellTrakAFort1 trakPre;
						trakPre.iVol = i;
						trakPre.trak = *pTrak;

						trakPre.trak.IndPre = -1;
						trakPre.trak.IndCur = pTrak->IndPre;
						trakPre.trak.pos = pTrak->PosPre;

						storm.traks.push_back( trakPre );
					}
					/////////////////////////
					tagStormCellTrakAFort1 trak;
					trak.iVol = i;
					trak.trak = *pTrak;

					int cntFort = pTrak->cntFort;
					tagStormPos *pos = GetFortInfo(i,j, cntFort);
					if(pos != NULL)
					{
						for(int k=0; k<cntFort; k++ )
							trak.forts.push_back( *(pos+k) );
					}

					storm.traks.push_back( trak );

					/////////////////////////
					storm.timeStart = pVolHead->time;
					m_vecStorms.push_back( storm );
				} 
				else 
				{
				/// 不是新生的风暴体 
					int lastIndex = pTrak->IndPre;

					// 检测 vecStorm中 各个风暴体是否存在对应的风暴体
					   // 存在，在风暴体链后增加相应的风暴体信息
					   // 不存在，增加新的风暴体
					size_t ivecStom = 0;
					bool bIsInIt = false;
					for( ivecStom=0; ivecStom< m_vecStorms.size(); ivecStom++ )
					{
						int sizeS = m_vecStorms[ivecStom].traks.size();
						if( lastIndex == m_vecStorms[ivecStom].traks[sizeS-1].trak.IndCur &&
							(i - m_vecStorms[ivecStom].traks[sizeS-1].iVol) == 1)
						{
							// 插入到列表后
							tagStormCellTrakAFort1 trak;
							trak.iVol = i;
							trak.trak = *pTrak;

							int cntFort = pTrak->cntFort;
							tagStormPos *pos = GetFortInfo(i,j,cntFort);
							if(pos != NULL)
							{
								for(int k=0; k<cntFort; k++ )
								{
									trak.forts.push_back( *(pos+k) );
								}
							}
							m_vecStorms[ivecStom].timeEnd = pVolHead->time;
							m_vecStorms[ivecStom].traks.push_back( trak );
							
							bIsInIt = true;
							break;
						}
					}
					//
					if( !bIsInIt )
					{
						/// 是新生的风暴体
						tagStormTrak storm;

						if( pTrak->IndPre != -1 ) 
						{
							tagStormCellTrakAFort1 trakPre;
							trakPre.iVol = i;
							trakPre.trak = *pTrak;

							trakPre.trak.IndPre = -1;
							trakPre.trak.IndCur = pTrak->IndPre;
							trakPre.trak.pos = pTrak->PosPre;

							storm.traks.push_back( trakPre );
						}
						
						//////////////////////
						tagStormCellTrakAFort1 trak;
						trak.iVol = i;
						trak.trak = *pTrak;

						int cntFort = 0;
						tagStormPos *pos = GetFortInfo(i, j, cntFort);

						for(int k=0; k<cntFort; k++ )
							trak.forts.push_back( *(pos+k) );

						storm.traks.push_back( trak );

						//////////////////////////
						storm.timeStart = pVolHead->time;
						storm.bVisible = true;
						m_vecStorms.push_back( storm );
					}
				}
			}
		}
	}

	i = j;
}


unsigned int STIPro::GetStormCount()
{
	return m_vecStorms.size();
}

tagStormTrak *STIPro::GetStormTrak( unsigned int stormIndex)
{
	size_t size = m_vecStorms.size();
	if( stormIndex>=0 && stormIndex< size )
	{
		return &m_vecStorms[stormIndex];
	}
	return NULL;
}

bool STIPro::PhraseVol( unsigned char *pData, unsigned int iLen)
{
	unsigned int curPos = 0;
	/*if( iLen <= sizeof(tagRealFile) + sizeof(tagStormTrakHeader) )  return false;*/

	if( iLen <= sizeof(tagRealFile) + sizeof(tagStiHeader) )  return false;
	tagRealFile *pHeader = (tagRealFile *)pData;
	curPos += sizeof(tagRealFile);
	
	m_Header = *pHeader;

	//tagStormTrakHeader *pTrakHead = (tagStormTrakHeader *)(pData + curPos);
	//curPos += sizeof(tagStormTrakHeader);
	tagStiHeader *pStiHead = (tagStiHeader *)(pData + curPos);
	curPos += sizeof(tagStiHeader);

	if( m_bInitGetData ) {
		m_timeStart = pStiHead->tStart;
		m_timeEnd = pStiHead->tStop;
		m_cntVol = 1;
		m_bInitGetData = false;
	}else {
		m_timeEnd = pStiHead->tStop;
		m_cntVol++;
	}

	//int nTrak = pTrakHead->nStormTrak;
	int nTrak = pStiHead->nStrom;
	
	int i = 0,j=0; 

	for( i=0; i< nTrak; i++ )
	{
		if( iLen < curPos + sizeof(tagStormCellTrak) )  {
			return false;
		}		
		
		tagStormCellTrak *pTrak = ( tagStormCellTrak *)(pData + curPos);
		curPos += sizeof(tagStormCellTrak);
		
		m_iPosVol = m_iVol = pTrak->iVol;

		if( pTrak->IndPre == -1 )
		{
			/// 是新生的风暴体
			////// 判断新生风暴体是否第一次匹配
			tagStormTrak storm;
			tagStormCellTrakAFort1 trak;
			trak.trak = *pTrak;
			
			int cntFort = pTrak->cntFort;

			if( iLen < (curPos + cntFort * sizeof(tagStormPos) )) {
				return false;
			}

			for( j=0; j<cntFort; j++)
			{
				tagStormPos *pPos = (tagStormPos *)(pData + curPos);
				curPos += sizeof(tagStormPos);
				trak.forts.push_back( *pPos );
			}

			storm.traks.push_back( trak );

			/////////////////////////
			//storm.timeStart = pTrakHead->time;
			//storm.timeEnd = pTrakHead->time;

			storm.timeStart = pStiHead->tStart;
			storm.timeEnd = pStiHead->tStop;
			storm.nStormTrak = 1;
			storm.bVisible = true;
			
			sprintf_s(g_cIdx,"%c%d",g_cIdx1,g_cIdx0);
			//storm.idx = g_cIdx;
			//g_cIdx = storm.idx;
			memcpy(storm.idx,g_cIdx,5);
			m_vecStorms.push_back( storm );
			g_cIdx1++;
			if( g_cIdx1 > 'Z' ) 
			{
				g_cIdx1 = 'A';
				g_cIdx0++;
			}

		} else {
			/// 不是新生的风暴体 
			int lastIndex = pTrak->IndPre;

			// 检测 vecStorm中 各个风暴体是否存在对应的风暴体
			// 存在，在风暴体链后增加相应的风暴体信息
			// 不存在，增加新的风暴体
			size_t ivecStom = 0;
			bool bIsInIt = false;
			for( ivecStom=0; ivecStom< m_vecStorms.size(); ivecStom++ )
			{
				int sizeS = m_vecStorms[ivecStom].traks.size();
				int preIndex = m_vecStorms[ivecStom].traks[sizeS-1].trak.IndCur;
				int preIVol = m_vecStorms[ivecStom].traks[sizeS-1].trak.iVol;

				if( lastIndex == preIndex &&
					( pTrak->iVol - preIVol ) == 1)
				{
					// 插入到列表后
					m_vecStorms[ivecStom].nStormTrak++;
					tagStormCellTrakAFort1 trak;
					trak.iVol = i;
					trak.trak = *pTrak;

					int cntFort = pTrak->cntFort;
					if( iLen < (curPos + cntFort * sizeof(tagStormPos) )) {
						return false;
					}

					for( j=0; j<cntFort; j++)
					{
						tagStormPos *pPos = (tagStormPos *)(pData + curPos);
						curPos += sizeof(tagStormPos);
						trak.forts.push_back( *pPos);
					}
					
					//m_vecStorms[ivecStom].timeEnd = pTrakHead->time;
					m_vecStorms[ivecStom].timeEnd = pStiHead->tStop;
					m_vecStorms[ivecStom].traks.push_back( trak );

					bIsInIt = true;
					break;
				}
			}
			//
			if( !bIsInIt )
			{
				/// 是新生的风暴体
				tagStormTrak storm;
				tagStormCellTrakAFort1 trak;
				trak.trak = *pTrak;

				int cntFort = pTrak->cntFort;
				if( iLen < (curPos + cntFort * sizeof(tagStormPos) )) {
					return false;
				}

				for( j=0; j<cntFort; j++)
				{
					tagStormPos *pPos = (tagStormPos *)(pData + curPos);
					curPos += sizeof(tagStormPos);
					trak.forts.push_back( *pPos);
				}

				storm.traks.push_back( trak );

				//////////////////////////
				//storm.timeStart = pTrakHead->time;
				//storm.timeEnd = pTrakHead->time;

				storm.timeStart = pStiHead->tStart;
				storm.timeEnd = pStiHead->tStop;
			
				storm.bVisible = true;
				sprintf_s(g_cIdx,"%c%d",g_cIdx1,g_cIdx0);
// 				storm.idx = g_cIdx;
// 				g_cIdx = storm.idx;
				memcpy(storm.idx,g_cIdx,5);
				m_vecStorms.push_back( storm );
				g_cIdx1++;
				if( g_cIdx1 > 'Z' ) 
				{
					g_cIdx1 = 'A';
					g_cIdx0++;
				}
			}
		}
	}

	
	/// 清除掉已经消失了的天气过程
	/* 当风暴体追踪单元的最后时间比当前时间早，则清除此风暴体
	
	*/
	vector< tagStormTrak >::iterator it,ite;
	it = m_vecStorms.begin();
	ite = m_vecStorms.end();

	while( it != ite )
	{
		tm tmEnd = it->timeEnd;
		if( memcmp( &tmEnd, &m_timeEnd, sizeof(tm)) != 0 )
		{
			m_vecStorms.erase( it );
			it = m_vecStorms.begin();
			ite = m_vecStorms.end();
			continue;
		}

		/////
		it++;
	}

	return true;
}

int STIPro::SerialData( unsigned char **pData )
{
	if( m_vecStorms.size() == 0 ) return 0;

	if( m_pSerialData ) {
		delete m_pSerialData;
		m_pSerialData = NULL;
	}
	
	(*pData) = (unsigned char*)this;
	
	return int( m_vecStorms.size() );

	return -1;
}

//int STIPro::PhraseSerialData( unsigned char *pData )
//{
//	
//	this->pra
//STIPro *pIn = (STIPro *)pData;

//	this->m_bInitGetData = true;
//
//	this->m_Header = pIn->m_Header;
//	this->m_StiHeader = pIn->m_StiHeader;
//	this->m_timeStart = pIn->m_timeStart;
//	this->m_timeEnd = pIn->m_timeEnd;
//	this->m_cntVol = pIn->m_cntVol;
//	this->m_iVol = pIn->m_iVol;
//	this->m_iPosVol = pIn->m_iPosVol;
//	this->m_vecStorms = pIn->m_vecStorms;
//
//	return 0;
//}

int STIPro::PhraseSerialData( char *pData,int iLen)
{
	//RVWPro::OpenBuff( pData, iLen );
	STIPro *pIn = (STIPro *)pData;
	int curPos = 0;
	if( iLen <= sizeof(tagRealFile) + sizeof(tagStiHeader) )  return false;
	tagRealFile *pHeader = (tagRealFile *)pData;
	curPos += sizeof(tagRealFile);
	m_Header = *pHeader;
	return 0;
}

