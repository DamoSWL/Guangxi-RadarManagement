/*! \file productnamephrase.h 
	\brief 本文件定义了FL产品命名解析类

	\ingroup GRSGroup
	
	\version 1.0
	\author lava_sdb
	\date 2006/07/07

	\b modify log:
	\arg \c version 1.00,lava_sdb,2006/06/06 实现基本功能
	\arg \c version 1.01,lava_sdb,2006/07/07 实现对新命名规则的支持。 

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GRSInterface
*/
#ifndef _PRODUCTNAME_PHRASE
#define _PRODUCTNAME_PHRASE

#pragma once

#include "stdio.h"
#include "time.h"
#include <string>
using namespace std;


//static const char szExample[] = "20050806_013706.00.003.001.1";
#define PRODUCTINDEXFILENAME (wxT("\\ProductIndex.idx"))
/*! \class CProductName
	产品名称解析类
	
	本类用于解析产品文件名称
*/

//!< 降雨标识的枚举类型
enum PrecipEstFlag
{
	PrecipEst_NoSave=-1,	//没有降雨标志
	PrecipEst_Shine=0,		//没有气象
	PrecipEst_PartRain=1,	//局地小雨
	PrecipEst_BigRain=2,	//局地大雨
	PrecipEst_LargeRain=3,	//大面积降雨
	PrecipEst_Hail=4,		//雷雨或冰雹
	PrecipEst_Thunder=5,	//雷雨（暂时无法识别，保留）
	PrecipEst_Typhoon=6,	//台风（暂时无法识别，保留）
	PrecipEst_Tornado=7,	//龙卷（暂时无法识别，保留）
	PrecipEst_NULL=9		//无效数据
};

class CProductName
{
public:
	// 20050806_013706.00.003.001.1.bz2
	CProductName() 
	{
		InitData();
	};

	CProductName( string szName)
	{
		InitData();
		if( szName == "" )  return;
		m_szRadarNum = "NULL";
		string::size_type iSp = szName.rfind('/');
		while (iSp != string::npos)
		{
			szName = szName.replace(szName.begin()+iSp,szName.begin()+iSp+1,"\\");
			iSp = szName.rfind('/');
		}
		string::size_type iS = szName.rfind('\\');
		if( iS != string::npos )
		{
			m_szPath = string(szName.begin(), szName.begin()+iS +1);
			szName = string(szName.begin() + iS + 1, szName.end() );
		}

		string szZip = string(szName.end()-3, szName.end());
		if( szZip == "zdb" || szZip == "ZDB" || szZip == "zip" || szZip == "ZIP" || szZip == "BZ2" || szZip == "bz2")
		{
			m_bZip = true;
			if ( szZip == "zip" || szZip == "ZIP")
			{
				m_izipfom = 1;
			}
			else if ( szZip == "BZ2" || szZip == "bz2")
			{
				m_izipfom = 0;
			}
			else if ( szZip == "zdb" || szZip == "ZDB")
			{
				m_izipfom = -1;
			}
			else
			{
				m_izipfom = -2;
			}

			szName = string( szName.begin(), szName.end() - 4 );//去掉后缀名
		}
		else 
		{ 
			m_bZip = false;
		}

		m_szName = szName;
		m_time.tm_year = atoi(string(m_szName.begin(), m_szName.begin()+4).c_str()) - 1900;
		m_time.tm_mon = atoi(string(m_szName.begin()+4, m_szName.begin()+6).c_str()) - 1;
		m_time.tm_mday = atoi(string(m_szName.begin()+6, m_szName.begin()+8).c_str());
		m_time.tm_hour = atoi(string(m_szName.begin()+9, m_szName.begin()+11).c_str());
		m_time.tm_min = atoi(string(m_szName.begin()+11, m_szName.begin()+13).c_str());
		m_time.tm_sec = atoi(string(m_szName.begin()+13, m_szName.begin()+15).c_str());
	
		m_iLayer = atoi(string(m_szName.begin()+16, m_szName.begin()+18).c_str());
		m_iIndex = atoi(string(m_szName.begin()+19, m_szName.begin()+22).c_str());
		m_iScanMode = atoi(string(m_szName.begin()+23, m_szName.begin()+26).c_str());

		//szName = string( szName.begin(), szName.end() - 2 );
		//m_szName = szName;

		if( m_szName.size() >= 26 )
		{
			
// 			if (  m_szName.size() >= 27 )
// 			{
// 				m_szParame = string(m_szName.begin()+27, m_szName.end());
// 				
// 			}else
			{
				m_szParame = string(m_szName.begin()+26, m_szName.end());
			}
			//查找是否有降雨标识
			string::size_type iSRT = m_szParame.rfind("_R");
			if (iSRT != string::npos && m_szParame.size()>=3)
			{
				m_iRainType = PrecipEst_NULL;
				//找到降雨标识
				string strRaintype(m_szParame.begin()+ iSRT+2, m_szParame.begin()+iSRT+3 );
				m_iRainType = atoi(strRaintype.c_str());
				if ( iSRT > 0 )
				{
					//标识"_R",需要去除,去除"_"
					m_szParame = string(m_szParame.begin()+1, m_szParame.begin()+iSRT );
				}
				else
				{
					//无参数只有 R0
					m_szParame = "";
				}
			}
			else if ( m_szParame.size() >= 2 && m_szParame[0] == '_' )  //没有标识R的名称需要将"_"去除
			{
				//去除"_"
				m_szParame = string(m_szParame.begin()+1,m_szParame.end() );
			}
			char cchar[256] = { "" };
			sprintf( cchar , "%d" , m_iIndex);
			if ( m_szParame.empty() )
			{
				m_szMCode = cchar;
			}
			else
			{
				m_szMCode = cchar;
				m_szMCode = m_szMCode + "-" + m_szParame;
			}
		}
		else
		{
			m_szParame = "";
			m_szMCode = "";
		}
			
		//m_iLevel = atoi(string(m_szName.begin()+27, m_szName.begin()+28).c_str());
		//雷达编号
		string::size_type iSpos = m_szPath.rfind('\\');
		//查找'\'个数
		int iCut = 0;
		string PathTmp = m_szPath;
		while(iSpos != string::npos)
		{
			PathTmp = string(PathTmp.begin(), PathTmp.begin()+iSpos);
			iSpos = PathTmp.rfind('\\');
			iCut++;
		}
		if( iCut >=4 )
		{
			m_szRadarNum = m_szPath;
			m_szRadarNum = string(m_szRadarNum.begin(), m_szRadarNum.begin()+m_szRadarNum.rfind('\\'));
			m_szRadarNum = string(m_szRadarNum.begin(), m_szRadarNum.begin()+m_szRadarNum.rfind('\\'));
			//判断是否是日期
			string strdata = string(m_szRadarNum.begin()+m_szRadarNum.rfind('\\')+1,m_szRadarNum.end());
			if (atoi(strdata.c_str()) != 0 && strdata.length() == 8)
			{
				m_szRadarNum = string(m_szRadarNum.begin(), m_szRadarNum.begin()+m_szRadarNum.rfind('\\'));
				m_szRadarNum = string(m_szRadarNum.begin() + m_szRadarNum.rfind('\\') + 1, m_szRadarNum.end() );
			}
			else
			{
				m_szRadarNum = "NULL";
			}
		}
	}

