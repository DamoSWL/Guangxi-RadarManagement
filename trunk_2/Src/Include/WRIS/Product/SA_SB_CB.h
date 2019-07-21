
#ifndef RADARFORMAT_SA_SB_CB_HEADER
#define RADARFORMAT_SA_SB_CB_HEADER

//#include "RadialHead.h"
// #include <map>

#define SA_SB_FORMAT
//#define CB_FORMAT
#ifndef CB_FORMAT
     //***  SA/SB      ***
	#define iReflectivityGateSize 1000	//	----	反射率库长；
	#define iDopplerGateSize	250	//	----	多普勒库长；
	#define iReflectivityGates	460	//	----	反射率库数；
	#define iDopplerGates	920	//	----	多普勒库数；
#else
	// ***		如果对CB转换，则打开下面定义屏蔽上面定义	***		<---
	#define iReflectivityGateSize 500	//	----	反射率库长；
	#define iDopplerGateSize	125	//	----	多普勒库长；
	#define iReflectivityGates	800	//	----	反射率库数；
	#define iDopplerGates	1600 	//	----	多普勒库数；
#endif

#define MaxCuts  20   //最大仰角层数
#define MaxRads  360   //每层仰角上的方位数,每度保留一个径向
#define CODE_INVALID 0   //编码值中的特殊标记,表示无有效观测数据
#define CODE_RANFOLD 1   //编码值中的特殊标记,表示有距离模糊

#define VALUE_INVALID -999.  //实际值中的特殊标记,表示无有效观测数据
#define VALUE_RANFOLD  999.  //实际值中的特殊标记,表示有距离模糊

#define RES_POINT_FIVE 2  //速度精度类型,代表的精度为0.5 M/S
#define RES_ONE_POINT  4  //速度精度类型,代表的精度为1.0 M/S

#define VOL_BEG 3    //体扫开始状态标志
#define VOL_END 4    //体扫结束状态标志
#define ELV_BEG 0    //仰角开始状态标志
#define ELV_END 2    //仰角结束状态标志

#define RADIAN  (M_PI/180.)


/*
说明：

1．数据的存储方式

每个体扫存储为一个单独的文件

2．数据的排列方式

按照径向数据的方式顺序排列，对于CINRAD SA/SB雷达，体扫数据排列自低仰角开始到高仰角结束。

3．径向数据的长度

径向数据的长度固定，为2432字节。

4．距离库长和库数

反射率距离库长为1000米，最大距离库数为460；

速度和谱宽距离库长为250米，最大距离库数为920。
*/
namespace CDYW {
	namespace RadarDataFormat
	{
		namespace SA_SB_CB
		{
		/////////////////////////////////////////////////////////////////////////
			/************************************************************************/
			/*			数据类：分别定义FL数据的一个径向、一个CUT、一个体扫         */
			/************************************************************************/
		#pragma pack ( push, 1)

// 			typedef struct  _RADIAL_BASEDATA
// 			{
// 				BASE_DATA_HEAD RadialHeader;	//	----	径向数据头；
// 				unsigned char b_RData[iReflectivityGates];	//	----	体扫反射率数据数组；
// 				unsigned char b_VData[iDopplerGates];	//	----	体扫速度数据数组；
// 				unsigned char b_WData[iDopplerGates];	//	----	体扫谱宽数据数组；
// 			}tagOneRadial;//	----	FL一个径向数据格式
// 			//////////////////////////////////////////////////////////////////////////
// 
// 			typedef struct  _CUT_BASEDATA
// 			{
// 				tagOneRadial RadialinoneCUT[MaxRads];	//	----	CUT数据格式；
// 			}tagOneCUT;//	----	FL一个CUT数据格式
// 			////////////////////////////////////////////////////////////////////////// 
// 
// 			typedef struct  _VCP_BASEDATA
// 			{
// 				tagOneCUT CUTinoneVCP[MaxCuts];	//	----	VCP数据格式；
// 			}tagOneVCP;//	----	FL一个体扫数据格式

			typedef struct _SAHeader
			{
				char Unused1[14];//1-14 保留
				//unsigned short int MessageType;//15~16 记录数据类型，具体说明见附表
				unsigned char MessageType;//15~16 记录数据类型，具体说明见附表
				unsigned char Channel;
				char Unused2[12];//17-28 保留

				unsigned int RadialCollectionTime;//29-32 径向资料采集的GMT时间(毫秒)
				unsigned short RadialCollectionDate;//33-34 径向资料采集的日期,以1/1/1970以来的Julian日期表示
				unsigned short int UnambiguousRange;//35-36 不模糊距离,单位:0.1Km

				unsigned short int AzimuthAngle;//37-38 方位角,实际值=(value>>3)*0.043945 (度)
				unsigned short int DataNumber;//39-40 径向数据序号
				unsigned short int DataStatus;//41-42 径向数据状态

				unsigned short int ElevationAngle;//43-44 仰角,实际值=(value>>3)*0.043945 (度)
				unsigned short int ElevationNumber;// 45-46 体扫内的仰角编号

				//////////////////////////////////////////////////////////////////////////

				short int FirstGateRangeOfRef;//47-48 第一个强度库的距离(米)
				short int FirstGateRangeOfDoppler;//49-50 第一个速度/谱宽库的距离(米)
				unsigned short int ReflectivityGateSize;//51-52 强度库长(米)
				unsigned short int DopplerGateSize;//53-54 速度/谱宽库长(米)
				unsigned short int ReflectivityGates;//55-56 强度库数
				unsigned short int DopplerGates;//57-58 速度/谱宽库数

				//////////////////////////////////////////////////////////////////////////

				unsigned short CutSectorNumber;	//	----	CUT号；
				unsigned int CalibrationConst ;	//59-64 保留	标定常数；
				unsigned short int RefPointer;//65-66 从雷达数据头到强度数据开始的字节数
				unsigned short int VelPointer;//67-68 从雷达数据头到速度数据开始的字节数
				unsigned short int SWPointer;//69-70 从雷达数据头到谱宽数据开始的字节数
				short int VelResolution;//71-72 速度分辨率:2=0.5m/s;4=1.0m/s

				unsigned short int VCP;//73-74 体扫VCP模式 (11,21,31,32)
				//11：降水模式，16层仰角
				//21：降水模式，14层仰角
				//31：晴空模式，8层仰角
				//32：晴空模式，7层仰角
				char Unused4[8];//75-88 保留
				unsigned short PtrtoRef;
				unsigned short PtrtoVel;
				unsigned short PtrtoWth;
				unsigned short int NyquistVelocity;//89-90 Nyquist速率(0.01m/s)不模糊速度
				unsigned short temp46;	//	----	保留；
				unsigned short temp47;
				unsigned short temp48;
				unsigned short CircleTotal;	//	----	仰角数；
				unsigned char Unused5[30];//91-128 保留
				//unsigned char Data[ iReflectivityGates+ iDopplerGates + iDopplerGates];//129-2428字节数据；或129-4128字节基数据；
				unsigned char *Data;//129-2428字节数据；或129-4128字节基数据；
				char Unused6[4];//2429-2432 保留 ；或 4129-4132 保留；

			}tagSAHeader,tagSBHeader;

		#pragma pack( pop )
			/************************************************************************/
			/*	  函数：   分别用于取得SA_SB/CB日期时间、速度、仰角、方位角，		*/
			/*							以及得到FL加权方位角				     	*/
			/************************************************************************/

		};//end namespace SA_SB
	};// end namespace RadarDataFormat
}; // end namespace CDYW


#endif