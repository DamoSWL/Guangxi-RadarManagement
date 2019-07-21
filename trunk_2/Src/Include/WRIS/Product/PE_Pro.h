
#pragma once

#include "rvw_pro.h"
#include "time.h"
#include <vector>
#include <string>
using namespace std;

#pragma pack(1)

//数据输入定义
typedef struct _WMPEOFVOLINFILE_
{
	//人工增雨预警指标
	float minPCRWarning;//组合反射率因子,dBz
	float maxPCRWarning;
	float minPVILWarning;//垂直积分液态水含量,kg/m2
	float maxPVILWarning;
	float minPETWarning;//回波顶高,km

	//人工防雹预警指标
	float minHCRWarning;//组合反射率因子,dBz
	float minHVILWarning;//垂直积分液态水含量,kg/m2
	float minHETWarning;//回波顶高,km
	float mintH_45Warning;//45dBz强回波伸展高度,km
	float mintH_0Warning;//负温区回波厚度,km
	//人工防雹作业指标
	float minHCRTask;
	float minHVILTask;
	float minHETTask;
	float mintH_45Task;
	float mintH_0Task;

}tagWMPEofVOLInFileHead;
//目标云块识别结果标识
enum RecogniseType
{
	Warining = 1,  //警告
	Working        //作业
};

//产品头信息
typedef struct {
	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含站点个数信息(即数据开始偏移)		【4】
	unsigned int InPathNum;//输入数据路径个数，输入数据路径保存在产品头之后，每个路径长度固定为86字节

	unsigned int CloudNum;//识别的目标云块数

	//LAPS对应温度层高度
	float Temperature0;//0℃层高度
	float Temperature_5;//-5℃层高度
	float Temperature_10;//-10℃层高度

	//输入区域经纬度范围
	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4】

	char Reserve[100];      //!< 保留 【100】
}tagWMPEofVOLHead;

//目标云块属性
typedef struct {
	int RecogniseResult;//识别结果标识

	float Area;    ///作业区面积（即达到雷达识别指标的栅格个数与单个栅格面积的乘积）
	float CR;///反射率（平均值）
	float ET;  ///顶高（平均值）
	float VIL; ///含水量（平均值）
	float Hstorm;///风暴高度（反射率因子以30dBz为强度阈值的回波顶高度）（平均值）  只有人工防雹时才需要该属性

	long int MinLon;///最大最小经纬度
	long int MinLat;
	long int MaxLon;
	long int MaxLat;

	unsigned short SegNumofComp;//轮廓点个数

	long int CenterLon;//云块中心经纬度
	long int CenterLat;

	char Reserve[10];      //!< 保留 【18】
}CloudInfo;

//////目标云块的轮廓信息（即每个位置的经纬度值）
typedef struct {
	long int Lontitude;
	long int Latitude;
}CloudOutline;

#pragma pack()