	int GetZipFoam()
	{
		return m_izipfom;
	}
	string &SetData(int year, int month, int day, 
		int hour, int min, int sec, 
		int layer, int index, int mode, int lev)
	{
		m_time.tm_year = year - 1900;
		m_time.tm_mon = month - 1;
		m_time.tm_mday = day;
		m_time.tm_hour = hour;
		m_time.tm_min = min;
		m_time.tm_sec = sec;

		m_iLayer = layer;
		m_iIndex = index;
		m_iScanMode = mode;

		BuildName();
		return m_szName;
	}


	inline int GetYear()  { return m_time.tm_year + 1900; };
	string &SetYear(int year, bool bRe = true) 
	{
		m_time.tm_year = year - 1900; 
		if( bRe)
			BuildName();

		return m_szName; 
	}

	inline int GetMonth() { return m_time.tm_mon+1; };
	string &SetMonth(int month, bool bRe = true)
	{
		m_time.tm_mon= month-1;
		if(bRe)
			BuildName();
		return m_szName;
	}

	inline int GetDay()   { return m_time.tm_mday; };
	string &SetDay(int day, bool bRe = true)
	{
		m_time.tm_mday = day;
		if(bRe)
			BuildName();
		return m_szName;
	}

	inline int GetHour()  { return m_time.tm_hour;};
	string &SetHour(int hour, bool bRe = true)
	{
		m_time.tm_hour = hour;
		if(bRe)
			BuildName();
		return m_szName;
	}

	inline int GetMin()   { return m_time.tm_min; };
	string &SetMin(int min, bool bRe = true)
	{
		m_time.tm_min = min;
		if(bRe)
			BuildName();
		return m_szName;
	}

	inline int GetSec()   { return m_time.tm_sec; };
	string &SetSec(int sec, bool bRe = true)
	{
		m_time.tm_sec = sec;
		if(bRe)
			BuildName();
		return m_szName;
	}

	//! 获取产品的层号
	inline int GetLayer() { return m_iLayer; };
	string &SetLayer(int layer, bool bRe = true)
	{
		m_iLayer = layer;
		if(bRe)
			BuildName();
		return m_szName;
	}

