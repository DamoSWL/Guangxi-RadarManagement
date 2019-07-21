#pragma once
#include <Winbase.h>
#define maxHeiNum 150	//最多的高度层
//! 拼图产品头
#pragma pack(1)
//站点信息
typedef struct _PZ_RADAR_INF//【96】
{
	char Station[20];    //!< 站点名  【20】
	char StationNo[20];  //!< 站点编号  【20】
	char RadarType[20];   //!< 雷达型号  【20】
	long int LongitudeV; //!< 天线所在径度的数值表示，单位取1／360000度(东经为整，西径为负)   **  【4】
	long int LatitudeV;  //!< 天线所在纬度的数值表示，单位取1／360000度(北纬为正，南纬为负)   **  【4】
	long int Height;	 //!< 天线所在的海拔高度，以mm为单位    **  【4】
	SYSTEMTIME DataTime; //年月日时分秒（所有参与拼图数据的开始时刻）		【16】
	unsigned short int MaxDis;//雷达站最大扫描距离，单位：km
	char Reserve[6];      //!< 保留 【8】
}tagPzRadarInf;

//CR拼图头信息
typedef struct _CR_HEAD		//【96】
{
	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含站点个数信息(即数据开始偏移)		【4】
	unsigned short RadarsCount;	//站点个数										【2】
	SYSTEMTIME StartTime;		//年月日时分秒（参与拼图数据的开始时刻）		【16】
	SYSTEMTIME EndTime;			//年月日时分秒（参与拼图数据的结束时刻）		【16】
	//边界经纬度，必须采用经纬线为直线的地图
	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4】

	unsigned int LenofWin;		//拼图窗口大小 1/1000度（WGS84）或米（mercator）【4】
	unsigned int WidthofWin;	//拼图区域x方向的网格数		【4】
	unsigned int HeightofWin;	//拼图区域y方向的网格数		【4】

	unsigned char ProjectionMode;//拼图所采用的投影方式（网格大小的单位）		【1】
	char Reserve[15];      //!< 保留 【15】
}tagCRHead, tagETEBPZHead;

//CAPPI拼图头信息
typedef struct _CAPPI_HEAD		//【100】
{
	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含站点个数信息(即数据开始偏移)		【4】
	unsigned short RadarsCount;	//站点个数										【2】
	SYSTEMTIME StartTime;		//年月日时分秒（参与拼图数据的开始时刻）		【16】
	SYSTEMTIME EndTime;			//年月日时分秒（参与拼图数据的结束时刻）		【16】
	//边界经纬度，必须采用经纬线为直线的地图
	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4】

	int iHeight;				//切割高度 m		【4】
	unsigned int LenofWin;		//拼图窗口大小 1/1000度（WGS84）或米（mercator）		【4】
	unsigned int WidthofWin;	//拼图区域x方向的网格数		【4】
	unsigned int HeightofWin;	//拼图区域y方向的网格数		【4】

	unsigned char ProjectionMode;//拼图所采用的投影方式（网格大小的单位）		【1】
	char Reserve[15];      //!< 保留 【15】
}tagCAPPIHead;

//三维格点拼图头信息
typedef struct _RTDPZ_HEAD		//【2954】
{

	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含文件头、产品头、各站点信息(即数据开始偏移)		【4】
	unsigned short RadarsCount;	//站点个数										【2】
	SYSTEMTIME StartTime;		//年月日时分秒（参与拼图数据的开始时刻）		【16】
	SYSTEMTIME EndTime;			//年月日时分秒（参与拼图数据的结束时刻）		【16】
	//边界经纬度，必须采用经纬线为直线的地图
	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4】

	unsigned int iHeight;		//高度上的切割间隔值，单位米	【4】
	unsigned int LenofWin;		//拼图窗口大小，1/1000度			【4】
	unsigned int RealCutNum;	//纵向上实际切割层数			【4】
	unsigned int WidthofWin;	//拼图区域x方向的网格数		【4】
	unsigned int HeightofWin;	//拼图区域y方向的网格数		【4】
	unsigned int LayerPos[maxHeiNum];    //每一层的相对于文件起始位置的偏移

	unsigned char ProjectionMode;//拼图所采用的投影方式（网格大小的单位）		【1】
	char Reserve[15];      //!< 保留 【15】
}tagRTDPzHead;


//  [tagCRHead]-[tagPzRadarInf]...[tagPzRadarInf]-[Data]
//	|			HeadLen							|
//层头
//  [tagLayerHead]-[tagLayerColHead] ..[tagLayerColHead] -[Data]
typedef struct _LAYER_HEAD		//【96】
{
	unsigned int LayerSize;			//当前层的存储数据大小
	char Reserve[64];
}tagLayerHead;
//每一层里面每一列数据标识
typedef struct _LAYER_COL_HEAD		//【96】
{
	unsigned int ColPos;		//每一列的起始偏移，从当前层的数据开始位置，不包括列数据标识
	unsigned short StartPos;		//有数据的开始位置
	unsigned short EndPos;		//有数据的结束位置
}tagLayerColHead;

