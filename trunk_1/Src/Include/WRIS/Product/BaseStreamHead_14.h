/*! \file basestreamhead_XX6.h 
	\brief 本文件记录了XX6雷达基数据流格式相关结构

	\ingroup GRSGroup
	
	\version 1.01
	\author lava_sdb
	\date 2008/10/29

	\b modify log:
	\arg \c version 1.00,lava_sdb,2008/09/06 创建
	\arg \c version 1.01,lava_sdb,2008/10/29  更新了命名空间 

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GRSInterface
*/


#ifndef _HEADER_XX6BaseDataHeader_20080906
#define _HEADER_XX6BaseDataHeader_20080906

namespace CDYW
{
namespace GRS
{
namespace DataFormat
{
namespace SXX6
{
//////////////////////////////////////////////////////////////////////////
#pragma pack(1)		// one byte alignment

/*! \brief 每一层的结构信息
*/
struct LAYERPARAM  //【24】
{           
	unsigned char Ambiguousp;   /*! 本层退模糊状态【1】
								 - 0=无退模糊状态
								 - 1=软件退模糊
								 - 2=双T退模糊
								 - 3=批式退模糊
								 - 4=双T+软件退模糊
								 - 5=批式+软件退模糊
								 - 6=双PPI退模糊
								 - 9=其他方式
								*/
	unsigned short PRF1;     //!< 本层第一脉冲重复频率，计数单位为1/10Hz【2】
	unsigned short PRF2;     //!< 本层第二脉冲重复频率，计数单位为1/10Hz【2】
	unsigned char Filter;    //!< 滤波器代号【1】
	unsigned short PluseW;    //!< 本层的脉冲宽度，计数单位为微秒【2】
	unsigned short MaxV;      //!< 本层向的最大可测速度，计数单位为厘米/秒【2】
	unsigned short MaxL;      //!< 本层向的最大可测距离，以10米为计数单位【2】
	unsigned short BinWidth;  //!< 本层向反射率数据的库长，以1/10米为计数单位【2】
	unsigned short BinNumber;      //!< 本层扫描的库数【2】
	unsigned short RecodeNumber;   //!< 本层扫描径向个数【2】
	char DataForm;                 /*! 本层中的数据排列方式：【1】
									 - 11 单要素排列 ConZ ,地物消除后的反射率
									 - 12 单要素排列 UnZ  ,原始反射率
									 - 21 按要素排列 ConZ + UnZ
									 - 22, 27按要素排列 ConZ + V + W
									 - 23 按要素排列 UnZ + V + W
									 - 24 按要素排列 ConZ + UnZ + V + W
									 - 25 按要素排列 V + W
									 - 26 按要素排列
									*/
	unsigned char ElevIndex;	//!< 体扫时，本次扫描位于体扫中的序号（第几个仰角）【1】
	unsigned long CutIndex;		//!< 雷达扫描的PPI序号，此序号用于径向传输时的数据匹配，在网络传输出现错误时及时修正【4】
};


/*! \brief XX6基数据格式 雷达性能参数段

*/
struct RADARPERFORMANCEPARAM  //【54】
{
	int AntennaG;                 //!< 天线法向增益，以0.01dB为计数单位【4】
	unsigned short BeamH;         //!< 法线方向上的波束宽度，以1/100度为计数单位【2】
	unsigned short BeamL;         //!< 切线方向上的波束宽度，以1/100读为计数单位【2】
	unsigned char Polarizations;  /*! 极化状况【1】
									 - 0=水平
									 - 1=垂直
									 - 2=双线偏振
									 - 3=其他待定
									*/
	unsigned short SideLobe;         //!<第一旁瓣，以0.01dB为计数单位【2】
	int Power;                     //!< 雷达脉冲峰值功率，以瓦为单位【4】
	int WaveLength;                //!< 波长，以微米为计数单位【4】
	unsigned short LineA;          //!< 接收机动态范围，以0.01dB为计数单位【2】
	short int LineMinPower;       //!< 接收机最小可测功率，计数单位为0.01dBm【2】
	char ClutterP;                 //!< 地杂波消除阈值，计数单位为0.01dB【1】
	char ClutterS;                 //!< 海杂波消除阈值，计数单位为0.01dB【1】
	unsigned char VelocityP;		/*! 速度处理方式【1】
										 - 0=无速度处理
										 - 1=PPP
										 - 2=FFT
									*/
	unsigned char FilterP;			/*! 地杂波消除方式【1】
									 - 0=无地杂波消除
									 - 1=地杂波扣除法
									 - 2=地杂波+滤波器处理
									 - 3=滤波器处理
									 - 4=谱分析处理
									 - 5=其他处理法
									*/
	unsigned char FilterS;            //!< 海杂波消除方式【1】
	//待定
	unsigned char NoiseT;            //!< 噪声消除阈值（0-255）【1】
	unsigned char IntensityR;          /*! 强度估算是否进行了距离订正【1】
									 - 0，无
									 - 1，以进行了距离订正
									*/
	char Reserved[24];                //!< 保留【24】
};

/*! \brief XX6基数据格式 雷达观测参数段

*/
struct RADAROBSERVATIONPARAM  // 【64】
{
	unsigned char SType;             //!< 工作方式【1】
	//1= 搜索模式
	//2= 单强度PPI
	//3= 单强度RHI
	//4= 强度速度PPI
	//5= 体扫
	unsigned char WeatherMode;	     //!< 天气状况【1】
	unsigned short SYear;            //!< 观测记录开始时间的年（2000-）【2】
	unsigned char SMonth;           //!< 观测记录开始时间的月（1-12）【1】
	unsigned char SDay;             //!< 观测记录开始时间的日（1-31）【1】
	unsigned char SHour;            //!< 观测记录开始时间的时（00-23）【1】
	unsigned char SMinute;           //!< 观测记录开始时间的分（00-59）【1】
	unsigned char SSecond;           //!< 观测记录开始时间的秒（00-59）【1】
	unsigned char Calibration;        //!< 标校状态【1】
	//0=无标校
	//1=自动标校
	//2=一星期内人工标校
	//3=一月内人工标校
	//其他码不用
	LAYERPARAM LayerlInfo;     //!< 层参数结构（各层扫描状态设置）【24】
	//注：当扫描方式是PPI或RHI时，只在第一个元素中填写，其余为0
	unsigned short int RHIA;           //!< 做RHI时的所在方位角，计数单位为1/100度，做PPI和立体扫描时为65535【2】
	short int RHIL;                  //!< 做RHI时的最低仰角，计数单位为1/100度，做PPI和立体扫描时为对应的仰角【2】
	short int RHIH;                  //!< 做RHI时的最高仰角，计数单位为1/100度，做其他扫描时为-32768【2】
	char Reserved[24];             //!< 保留【24】
};

/*! \brief 雷达Cut扫描头

	本结构体作为XX6和FL的中间结构体，用于XX6和FL的CUT头的互换
*/
typedef struct _SCANCUTHEADER  //【154】
{
	RADARPERFORMANCEPARAM  RadarPerformanceInfo; //!< 雷达性能数据头【54】
	RADAROBSERVATIONPARAM  RadarObservationInfo;//!< 雷达观测数据头【64】
	unsigned short int RadialNum;	//!< 本层扫描的包含径向数。（360/径向角度分辨率）【2】
	unsigned short int MaxLV;     //!< 速度最大可测距离，批模式用，以10米为计数单位【2】
	unsigned short int wavForm;   //!< 当前Cut扫描形式（批处理/HCD/LCD/LCS）
	unsigned short int usRefBinWidth;  //!< 本扫描反射率数据库长，以10米为计数单位【2】
	unsigned short int usDopBinWidth;  //!< 本扫描多普勒数据库长，以10米为计数单位【2】
	unsigned short int usRefBinNum;	   //!< 本扫描每径向反射率的库数【2】
	unsigned short int usDopBinNum;    //!< 本扫描每径向多普勒的库数【2】
	char Reserved[24]; //!<【24】
}tagScanCutHeader;

struct RADARDATAHEADER{
	RADARPERFORMANCEPARAM  RadarPerformanceInfo;  //【54】
	RADAROBSERVATIONPARAM  RadarObservationInfo;  //【62】
	char Reserved[24];  //【24】
};

//! 径向数据存放每个径向探测的回波数据，以极坐标方式排列，其结构如下：
struct LineDataBlock {
	unsigned long CutIndex;	   // 雷达扫描的PPI序号，此序号用于径向传输时的数据匹配，在网络传输出现错误时及时修正【4】

