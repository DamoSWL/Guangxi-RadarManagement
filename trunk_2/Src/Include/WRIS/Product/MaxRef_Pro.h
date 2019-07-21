/************************************************************************
CMaxRefFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef __SOURCE_MAXREF_FILE_080111
#define __SOURCE_MAXREF_FILE_080111

#include "rvw_pro.h "

//! 基数据及其类似数据解析类
/*!
* 此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
	\arg 回波顶高产品（ET）
	\arg 回波底高产品（BT）
	\arg 最大反射率产品（MAXREF）

* \author lava_sdb
* \ingroup group_UT
*/
class MaxRefPro:
	public RVWPro
{
public:
	MaxRefPro(void);
	~MaxRefPro(void);

	unsigned char *m_pDataV;

	int GetSizeOfRadial();

	//! 获取数据项每个径向的大小
	int GetSizeOfData();

	int Open(string szPath);	
	int OpenBuff( char *pBuff,int iLen, bool bCopy = true );

	unsigned char* GetHeightData( unsigned int iRadNum );

	inline float GetElevation()
	{
		return 0.0f;
	}
};


#endif
