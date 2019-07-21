/************************************************************************
CSingleFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __SOURCE_FILE_080199__ 
#define __SOURCE_FILE_080199__ 
//
#include "basefilehead_fl.h"
#include "radialhead.h"

#include <string>
using namespace std; 
using namespace CDYW::GRS::DataFormat;
using namespace CDYW::GRS::DataFormat::XFL;

class RHIPro
{
public:
	RHIPro(void);
	~RHIPro(void);

	string m_szPath;	//!< 文件路径

	tagRealFile m_Header;	//!< 文件头
	unsigned char *m_pData;
	
	unsigned int m_dataSize;
	unsigned char *m_pAData;

	int m_NumOfRadial;	//!< 每圈径向个数
	int m_kilo;			//!< 径向公里数
	int m_RangeBin;		//!< 每公里取点数

	int Open(string szPath);
	int OpenBuff( char *pBuff,int iLen);
	void Close();
//	
	inline float GetElevation()
	{
		return m_Header.obserSec.iElevation[0];
	}


	inline short int GetGuanCeYaoSu()
	{
		return m_Header.obserSec.cObsEssential[0];
	}

	//!得到某cut的速度推模糊的方式
	inline short int GetModeOfTMH()
	{
		return m_Header.obserSec.cVDeblurring[0];
	}		
	
	//! 某Cut的第一脉冲重复频率
	inline short int GetPrt1()
	{
		return m_Header.obserSec.iFirstKindPRF[0];
	}

	//! 某Cut的第二脉冲重复频率
	inline short int GetPrt2()
	{
		return m_Header.obserSec.iSecondKindPRF[0];
	}

	//! 某 Cut 的脉冲宽度
	inline short int GetWidOfMC()
	{
		return m_Header.obserSec.iPulseWidth[0];
	}
	//! 某 Cut 的 最大测速
	inline unsigned short int GetMaxV()
	{
		return m_Header.obserSec.iMaxObsVel[0];
	}
	//! 某 Cut 的 最大测距
	short int GetMaxD(int proType);

	//! 某 Cut 的 径向个数
	inline unsigned short int GetRadialCount()
	{
		return m_Header.obserSec.iRadialNum[0];
	}

	//! 某 Cut 的 起始Radial
	inline short int GetStartRadial()
	{
		return m_Header.obserSec.iFirstBinDis[0];
	}
	
	//! 某 Cut 的 数据要素
	inline short int GetDataYaoSu()
	{
		return m_Header.obserSec.cObsEssential[0];
	}

	//! 某 Cut 的 数据在文件的位置
	inline unsigned long GetCutPos()
	{
		return m_Header.obserSec.iPPIStartPos[0];
	}

	inline unsigned long GetSizeOfYaoSu()
	{
		return m_Header.obserSec.cObsEssential[0];
	}
	//! 某 cut 的 径向上的 radial 上的 距离库数

	inline float Deg2Float(short data)
	{
 		char *pData = (char *)&data;
 		return (float)(pData[0] *0.02197265625 + pData[1] * 2.8125);
	}
//	
//	/*! 获取指定径向的数据
//	*/
	unsigned char * GetRadialData( int cutIndex, int radNum, int proType);
//
//	/*! 获取指定径向的径向头
//	*/
	BASE_DATA_HEAD *GetRadialHeader(int cutIndex, int radNum);
//
//	//! 获取指定Cut 的指定类型产品，相对于 Radail数据（包含radial头的偏移）
	unsigned int GetProductRadialOffset(unsigned int type);
//	
//	//! 获取指定Cut的第一个产品的类型
	unsigned int GetProductTypeOfCut();
//
//	//! 获取某Cut的每Radial的长度
	unsigned int GetSizeOfRadial();
//
//	//! 获取某Cut的某类型产品的距离库数
	unsigned int GetRangeBinOfRad(unsigned int type);
//
//	//! 获取产品的距离库库长
	unsigned short int GetBinWidth(unsigned int type);
//
//	//! 获取产品文件名字
	const char *GetDataName() { return m_szPath.c_str();};
//
//	//! 是否有数据
	bool HasData() { return (m_pData)?true:false; };
};

#endif
