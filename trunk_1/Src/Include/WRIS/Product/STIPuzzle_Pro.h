/*! \file STI_Pro.h
	本文件定义了 STIPZ 产品解析的相关结构体
*/  

#pragma once

//#include "ss_pro.h"
#include "rvw_pro.h"
#include <vector>
#include <string>
using namespace std;

#define MaxVOLNum 30    // 最大体扫计数 
#define MaxCellNum 100   ///每个时刻可跟踪的最大风暴个数

#define RefThrCnt 1//风暴反射率识别阈值个数    尝试只考虑一个阈值35dBZ

#pragma pack(1)

//----------------------------SSPZ----------------------------

//////风暴段特征量数据结构
typedef struct{
// 	float EL;     ///所在的仰角，度
// 	float AZ;     ///风暴段所在的方位，度
	int Col;     ///风暴段所在的列
	int BegRow;     ///风暴段开始所在的行
	int EndRow;     ///风暴段结束所在的行

	float DBZEmax;     ///最大反射率因子，dBZ
	int DBZEmaxW;		///最大反射率因子所在位置    ？？？？？是否需要
	float MWL;         ///风暴段的质量权重长度，kg/km2
	float MWLS;        ///风暴段的质量权重长度平方，kg/km
	int Tz;          ///对应的反射率门限，dBZ----------------------------有用？？？？？？？？？？？？？？？？

}STORMSEGMENTofPZ;


//////风暴分量特征描述数据结构
typedef struct{
	long int Lon;
	long int BegLat;
	long int EndLat;
}COMPOUTLINE;//存储风暴分量的轮廓信息（即每个段的位置信息）

//////存储风暴分量的轮廓信息（即每个位置的经纬度值）
typedef struct {
	long int Lontitude;
	long int Latitude;
}COMPOUTLINEofPZ;

typedef struct{
 	float iHeight;    ///所在的高度，m
// 
// 	float AC;      ///质量权重中心所在的方位,度
// 	float RC;      ///质量权重中心所在的距离,km
// 
// 	float XC;        ///质量权重中心所在的X坐标,km
// 	float YC;        ///质量权重中心所在的Y坐标，km
// 	float HC;        ///质量权重中心所在的高度，km
	float DBZECmax;  ///该风暴分量内的最大反射率，dBZ
	int DBZECmaxCol; ///最大值对应的网格位置
	int DBZECmaxRow;
	//float MC;        ///质量权重面积
	unsigned short SegNumofComp;
	char Reserve[2];      //!< 保留 【2】
}STORMCOMPONENTofPZ;

//////风暴体特征描述数据结构
typedef struct{
	unsigned long offset; // 第一个风暴组件对应风暴组件数据偏移；
	unsigned short countOfComponent;	// 包含风暴分量个数
	///
	unsigned short Serial;  ///风暴体序列号

// 	float LowEL;   ///风暴体对应的最低仰角，度
// 	float HighEL;  ///风暴体对应的最高仰角，度
	float TOP;     ///风暴体的顶高,m    以前是质量权重高度，km
	float BASE;    ///风暴体的底高,m

// 	float HSC;     ///风暴体质量权重中心所在的高度,km
// 	float RS;      ///风暴体质量权重中心所在的距离,km
// 	float AS;      ///风暴体质量权重中心所在的方位，度
// 	float XSC;     ///风暴体质量权重中心所在的X坐标，km
// 	float YSC;     ///风暴体质量权重中心所在的Y坐标，km
// 	float MSV;     ///风暴体的质量权重体积
	float VIL;     ///风暴体对应的垂直累积液态含水量，kg/m2
	float Zmax;    ///风暴体内的最大反射率，dBZ
	float H_Zmax;  ///风暴体内的最大反射率对应的高度，km
	long int Lontitude_Zmax; ///最大值对应经纬度
	long int Latitude_Zmax;
// 	int Col_Zmax; ///最大值对应经纬度
//  	int Row_Zmax;

}STORMCELLofPZ;