//VIL拼图头信息
typedef struct _VILPZ_HEAD		//【2946】
{
	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含站点个数信息(即数据开始偏移)		【4】
	unsigned short RadarsCount;	//站点个数										【2】
	SYSTEMTIME StartTime;		//年月日时分秒（参与拼图数据的开始时刻）		【16】
	SYSTEMTIME EndTime;			//年月日时分秒（参与拼图数据的结束时刻）		【16】
	//边界经纬度，必须采用经纬线为直线的地图
	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4】

	unsigned int LenofWin;		//拼图窗口大小 1/100度		【4】
	unsigned int WidthofWin;	//拼图区域x方向的网格数		【4】
	unsigned int HeightofWin;	//拼图区域y方向的网格数		【4】

	unsigned char ProjectionMode;//拼图所采用的投影方式（网格大小的单位）		【1】
	char Reserve[15];      //!< 保留 【15】
}tagVILPZHead;

//RCS拼图头信息
typedef struct _RCS_HEAD		//【96】
{
	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含站点个数信息(即数据开始偏移)		【4】
	unsigned short RadarsCount;	//站点个数										【2】
	SYSTEMTIME StartTime;		//年月日时分秒（参与拼图数据的开始时刻）		【16】
	SYSTEMTIME EndTime;			//年月日时分秒（参与拼图数据的结束时刻）		【16】
	//边界经纬度，必须采用经纬线为直线的地图
	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4】

	unsigned int iInLoLaCnt;				//参与切割的经纬度点数		【4】
	unsigned int iHeight;				//拼图高度分辨率，单位米
	unsigned int LenofWin;		//拼图窗口大小 1/1000度		【4】
	unsigned int WidthofWin;	//拼图区域x方向的网格数		【4】
	unsigned int HeightofWin;	//拼图区域y方向的网格数		【4】

	unsigned char ProjectionMode;//拼图所采用的投影方式（网格大小的单位）		【1】
	char Reserve[15];      //!< 保留 【15】
}tagRCSPZHead;

//CAPPIPZ算法传入的文件结构体
typedef struct _RCSPZ_Longitude_Latitude_
{
	long int LongitudeV; //!< 天线所在径度的数值表示，单位取1／360000度(东经为整，西径为负)   **  【4】
	long int LatitudeV;  //!< 天线所在纬度的数值表示，单位取1／360000度(北纬为正，南纬为负)   **  【4】
}tagRCSPZLonLat;
typedef struct _RCSINFILE_
{
	char cDataMode;					//(H)数据模式，0：无效 1：(曲线)全部经纬度点，2：(直线)直线端点经纬度，3：(折线)各个直线端点经纬度
	char cTmp[3];					//补齐
	unsigned int PointCnt;			//(H)经纬度个数
	tagRCSPZLonLat* ParmPtr;		//(H)数据指针地址，WEB不需要赋值，只需要设置为0，PT用于存放经纬度点数据(tagLonLat)
}tagRCSPZInFileHead;


//RZ拼图头信息
typedef struct _OHPPZ_HEAD		//【  】
{
	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含站点个数信息(即数据开始偏移)		【4】
	unsigned short RadarsCount;	//站点个数										【2】
	SYSTEMTIME StartTime;		//年月日时分秒（参与拼图数据的开始时刻）		【16】
	SYSTEMTIME EndTime;			//年月日时分秒（参与拼图数据的结束时刻）		【16】
	//边界经纬度，必须采用经纬线为直线的地图
	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4】

	unsigned int LenofWin;		//拼图窗口大小 1/100度		【4】
	unsigned int WidthofWin;	//拼图区域x方向的网格数		【4】
	unsigned int HeightofWin;	//拼图区域y方向的网格数		【4】

	float fA;          //!< Z-R参数A
	float fb;          //!<Z-R参数b
	float maxValue;      //!<最大值

	unsigned char ProjectionMode;//拼图所采用的投影方式（网格大小的单位）		【1】
	char Reserve[15];      //!< 保留 【15】
}tagOHPPZHead;



//HPF拼图头信息
typedef struct _HPFPZ_HEAD		//【  】
{
	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含站点个数信息(即数据开始偏移)		【4】
	unsigned short RadarsCount;	//站点个数										【2】
	SYSTEMTIME StartTime;		//年月日时分秒（参与拼图数据的开始时刻）		【16】
	SYSTEMTIME EndTime;			//年月日时分秒（参与拼图数据的结束时刻）		【16】
								//边界经纬度，必须采用经纬线为直线的地图
	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4】

	unsigned int LenofVILPZWin;		//基于计算的VIL拼图窗口大小 1/100度		【4】
	unsigned int LenofHPFPZWin;		//HPF拼图窗口大小 1/100度		【4】

	unsigned int HPFPZNum; //拼图区域HPFPZ有效数据个数		【4】

	float max_Value;      //!<最大值
// 	long int max_Lontitude;//!<最大值对应的经纬度
// 	long int max_Latitude;

	unsigned char ProjectionMode;//拼图所采用的投影方式（网格大小的单位）		【1】
	char Reserve[15];      //!< 保留 【15】
}tagHPFPZHead;

