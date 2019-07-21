/*! \file VWP_Pro.h
	本文件实现了 VWP 产品解析类                                                                  
*/  

#ifndef _VWP_FILE_080618
#define _VWP_FILE_080618

#include "rvw_pro.h"

#include < list >
using namespace std;

#pragma pack(4)
 
//! VWP产品头
typedef struct __VWP_HEADER { 
	int heiCnt;        //!< 本产品分析的高度个数	
	float maxVh;       //!< 最大风速
	float maxWind;     //!< 最大风速对应的风向
	float maxVhHeight; //!< 最大风速所在的高度
	float VOLCnt;	   //!< 体扫个数
	float HorRan;      //!< 计算采用的距离
	float fRMS;        //!< 计算采用的均方差阀值
	float fSYM;        //!< 计算采用的对比性阀值
}tagVWPHead;


//! VWP数据结构体
typedef struct _VWPData{
	long lTime;       //!< 时间
	float fHei[30];   //!< 高度, 公里
	float fVh[30];    //!< 风向, 弧度
	float fWind[30];  //!< 风速, m/s
	float fFC[30];    //!< 均方根误差
}tagVWPData;

#pragma pack()

//!本类用于解析VWP产品
/*! \class 
	此类用于解析VWP数据及其类似格式的数据，目前支持的数据类型有：
	\速度方位显示垂直风廓线（VWP）

	\author lava_sdb
	\ingroup group_UT
*/
class VWPPro:
	public RVWPro
{
	long m_lTime[30];

	float *m_pVWPData;
	unsigned char *m_pMakeFileBuff;
public:
	tagVWPHead m_VWPHeader;

	float maxVh;  //!< 最大风速
	float maxWind;  //!< 最大风速对应的风向
	float maxVhHeight;  //!< 最大风速所在的高度
	float HorRan;
	float fRMS;
	float fSYM;

	list< tagVWPData >  m_lsVWP;

	int m_iDisVWP;  //!< 显示的 VWP的个数
public:
	VWPPro(void);
	~VWPPro(void);

	//! 获取显示的VWP时次的个数
	int GetDisCount();

	//! 设置显示的VWP时次的个数
	void SetDisCount( int cntVWP );

	int Open(string szPath);
	int OpenBuff( char *pBuff, int iLen);

	/*! 根据当前数据构造VWP文件
		\param ppData
		\return > 0 成功，返回缓冲长度
		        < 0 失败
	*/
	int MakeVWPFile( unsigned char **ppData );

};




#endif