	unsigned char lastRadial;  // 是否是本次扫描的最后一个径向：1，是。【1】
	short Elev;                //仰角，计数单位1/100度【2】
	unsigned short Az;         //方位，计数单位1/100度【2】
	char Longtitude[16];       /*经度，以字符串记录1/100秒【16】
							   xx6:字符格式--   E180/23/23
											
							   xx2:经度，以字符串记录1/100秒【16】
							   */
	char Latitude[16];         //纬度，以字符串记录1/100秒【16】
								/*
								xx6:字符格式-- N34/21/25
								
								xx2:纬度，以字符串记录1/100秒【16】
								*/
	float Vs;                  //纵摇，计数单位1/100度【4】
	float Hs;                  //横摇，计数单位1/100度【4】
	unsigned short Course;     //航向，计数单位1/100度【2】
	unsigned short Nv;         //舰速，计数单位 0.01 米/秒【2】
								/*
								xx6:节

								xx2: 加权值 单位为 55/8192 节。 -(dblock[i].Nv - 6144) * 55/8192, > 6144 为负
								*/
	unsigned char CorZ[1024];  //经过地物杂波消除的dBZ值=（CorZ-64）/2【1024】
	unsigned char UnZ[1024];   //不经过地物杂波消除的dBZ值=（UnZ-64）/2【1024】
	char V[1024];      /*!< 速度值，计数单位为最大可测速度的1/127【1024】
	                                                     正值表示远离雷达的速度，负值表示朝向雷达的速度
						无回波时计-128
					        */
	unsigned char W[1024];  //谱宽值，计数单位为最大可测速度的1/512【1024】
	//无回波时计零
};

struct RADARDATA_XX6
{
	unsigned char dataType; // 数据类型:0，扫描数据头； 1，径向数据

	union{
		RADARDATAHEADER  radarHeader; // 雷达数据头【140】
		LineDataBlock  radialData;  // 径向数据 【4149】
	};
};

#pragma pack()


//////////////////////////////////////////////////////////////////////////
};// end namespace XX6
};// end namespace DataFormat
};// end namespace GRS
};// end namespace CDYW

#endif



