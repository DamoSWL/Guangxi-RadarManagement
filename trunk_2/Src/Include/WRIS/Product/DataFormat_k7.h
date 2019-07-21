/*! \file stk7.h 
	\brief 本文定义了空七所的气象产品数据结构
	\ingroup SealsGroup
	
	\version 1.0
	\author lava_sdb
	\date 2008/10/29

	\b modify log:
	\arg \c version 1.0,lava_sdb,2008/10/29 

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup SealsInterface
*/


#ifndef __HEADER_stk7_20081023
#define __HEADER_stk7_20081023

namespace CDYW
{
namespace GRS
{
namespace DataFormat
{
namespace K7
{

//////////////////////////////////////////////////////////////////////////

#pragma pack(1)

//! 性能参数段
typedef struct{
	int AntennaG;                //天线法向增益，以0.01dB为计数单位【4】
	unsigned short BeamH;         //法向方向上波束宽度，以0.01度为计数单位【2】
	unsigned short BeamL;         //切线方向上波束宽度，以0.01度为计数单位【2】
	unsigned char Polarizations; //偏振情况【1】
	//0=水平
	//1=垂直
	//2=双线偏振
	//3=其他
	short SideLobe;              //第一旁瓣，以0.01dB为计数单位【2】
	int Power;                   //雷达脉冲峰值功率，以瓦为单位【4】
	int WaveLength;              //波长，以微米为计数单位【4】
	unsigned short LineA;        //接收机动态范围，以0.01dB为计数单位【2】
	short LineMinPower;          //接收机最小可测功率，计数单位为0.01dBm【2】
	char ClutterP;               //地杂波消除阈值，计数单位为0.2dB【1】
	char ClutterS;               //海杂波消除阈值，计数单位为0.2dB【1】
	unsigned char VelocityP;     //速度处理方式【1】
	//0=无速度处理
	//1=PPP
	//2=FFT
	unsigned char FilterP;       //地物杂波消除方式【1】
	//0=无地物杂波消除
	//1=地物杂波图扣除法
	//2=地物杂波+滤波器处理
	//3=滤波器处理
	//4=谱分析处理
	//5=其他
	unsigned char FilterS;       //海杂波消除方式【1】
	//待定
	unsigned char NoiseT;        //噪声消除阈值(0-255)【1】
	unsigned char Intensity;     //强度估算是否进行了距离订正【1】
	//0=无
	//1=以进行了距离订正
	char Reserved[24];           //保留【24】
}RADARPERFORMANCEPARAM;  // 【54】


//! 观测参数段
typedef struct{
	char Longitude[16];              //产品极坐标中心所在的经度【16】
	char Latitude[16];               //产品极坐标中心所在的纬度【16】
	unsigned char ProductNumber;     //产品代号，见表2【1】
	unsigned char LayerNumber;       //对于按层分类的产品，本层在整个体扫中所处的层数；对于不存在层的产品，该值取为0【1】
	short HeightAngle;               //PPI和体扫时为仰角值，单位为0.01度【2】
	//CAPPI时为高度值，单位为米
	//对于不存在层的产品，该值取为－1000
	unsigned char WeatherMode;       //天气状况【1】
	unsigned short Year;             //观测时间的年（2000-）【2】
	unsigned char Month;             //观测时间的月（1-12）【1】
	unsigned char Day;               //观测时间的日（1-31）【1】
	unsigned char Hour;              //观测时间的时（00-23）【1】
	unsigned char Minute;            //观测时间的分（00-59）【1】
	unsigned char Second;            //观测时间的秒（00-59）【1】
	unsigned char Calibration;       //标校状态【1】
	//0=无标校
	//1=自动标校
	//2=一星期内人工标校
	//3=一月内人工标校
	//其他码不用
	unsigned char Ambiguousp;        //产品退速度模糊状态【1】
	//0=无速度退模糊状态
	//1=软件退数度模糊
	//2=双T退速度模糊
	//3=批式退速度模糊
	//4=双T+软件退速度模糊
	//5=批式+软件退速度模糊
	//6=双PPI退速度模糊
	//9=其他方式
	unsigned short PRF1;             //第一脉冲重复频率，计数单位为0.1Hz【2】
	unsigned short PRF2;             //第二脉冲重复频率，计数单位为0.1Hz【2】
	unsigned char Filter;            //滤波器代号【1】
	unsigned short PluseW;           //脉冲宽度，计数单位为微秒【2】
	unsigned short MaxV;             //最大可测速度，计数单位为0.01米/秒【2】
	unsigned short MaxL;             //最大可测距离，以10米为计数单位【2】
	unsigned short BinWidth;         //产品数据的库长，以0.1米为计数单位【2】
	unsigned short BinNumber;        //产品的库数【2】
	unsigned short RecodeNumber;     //产品的径向个数【2】
	char Reserved[24];               //保留【24】
}POLAROBSERVATIONPARAM;


//! 极坐标文件头
typedef struct{
	RADARPERFORMANCEPARAM  RadarPerformanceInfo;//【54】
	POLAROBSERVATIONPARAM  RadarObservationInfo;//【87】
	char Reserved[24];                                 //【24】
}POLARPRODUCTHEADER_K7;

//! 极坐标径向数据联合结构体
typedef union {
	unsigned char ZP[1024];    //经过地物杂波消除的dBZ值=（CorZ-64）/2
	unsigned char UP[1024];    //不经过地物杂波消除的dBZ值=（UnZ-64）/2
	char VP[1024];             //速度值，计数单位为最大可测速度的1/127
	//正值表示远离雷达的速度，负值表示朝向雷达的速度
	//无回波时计-128
	unsigned char WP[1024];    //谱宽值，计数单位为最大可测速度的1/512
	//无回波时计零
	unsigned char ZC[1024];    //给定高度层ZP的CAPPI值=（ZC-64）/2
	unsigned char UC[1024];    //给定高度层UP的CAPPI值=（UC-64）/2
	char VC[1024];             //给定高度层VP的CAPPI值，单位同V
	unsigned char WC[1024];    //给定高度层WP的CAPPI值，单位同W
	unsigned char CR[1024];    //垂直最大回波强度值=（CR-64）/2
	unsigned short ET[1024];    //回波顶高，以1米为单位
	unsigned short EB[1024];    //回波底高，以1米为单位
	unsigned short RZ[1024];    //雨强值＝RZ/2，以0.01mm/h为单位
	unsigned short VIL[1024];   //垂直累积含水量，以0.01Kg/m*m米为单位
	unsigned short HP[1024];    //任意小时雨量累积，以0.01mm为单位
	short RV[1024];             //径向散度，以E-4/s为单位 即(1/(1000s))
	short AR[1024];             //方位涡度，以E-4/s为单位
	unsigned short CS[1024];    //合成切变，以E-4/s为单位
}POLARDATA;

//! 极坐标类径向数据
typedef struct{
	unsigned short AZ;           //方位，计数单位0.01度【2】
	POLARDATA Data;              //数据块【1024】
}PolarDataBuffer;


//////////////////////////////////////////////////////////////////////////
/// 直角坐标探测参数
typedef struct{
	char Longitude[16];              //雷达中心所在的经度【16】
	char Latitude[16];               //雷达中心所在的纬度【16】
	unsigned char ProductNumber;     //产品代号，见表2【1】
	short HeightAngle;               //RHI时的方位角，单位为0.01度【2】
	//其他情况下为0
	short Range;                     //有效探测距离（单位为千米）【2】
	short Resolution;                //网格距（单位为米）【2】
	short XLength;                   //水平格点数【2】
	short YLength;                   //垂直格点数【2】
	short XRadar;                    //雷达中心的位置X（X-左上角为零）【2】
	short YRadar;                    //雷达中心的位置Y（Y-左上角为零）【2】
	unsigned short Year;             //观测时间的年（2000-）【2】
	unsigned char Month;             //观测时间的月（1-12）【1】
	unsigned char Day;               //观测时间的日（1-31）【1】
	unsigned char Hour;              //观测时间的时（00-23）【1】
	unsigned char Minute;            //观测时间的分（00-59）【1】
	unsigned char Second;            //观测时间的秒（00-59）【1】
	char Reserved [24];              //备用24字节【24】
} SQUAREOBSERVATIONPARAM;


typedef struct{
	short Speed;          //径向风速，计数单位为0.1米/秒
	unsigned short Az;    //方位，计数单位为0.01度
}VAD;

typedef struct{
	unsigned short Speed; // 强回波移速，计数单位为 0.1米/秒
	unsigned short Angle; // 强回波移向，计数单位为0.01度

}CL;


typedef union {
	unsigned char ZR;    //经过地物杂波消除的dBZ值=（CorZ-64）/2
	unsigned char UR;    //不经过地物杂波消除的dBZ值=（UnZ-64）/2
	char VR;             //速度值，计数单位为最大可测速度的1/127
	//正值表示远离雷达的速度，负值表示朝向雷达的速度
	//无回波时计-128
	unsigned char WR;    //谱宽值，计数单位为最大可测速度的1/512
	//无回波时计零
	unsigned char ZS;    //任意垂直剖面的Z值=（ZS-64）/2
	unsigned char US;    //任意垂直剖面的UZ值=（US-64）/2
	char VS;             //任意垂直剖面的V值，单位同VR
	unsigned char WS;    //任意垂直剖面的W值，单位同WR
	char CR;             //垂直最大回波强度值=（CR-64）/2
	CL cl;				 //强回波移向移速
	VAD vad;             //速度方位显示
}SQUAREDATA;



typedef struct{
	SQUAREDATA Data;      //数据块，具体数据量由水平格点数和垂直格点数决定
}SquareDataBuffer;

struct _CLHeader {
	int StormNum; // 所检测到的风暴体的个数
};

struct StormBody{
	long longitude; // 风暴中心所在经度，单位，百分之一度；
	long latitude;  // 风暴中心所在纬度，单位，百分之一度；
	unsigned short speed; // 强回波移速。以0.1米/秒为单位；
	unsigned short angle; // 强回波移向。以0.01度为单位；
};

struct _CL{
	StormBody body; // 强风暴信息
	StormBody fort[8]; // 预报信息
};


typedef struct{
	unsigned short Height;  //VAD高度，计数单位为米
	unsigned short Distance;//VAD距离，计数单位为1000米
	unsigned short Speed;   //平均风速，计数单位为0.1米/秒
	unsigned short Az;      //平均风向，计数单位为0.01度
}VADPARAM;


//直角坐标文件头
typedef struct{
	RADARPERFORMANCEPARAM  RadarPerformanceInfo; //【54】
	SQUAREOBSERVATIONPARAM  RadarObservationInfo;//【78】
	char Reserved[24];                                  //【24】
}SQUAREPRODUCTHEADER_K7;




//////////////////////////////////////////////////////////////////////////
typedef struct{
	char Longitude[16];              //雷达中心所在的经度【16】
	char Latitude[16];               //雷达中心所在的纬度【16】
	unsigned char ProductNumber;     //产品代号，见表2【1】
	unsigned char RecordNumber;      //记录条数，表征一共有几组风场数据【1】
	unsigned char BinNumber;         //距离库数【1】
	unsigned short BinWidth;         //距离库长，单位为米【2】
	unsigned short StartHeight;      //起始高度，单位为米【2】
	char Reserved [24];              //备用24字节【24】
}VECTOROBSERVATIONPARAM;



typedef struct {
	RADARPERFORMANCEPARAM  RadarPerformanceInfo;  //【54】
	VECTOROBSERVATIONPARAM  RadarObservationInfo; //【63】
	char Reserved[24];                                  // 【24】
}VECTORPRODUCTHEADER;

typedef struct{
	unsigned short Year;             //观测时间的年（2000-）
	unsigned char Month;             //观测时间的月（1-12）
	unsigned char Day;               //观测时间的日（1-31）
	unsigned char Hour;              //观测时间的时（00-23）
	unsigned char Minute;            //观测时间的分（00-59）
	unsigned char Second;            //观测时间的秒（00-59）
	unsigned short AZ;    //矢量角度,计数单位为0.01度
	unsigned short Val;  //矢量大小,计数单位为0.01m/s
	char Reserved[24];               //备用24字节
}VectorDataStruct;

#pragma pack()

//////////////////////////////////////////////////////////////////////////

}; // end namespace K7
}; // end namespace DataFormat
}; // end namespace GRS
}; // end namespace CDYW


#endif // __HEADER_stk7_20081023