	//! 获取产品的类型编号
	inline int GetIndex() { return m_iIndex; };
	string &SetIndex(int index, bool bRe = true)
	{
		m_iIndex = index;
		if(bRe)
			BuildName();
		return m_szName;
	}

	//! 获取扫描模式
	inline int GetScanMode() { return m_iScanMode; };
	string &SetScanMode(int mode, bool bRe = true)
	{
		m_iScanMode = mode;
		if(bRe)
			BuildName();
		return m_szName;
	}

	//! 获取区别码的参数区域
	inline const char *GetPareme()
	{ 
		return m_szParame.c_str() ;
	}

	//! 获取完整区别码
	inline const char *GetMCode()
	{ 
		return m_szMCode.c_str() ;
	}

	//! 获取产品生成时间，已 time_t 样式返回
	time_t GetTime_T() 
	{
		return mktime( &m_time);
	};

	//! 获取产品生成时间，已 tm 样式返回
	tm &GetTime() 
	{ 
		return m_time; 
	};
	//! 获取降雨标识
	int GetRainType()
	{
		return m_iRainType;
	}
	//! 获取产品名称，不包括后缀名
	inline const char *GetName() 
	{ 
		return m_szName.c_str(); 
	};
	
	//! 获取产品文件路径，不包括文件名
	inline const char *GetPath() 
	{ 
		return m_szPath.c_str(); 
	};
	//! 获取产品文件站点编号
	inline const char *GetRadarNum() 
	{ 
		return m_szRadarNum.c_str(); 
	};
	inline bool IsZip() { return m_bZip; };

	void BuildName()
	{
		char *name = new char[256];
		memset(name, 0, 256);
		
		if( m_szParame != "" )
		{
			sprintf(name, "%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d.%s", 
				m_time.tm_year + 1900,
				m_time.tm_mon + 1,
				m_time.tm_mday,
				m_time.tm_hour,
				m_time.tm_min,
				m_time.tm_sec,
				m_iLayer,
				m_iIndex,
				m_iScanMode,
				m_szParame.c_str()
				);
		}else{
			sprintf(name, "%04d%02d%02d_%02d%02d%02d.%02d.%03d.%03d", 
				m_time.tm_year + 1900,
				m_time.tm_mon + 1,
				m_time.tm_mday,
				m_time.tm_hour,
				m_time.tm_min,
				m_time.tm_sec,
				m_iLayer,
				m_iIndex,
				m_iScanMode
				);
		}

		m_szName = name;
		delete []name;
	}


private:
	string m_szName;	// 名称
	string m_szPath;	// 路径
	string m_szRadarNum;// 雷达编号
	string m_szParame;	// 参数
	string m_szMCode;	// 区别码
	int	   m_iRainType;// 降雨标识
	tm m_time;

	bool m_bZip;
	int m_izipfom;//压缩格式
	int m_iLayer;  // 产品在体扫中的Cut序号
	int m_iIndex;  // 产品编号
	int m_iScanMode; // 产品的扫描模式

protected:
	inline void InitData()
	{
		memset(&m_time, 0, sizeof(tm));
		m_bZip       = 0;
		m_iLayer     = 0;
		m_iIndex     = 0;
		m_iScanMode  = 0;
		m_szRadarNum = "NULL";
		m_iRainType  = PrecipEst_NoSave;
		m_izipfom    = -2;
	}
};


/*! \class CMCodePhrase
	区别码解析类
*/
class CMCodePhrase
{
	int m_iProductNumber;	//!< 产品类型编号
	string m_szParame;	//!< 产品计算的参数
	string m_szMCode;	//!< 区别码
public:
	
	CMCodePhrase( const char *cMCode )
	{
		/// 初始化变量
		m_iProductNumber = -1;

		///
		m_szMCode = cMCode ;
		basic_string <char>::size_type pos = m_szMCode.find( "-" );

		if( pos == string::npos )
		{
			m_iProductNumber = atoi( m_szMCode.c_str() ); 
		}
		else
		{
			string szProNum( m_szMCode.begin(), m_szMCode.begin() + pos );
			m_iProductNumber = atoi( szProNum.c_str() ); 

			m_szParame = string( m_szMCode.begin()+pos, m_szMCode.end() );
			if( ( !m_szParame.empty() ) && 
				( m_szParame[0] == '-') )
				m_szParame.erase( m_szParame.begin() );
		}
	}

	//! 获取产品类型编号
	int GetProductNumber()
	{
		return m_iProductNumber;
	}

	//! 获取产品参数
	const char * GetParame()
	{
		return m_szParame.c_str();
	}

	//! 获取区别码
	const char *GetMCode()
	{
		return m_szMCode.c_str();
	}

};


#endif