/************************************************************************
CSingleFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __SOURCE_FILE_080111 
#define __SOURCE_FILE_080111 

#include "basefilehead_fl.h"
#include "radialhead.h"

using namespace CDYW::GRS::DataFormat;
using namespace CDYW::GRS::DataFormat::XFL;

#include <string>
using namespace std;

//! 基数据及其类似数据解析类
/*!
 * 此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
   \arg 反射率基数据产品（R）
   \arg 速度基数据产品（V）
   \arg 谱宽基数据产品（W）
   \arg 回波顶高产品（ET）
   \arg 回波底高产品（BT）
   \arg 最大反射率产品（MAXREF）
  
 * \author lava_sdb
 * \ingroup group_UT
*/
class RVWPro
{
	bool m_bDataIsCopy;		//!< 数据是复制的（true）还是直接使用的（false）
public:
	RVWPro(void);
	~RVWPro(void);
	string m_szPath;	//!< 文件路径

	tagRealFile m_Header;	//!< 文件头
	unsigned char *m_pData;
	

	unsigned char *m_pAData;
	unsigned int m_uiDataLen; //!< 数据区的长度


	/*! 通过路径名代开产品
		\param szPath 产品路径
		\return 返回 0，正常；
	*/
	int Open(string szPath);
	
	/*! 从 缓冲打开产品

		\param pBuff 缓冲指针
		\param iLen 缓冲数据长度
		\param bCopy 是否使用传入的数据
		             - true 在内存里复制传入数据的一个备份
					 - false 直接使用传入的数据
	*/
	int OpenBuff( char *pBuff,int iLen, bool bCopy = true );

	void Close();

	bool HasData() { return (m_pData)?true:false; };


	//! 获取产品的类型
	inline unsigned long GetProductType()
	{
		return m_Header.obserSec.iProductNumber;
	}

	//! 获取产品的仰角
	inline float GetElevation()
	{
		return m_Header.obserSec.iElevation[0]/100.f;
	}

	inline short int GetGuanCeYaoSu()
	{
		return m_Header.obserSec.cObsEssential[0];
	}

	//! 获取产品速度推模糊的方式
	inline short int GetModeOfTMH()
	{
		return m_Header.obserSec.cVDeblurring[0];
	}		
	
	//! 获取产品的第一脉冲重复频率
	inline short int GetPrt1()
	{
		return m_Header.obserSec.iFirstKindPRF[0];
	}

	//! 获取产品第二脉冲重复频率
	inline short int GetPrt2()
	{
		return m_Header.obserSec.iSecondKindPRF[0];
	}

	//! 某 Cut 的脉冲宽度
	inline short int GetWidOfMC()
	{
		return m_Header.obserSec.iPulseWidth[0];
	}
	//! 获取产品的最大测速
	inline unsigned short int GetMaxV()
	{
		return m_Header.obserSec.iMaxObsVel[0];
	}

	//! 获取产品最大测距
	short int GetMaxD();


	//! 获取产品的径向个数
	inline unsigned short int GetRadialCount()
	{
		return m_Header.obserSec.iRadialNum[0];
	}

	//! 获取产品的距离库库长
	unsigned short int GetBinWidth();

	
	//! 获取产品数据要素
	inline short int GetDataYaoSu()
	{
		return m_Header.obserSec.cObsEssential[0];
	}

	//! 获取产品数据在文件的位置
	inline unsigned long GetCutPos()
	{
		return m_Header.obserSec.iPPIStartPos[0];
	}

	//! 获取产品的径向长度（包含径向头）
	unsigned int GetSizeOfRadial( );

	//! 获取产品每个径向包含的距离库数
	unsigned int GetRangeBinOfRad( );

	//! 获取指定Radial的数据
	unsigned char* GetRadialData( unsigned int iRadNum );

	//! 获取指定径向的径向头
	BASE_DATA_HEAD *GetRadialHead( unsigned int iRadNum );

	//! 获取产品文件名字
	const char *GetDataName() { return m_szPath.c_str(); };
};

#endif
