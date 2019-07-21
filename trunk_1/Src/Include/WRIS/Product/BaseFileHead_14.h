/*! \file basefilehead_xx6.h 
	\brief 本文件记录了xx6雷达基数据格式相关结构

	\ingroup GRSGroup
	
	\version 1.0
	\author lava_sdb
	\date 2008/09/06

	\b modify log:
	\arg \c version 1.00,lava_sdb,2008/09/06 创建
	\arg \c version 1.01,lava_sdb,2008/10/29  更新了命名空间 

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GRSInterface
*/

#ifndef _HEADER_XX6BaseStreamDataHeader_20080906
#define _HEADER_XX6BaseStreamDataHeader_20080906

#include "basestreamhead_14.h"

namespace CDYW
{
namespace GRS
{
namespace DataFormat
{
namespace xx6
{
//////////////////////////////////////////////////////////////////////////
#pragma pack(1)		// one byte alignment

//其中，各层扫描状态设置的层参数结构LAYERPARAM的结构如下：
struct LAYERPARAM_14
{//【19】
	unsigned char Ambiguousp;     //本层退模糊状态【1】
	//0=无退模糊状态
	//1=软件退模糊
	//2=双T退模糊
	//3=批式退模糊
	//4=双T+软件退模糊
	//5=批式+软件退模糊
	//6=双PPI退模糊
	//9=其他方式
	unsigned short PRF1;       //本层第一脉冲重复频率，计数单位为1/10Hz【2】
	unsigned short PRF2;       //本层第二脉冲重复频率，计数单位为1/10Hz【2】
	unsigned char Filter;      //滤波器代号【1】
	unsigned short PluseW;     //本层的脉冲宽度，计数单位为微秒【2】
	unsigned short MaxV;       //本层的最大可测速度，计数单位为厘米/秒【2】
	unsigned short MaxL;       //本层的最大可测距离，以10米为计数单位【2】
	unsigned short BinWidth;   //本层数据的库长，以1/10米为计数单位【2】
	unsigned short BinNumber;  //本层扫描的库数【2】
	unsigned short RecodeNumber;  //本层扫描径向个数【2】
	char DataForm;                /*!<本层径向中的数据排列方式：【1】
									11 单要素排列 ConZ
									12 单要素排列 UnZ
									21 按要素排列 ConZ+UnZ
									22 按要素排列 ConZ+V+W
									23 按要素排列 UnZ+V+W
									24 按要素排列 ConZ+UnZ+V+W
								    */
};

//2.1.2 雷达探测参数【154+362】=516
struct RADAROBSERVATIONPARAM_14{
	unsigned char SType;             //扫描方式【1】
	//1= 搜索模式
	//2= 单强度PPI
	//3= 单强度RHI
	//4= 强度速度PPI
	//5= 体扫
	unsigned char WeatherMode;	     // 天气状况【1】
	unsigned short SYear;            //观测记录开始时间的年（2000-）【2】
	unsigned char SMonth;           //观测记录开始时间的月（1-12）【1】
	unsigned char SDay;             //观测记录开始时间的日（1-31）【1】
	unsigned char SHour;            //观测记录开始时间的时（00-23）【1】
	unsigned char SMinute;           //观测记录开始时间的分（00-59）【1】
	unsigned char SSecond;           //观测记录开始时间的秒（00-59）【1】
	unsigned char Calibration;        //标校状态【1】
	//0=无标校
	//1=自动标校
	//2=一星期内人工标校
	//3=一月内人工标校
	//4开机标校
	//其他码不用
	unsigned char LayerNum;            //层数【1】
	LAYERPARAM_14 LayerInfo[25];     //层参数结构（各层扫描状态设置）【19*25=475】
	//注：当扫描方式是PPI或RHI时，只在第一个元素中填写，其余为0
	unsigned short RHIA;           //做RHI时的所在方位角，计数单位为1/100度，做PPI和立体扫描时为65535【2】
	short RHIL;                  //做RHI时的最低仰角，计数单位为1/100度，做其他扫描时为-32768【2】
	short RHIH;                  //做RHI时的最高仰角，计数单位为1/100度，做其他扫描时为-32768【2】
	char Reserved[24];             //保留【24】
};


//2.2 数据记录【4144】
//数据记录存放探测的回波数据，以极坐标方式有序排列，其结构如下：
struct LineDataBlock_14 
{
	short Elev;                  //仰角，计数单位1/100度 【2】
	unsigned short Az;            //方位，计数单位1/100度 【2】
	char Longtitude[16];          //经度，以字符串记录【16】
	char Latitude[16];            //纬度，以字符串记录【16】
	float Vs;                    //纵摇，计数单位【4】
	float Hs;                    //横摇，计数单位【4】
	unsigned short Course;         //航向，计数单位【2】
	unsigned short Nv;            //舰速，计数单位【2】
	unsigned char CorZ[1024]; //经过地物杂波消除的dBZ值=（CorZ-64）/2【1024】
	// xx2雷达前100个字节有效
	//数据块的具体内容由层信息的DataForm决定
	unsigned char UnZ[1024];//不经过地物杂波消除的dBZ值=（UnZ-64）/2【1024】
	char V[1024];      //速度值，计数单位为最大可测速度的1/127【1024】
	//正值表示远离雷达的速度，负值表示朝向雷达的速度
	//无回波时计-128
	unsigned char W[1024]; //谱宽值，计数单位为最大可测速度的1/512【1024】
	//无回波时计零
};

struct RADARDATAHEADER_14{
	RADARPERFORMANCEPARAM  RadarPerformanceInfo; //雷达性能参数【54】
	RADAROBSERVATIONPARAM_14  RadarObservationInfo;//雷达探测参数【154+362】=【516】
	char Reserved[24];         //备用字段【24】
};


#pragma pack()


//////////////////////////////////////////////////////////////////////////
};// end namespace xx6
};// end namespace DataFormat
};// end namespace GRS
};// end namespace CDYW

#endif