//HPF拼图数据存储结构
typedef struct _HPFPZDATA_
{
	long int Longitude;
	long int Latitude;
	float HPFValue;
}tagHPFPZData;


//ETEB拼图头信息
// typedef struct _ETEBPZ_HEAD		//【2946】
// {
// 	unsigned int Ver;			//!< 版本号   **								【4】
// 	unsigned int HeadLen;		//头大小，包含站点个数信息(即数据开始偏移)		【4】
// 	unsigned short RadarsCount;	//站点个数										【2】
// 	SYSTEMTIME StartTime;		//年月日时分秒（参与拼图数据的开始时刻）		【16】
// 	SYSTEMTIME EndTime;			//年月日时分秒（参与拼图数据的结束时刻）		【16】
// 	//边界经纬度，必须采用经纬线为直线的地图
// 	long int LeftLongitude;		//左边界经度，即最小经度， 单位取1／360000度	【4】
// 	long int RightLongitude;	//右边界经度，即最大经度， 单位取1／360000度	【4】
// 	long int BottomLatitude;	//下边界纬度，即最小纬度， 单位取1／360000度	【4】
// 	long int TopLatitude;		//上边界纬度，即最大纬度， 单位取1／360000度	【4】
// 
// 	unsigned int LenofWin;		//拼图窗口大小 1/100度		【4】
// 	unsigned int WidthofWin;	//拼图区域x方向的网格数		【4】
// 	unsigned int HeightofWin;	//拼图区域y方向的网格数		【4】
// 
// 	unsigned char ProjectionMode;//拼图所采用的投影方式（网格大小的单位）		【1】
// 	char Reserve[15];      //!< 保留 【15】
// }tagETEBPZHead;


//人工影响天气参数估算

//指标定义
struct WMPEPara
{
	//人工增雨预警指标
	float minPCRWarning;//组合反射率因子,dBz
	float maxPCRWarning;
	float minPVILWarning;//垂直积分液态水含量,kg/m2
	float maxPVILWarning;
	float minPETWarning;//回波顶高,km
	//人工增雨作业指标 与预警指标一致，共用
	// 	float minPCRTask;
	// 	float maxPCRTask;
	// 	float minPVILTask;
	// 	float maxPVILTask;
	// 	float minPETTask;

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
};
//数据输入定义
typedef struct _WMPEINFILE_
{
	long int MinLongitude;
	long int MinLatitude;
	long int MaxLongitude;
	long int MaxLatitude;

	double LenofWin;

	WMPEPara WmpePara;

}tagWMPEInFileHead;
//人工作业类型定义
enum WMType
{
	Precipitation_enhancement = 1,
	Hail_suppression
};

struct SegtInfo
{
	int Col;     ///风暴段所在的列
	int BegRow;     ///风暴段开始所在的行
	int EndRow;     ///风暴段结束所在的行
};

//产品头信息
typedef struct {
	unsigned int Ver;			//!< 版本号   **								【4】
	unsigned int HeadLen;		//头大小，包含站点个数信息(即数据开始偏移)		【4】
	unsigned int InPathNum;//输入数据路径个数，输入数据路径保存在产品头之后，每个路径长度固定为84字节

	int WMflag;//作业类型
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
}tagWMPEHead;

//目标云块属性
typedef struct {
	float Area;    ///作业区面积（即达到雷达识别指标的栅格个数与单个栅格面积的乘积）
	float CR;///反射率（平均值）
	float ET;  ///顶高（平均值）
	float VIL; ///含水量（平均值）
	float Hstorm;///风暴高度（反射率因子以30dBz为强度阈值的回波顶高度）（平均值）  只有人工防雹时才需要该属性
	//float HeightofLAPS;//LAPS三个温度对应高度的平均值  作为作业高度的参考值   ？？？？？？存储三个对应值后是否还需要该值

// 	long int CenterLon;///中心经纬度
// 	long int CenterLat;

	long int MinLon;///最大最小经纬度
	long int MinLat;
	long int MaxLon;
	long int MaxLat;

	unsigned short SegNumofComp;//轮廓点个数
	char Reserve[18];      //!< 保留 【18】
}CloudInfo;

//////目标云块的轮廓信息（即每个位置的经纬度值）
typedef struct {
	long int Lontitude;
	long int Latitude;
}CloudOutline;

#pragma pack()