typedef struct _SSParaofPZ
{
	/*
	风暴段搜索所需的输入参数
	*/
	int Tz[RefThrCnt];  ///反射率第一下限，dBZ
	int TDz;  ///反射率第二下限，dBZ
	int Np;   ///容许点数
	//float L[7];    ///风暴段长度阀值，km---------------应该不能用长度来作为阈值
	float L[RefThrCnt];    ///风暴段长度阀值，度

	/*
	风暴分量搜索所需的输入参数
	*/
	float Area;  ///风暴分量的面积门限，平方km
	//float AT;  ///相邻风暴段方位间距阀值，度
	//float Lch;  ///相邻风暴段重叠距离，km
	float Lch;  ///相邻风暴段的间隔阈值(横向和纵向相同)，度----------------------------------网格数好还是经纬度间隔好？？？？？？？？？
	int   Nd;     ///最少风暴段数

	/*
	风暴体搜索所需的输入参数
	*/
	float ThrSearRad[3];   ///风暴分量垂直相关的搜索半径，km
	float ThrSearArea;   ///风暴分量垂直相关的重叠面积阈值，度
	//float ThrEleMer; ///判断风暴合并的仰角门限，度
	float ThrHorMer;  ///判断风暴合并的水平距离门限，km 
	float ThrHeiMer;   ///判断风暴合并的高度门限，m
	float ThrHorDel;   ///相邻风暴体的水平删除门限，km
	float ThrDepDel;  ///相邻风暴体的高度删除门限，km

	/*
	风暴的搜索范围
	*/
// 	float bAzi; // 起始点方位，度
// 	float bDis; // 起始点距离，km
// 	float eAzi; // 终止点方位，度
// 	float eDis; // 终止点距离，km
}tagSSParaofPZ;

//! 输出风暴结构文件头
typedef struct _SSOUTHEADOFPZ {

	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含文件头、产品头、各站点信息(即数据开始偏移)		【4】
	unsigned short RadarsCount;	//站点个数										【2】
	int   StormNum;     //!< 一个体扫内风暴的个数
	SYSTEMTIME startTime;	//!< 体扫起始时间
	SYSTEMTIME endTime;		//!< 体扫结束时间

	/*
	风暴段搜索所需的输入参数
	*/
	int Tz[RefThrCnt];  ///反射率第一下限，dBZ
	int TDz;  ///反射率第二下限，dBZ
	int Np;   ///容许点数
	//float L[7];    ///风暴段长度阀值，km---------------应该不能用长度来作为阈值
	float L[RefThrCnt];    ///风暴段长度阀值，度

	/*
	风暴分量搜索所需的输入参数
	*/
	float Area;  ///风暴分量的面积门限，平方km
	//float AT;  ///相邻风暴段方位间距阀值，度
	//float Lch;  ///相邻风暴段重叠距离，km
	float Lch;  ///相邻风暴段的间隔阈值(横向和纵向相同)，度----------------------------------网格数好还是经纬度间隔好？？？？？？？？？
	int   Nd;     ///最少风暴段数

	/*
	风暴体搜索所需的输入参数
	*/
	float ThrSearRad[3];   ///风暴分量垂直相关的搜索半径，km
	float ThrSearArea;   ///风暴分量垂直相关的重叠面积阈值，度
	//float ThrEleMer; ///判断风暴合并的仰角门限，度
	float ThrHorMer;  ///判断风暴合并的水平距离门限，km 
	float ThrHeiMer;   ///判断风暴合并的高度门限，m
	float ThrHorDel;   ///相邻风暴体的水平删除门限，km
	float ThrDepDel;  ///相邻风暴体的高度删除门限，km

	//边界经纬度，必须采用经纬线为直线的地图
	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4】
}tagSSOutHeadofPz;
#pragma pack()


//风暴结构输出信息（体信息+分量信息+轮廓信息）
typedef struct _StormCell
{
	STORMCELLofPZ  cell;
	std::vector<STORMCOMPONENTofPZ>  vecComponent;
	std::vector<COMPOUTLINE>  vecComponentOutLine;
}tagStormCellofPZ;


//----------------------------STIPZ----------------------------
#pragma pack(1)
//! 风暴位置结构体
typedef struct _StormPosition
{
	long int fAScur;    //!< 风暴现在的中心方位，度    X
	long int fRScur;    //!< 风暴现在的中心距离，km    Y
}tagStormPosofPZ;

