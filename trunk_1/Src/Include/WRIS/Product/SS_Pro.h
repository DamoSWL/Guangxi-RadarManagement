/************************************************************************
CSSFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef __SS_FILE_070915 
#define __SS_FILE_070915 

#include "rvw_pro.h"
#include "time.h"
#include <vector>
#include <string>
using namespace std;


#define ZUIDAYANGJIAOSHU 30     ///一个体扫最多包含的仰角个数
#define IR 1.21f                ///折射率因子
#define RE 6371                 ///地球半径，km 
#define PIE 1.37f               ///雨强指数，dBZe
#define MMF 486                 ///质量乘法因子，mm6/m3
#define MRM 80                  ///dBZ
#define MWF 53000               ///质量权重因子，hr.kg/km2m2
#define BW 0.55f                ///波束宽度，度

#pragma pack(4)

//! 风暴结构文件头
typedef struct _SSHEAD {
	int   StormNum;     //!< 一个体扫内风暴的个数
	time_t startTime;	//!< 体扫起始时间
	time_t endTime;		//!< 体扫结束时间
	/*
	风暴段搜索所需的输入参数
	*/
	int Tz[7];  //!< 反射率第一下限，dBZ
	int TDz;    //!< 反射率第二下限差值，dBZ
	int Np;     //!< 容许点数
	float L[7];    ///风暴段长度阀值，km

	/*
	风暴分量搜索所需的输入参数
	*/
	float Area;  ///风暴分量的面积门限，平方km
	float AT;  ///相邻风暴段方位间距阀值，度
	float Lch;  ///相邻风暴段重叠距离，km
	int   Nd;     ///最少风暴段数

	/*
	风暴体搜索所需的输入参数
	*/
	float ThrSearRad[3];   ///风暴分量垂直相关的搜索半径，km
	float ThrEleMer; ///判断风暴合并的仰角门限，度
	float ThrHorMer;  ///判断风暴合并的水平距离门限，km 
	float ThrHeiMer;   ///判断风暴合并的高度门限，km
	float ThrHorDel;   ///相邻风暴体的水平删除门限，km
	float ThrDepDel;  ///相邻风暴体的高度删除门限，km

	/*
	风暴的搜索范围
	*/
	float bAzi; // 起始点方位，度
	float bDis; // 起始点距离，km
	float eAzi; // 终止点方位，度
	float eDis; // 终止点距离，km
}tagSSHead;


//////风暴段特征量数据结构
typedef struct{
	float EL;     ///所在的仰角，度
	float AZ;     ///风暴段所在的方位，度

	float RSbeg;     ///起始距离，km
	float RSend;     ///结束距离，km

	float DBZEmax;     ///最大反射率因子，dBZ
	float MWL;         ///风暴段的质量权重长度，kg/km2
	float MWLS;        ///风暴段的质量权重长度平方，kg/km
	int Tz;          ///对应的反射率门限，dBZ

}STORMSEGMENT;


//////风暴分量特征描述数据结构
typedef struct{
	float EL;    ///所在的仰角，度

	float AC;      ///质量权重中心所在的方位,度
	float RC;      ///质量权重中心所在的距离,km

	float XC;        ///质量权重中心所在的X坐标,km
	float YC;        ///质量权重中心所在的Y坐标，km
	float HC;        ///质量权重中心所在的高度，km
	float DBZECmax;  ///该风暴分量内的最大反射率，dBZ
	float MC;        ///质量权重面积

}STORMCOMPONENT;

//////风暴体特征描述数据结构
typedef struct{
	unsigned long offset; // 第一个风暴组件对应风暴组件数据偏移；
	unsigned long countOfComponent;	// 包含风暴分量个数
	///
	unsigned short Serial;  ///风暴体序列号

	float LowEL;   ///风暴体对应的最低仰角，度
	float HighEL;  ///风暴体对应的最高仰角，度
	float TOP;     ///风暴体的顶高,km
	float BASE;    ///风暴体的底高,km

	float HSC;     ///风暴体质量权重中心所在的高度,km
	float RS;      ///风暴体质量权重中心所在的距离,km
	float AS;      ///风暴体质量权重中心所在的方位，度
	float XSC;     ///风暴体质量权重中心所在的X坐标，km
	float YSC;     ///风暴体质量权重中心所在的Y坐标，km
	float MSV;     ///风暴体的质量权重体积
	float VIL;     ///风暴体对应的垂直累积液态含水量，kg/m2
	float Zmax;    ///风暴体内的最大反射率，dBZ
	float H_Zmax;  ///风暴体内的最大反射率对应的高度，km

}STORMCELL;

typedef struct _SSPara
{
	/*
	风暴段搜索所需的输入参数
	*/
	int Tz[7];  ///反射率第一下限，dBZ
	int TDz;  ///反射率第二下限，dBZ
	int Np;   ///容许点数
	float L[7];    ///风暴段长度阀值，km

	/*
	风暴分量搜索所需的输入参数
	*/
	float Area;  ///风暴分量的面积门限，平方km
	float AT;  ///相邻风暴段方位间距阀值，度
	float Lch;  ///相邻风暴段重叠距离，km
	int   Nd;     ///最少风暴段数

	/*
	风暴体搜索所需的输入参数
	*/
	float ThrSearRad[3];   ///风暴分量垂直相关的搜索半径，km
	float ThrEleMer; ///判断风暴合并的仰角门限，度
	float ThrHorMer;  ///判断风暴合并的水平距离门限，km 
	float ThrHeiMer;   ///判断风暴合并的高度门限，km
	float ThrHorDel;   ///相邻风暴体的水平删除门限，km
	float ThrDepDel;  ///相邻风暴体的高度删除门限，km

	/*
	风暴的搜索范围
	*/
	float bAzi; // 起始点方位，度
	float bDis; // 起始点距离，km
	float eAzi; // 终止点方位，度
	float eDis; // 终止点距离，km
}tagSSPara;

#pragma pack()

typedef struct _StormCell
{
	STORMCELL  cell;
	std::vector<STORMCOMPONENT>  vecComponent;
}tagStormCell;


//! SS 产品解析类
/*!
	本类用于解析 SS 产品

* \author lava_sdb
* \ingroup group_UT
*/
class SSPro:
	public RVWPro
{
public:
	SSPro(void);
	~SSPro(void);

	unsigned char *m_pDataT;
	
	int Open(string szPath );	
	int OpenBuff( char *pBuff,int iLen);

	inline float Deg2Float(short data)
	{
		char *pData = (char *)&data;
		return (float)(pData[0] *0.02197265625 + pData[1] * 2.8125);
	}


	//! 获取风暴体个数
	unsigned int GetStormCount( );

	//! 获取指定风暴体的风暴组件个数
	/*!
	\param iIndex 风暴体的序号
	*/
	unsigned int GetStormComponentCount(int iIndex);
	//! 获取指定风暴体的相关数据
	/*!
	\param iIndex 风暴体的序号
	*/

	STORMCELL *GetStormCell(int iIndex);
	//! 获取制定风暴体的相关风暴组件
	/*!
		\param iIndex 风暴体的序号
		\param EL Cut序号
	*/
	STORMCOMPONENT *GetStormComponentOfCell( int iIndex, float EL );

	//! 获取风暴体的首个风暴组件
	/*!
	\param iIndex 风暴体的序号
	*/
	STORMCOMPONENT *GetStormComponentOfCell( int iIndex);
	tagSSHead m_SSHeader;

};


#endif