//! 风暴单元结构体
typedef struct _StormCellTrak
{
	unsigned int iIndex; //!<  所属风暴体的序号
	unsigned int iVol;   //!<  所属体扫的序号
	bool bNewStorm;	     //!<  是否新的风暴体
	char Reserve[3];      //!< 保留 【3】

	STORMCELLofPZ stormCell; //!<  风暴体信息     
	int cntComp;//!<  风暴包括的分量个数

	float SS;		 //!< 风暴体的移速,m/s
	float DS;		 //!< 风暴体的移向,度

	float ASPre;    //!< 前一次体扫时风暴体的中心方位，度     //采用网格或经纬度？？？？？？
	float RSPre;    //!< 前一次体扫时风暴体的中心距离，km

	float ASCur;     //!< 
	float RSCur;

	int IndPre;          //!<  与前一个体扫匹配的风暴体在前一个体扫风暴中的序号
	tagStormPosofPZ PosPre;	 //!<  风暴体位置信息

	int IndCur;	        //!<  当前体扫时匹配的风暴体在当前体扫风暴中的序号
	tagStormPosofPZ PosCur;	//!<  风暴体位置信息

	int cntFort;	    //!<  预测的数据个数
	SYSTEMTIME time;            //!< 观测时间

	unsigned int CompPos;//轮廓信息相对于当前风暴信息开始位置的偏移
	unsigned int FortPos;//预测信息相对于当前风暴信息开始位置的偏移
}tagStormCellTrakofPZ;

//! 风暴追踪路径预测结构体
// typedef struct _StormCellTrakAFort
// {
// 	tagStormCellTrakofPZ trak;
// 	vector<tagStormPosofPZ> relStorm;	//!<  相关风暴体信息
// 	vector<tagStormPosofPZ> forts;	    //!<  风暴体移动预测路径
// }tagStormCellTrakAFortofPZ;


//! 体扫风暴体结构体
// typedef struct _VolStormTrak
// {	
// 	SYSTEMTIME time;
// 	int nStormTrak;	 //!< 包含的Storm的个数
// 	vector<tagStormCellTrakAFortofPZ> traks;
// }tagVolStormTrakofPZ;

//! STI 产品头
typedef struct _STIHeader
{
	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含文件头、产品头、各站点信息(即数据开始偏移)		【4】
	unsigned short RadarsCount;	//站点个数
	char Reserve1[2];			//!< 保留 【2】
	float Area; //!<  输入参数，风暴面积
	int nStrom; //!<  风暴体个数
	SYSTEMTIME tStart;	//!<  风暴追踪起始时间
	SYSTEMTIME tStop;	//!<  风暴追踪终止时间
	int nVol;	//!<  风暴追踪使用的体扫文件个数

	//边界经纬度，必须采用经纬线为直线的地图
	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4

	unsigned int StormCellPos[MaxCellNum];    //每一个风暴体相对于文件起始位置的偏移

	char Reserve[100];      //!< 保留 【100】
}tagStiHeaderofPZ;


typedef struct _StormCellTrakAFort1
{
	tagStormCellTrakofPZ trak;
	int iVol;	 //!< 所属体扫的序号
	//vector<tagStormPosofPZ> relStorm;	//!< 相关风暴体信息
	std::vector<STORMCOMPONENTofPZ>  vecComponent;
	std::vector<COMPOUTLINEofPZ>  vecComponentOutLine;

	vector<tagStormPosofPZ> forts;	//!< 风暴体移动预测路径
}tagStormCellTrakAFort1ofPZ;/*! 用于按照条绘制*/

							//! 体扫风暴体结构体
typedef struct _VolStormTrak
{
	SYSTEMTIME time;
	int nStormTrak;	 //!< 包含的Storm的个数
	vector<tagStormCellTrakAFort1ofPZ> traks;
}tagVolStormTrakofPZ;

//! 风暴体总信息结构体
typedef struct _StormTrakPZ
{
	char idx[4];  //!< 风暴体的序号，这是一个全局的序号，由'A' 开始，到'Z' 后，又从头循环
	SYSTEMTIME timeStart;  //!< 风暴起始时间
	SYSTEMTIME timeEnd;	 //!< 风暴停止时间
	unsigned int nStormTrak;	//!< 包含的Storm的个数
	bool bVisible;  //!< 是否显示此风暴体
					//unsigned int TraksNum;
	char Reserve[15];
}tagSTIPZ;

//! 风暴体结构体
typedef struct _StormTrak
{	
	tagSTIPZ STIPZInfo;
	vector<tagStormCellTrakAFort1ofPZ> traks;  //!< 风暴追踪路径
}tagStormTrakofPZ;


#pragma pack()

