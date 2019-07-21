/*! \file basestreamhead.h 
	\brief 本文件记录了雷达基数据流格式相关结构

	\ingroup GRSGroup
	
	\version 1.01
	\author lava_sdb
	\date 2010/03/11

	\b modify log:
	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GRSInterface
*/


#ifndef _HEADER_BaseDataStreamHeader_20100311
#define _HEADER_BaseDataStreamHeader_20100311

namespace CDYW
{
namespace GRS
{
namespace DataFormat
{
//////////////////////////////////////////////////////////////////////////
#pragma pack(1)		// one byte alignment

/*! \brief 雷达Cut扫描头

	本结构体作为XX6和FL的中间结构体，用于XX6和FL的CUT头的互换
*/
typedef struct _SCANCUTINFO  //【154】
{
	unsigned short int radialNum;	//!< 本层扫描的包含径向数。（360/径向角度分辨率）【2】

	unsigned short int wavForm;   //!< 当前Cut扫描形式（批处理/HCD/LCD/LCS）
	unsigned char ambiguousp;   /*! 速度退模糊  
								- 0.无退模糊处理；
								- 1.软件退模糊；
								- 2.双PRF退模糊；
								- 3.批式退模糊；
								- 4.批式加软件退模糊；
								- 5.双PRF退模糊；
								- 6.双PRF+软件退模糊
								*/
	unsigned short prf1;     //!< 本层第一脉冲重复频率，计数单位为1/10Hz【2】
	unsigned short prf2;     //!< 本层第二脉冲重复频率，计数单位为1/10Hz【2】
	unsigned char filter;    //!< 滤波器代号【1】
	unsigned short pluseW;    //!< 本层的脉冲宽度，计数单位为微秒【2】
	unsigned short maxV;      //!< 本层向的最大可测速度，计数单位为厘米/秒【2】
	unsigned short maxL;      //!< 本层向的最大可测距离，以km为计数单位【2】


	unsigned short int usRefBinWidth;  //!< 本扫描反射率数据库长，以10米为计数单位【2】
	unsigned short int usDopBinWidth;  //!< 本扫描多普勒数据库长，以10米为计数单位【2】
	unsigned short int usRefBinNum;	   //!< 本扫描每径向反射率的库数【2】
	unsigned short int usDopBinNum;    //!< 本扫描每径向多普勒的库数【2】
	unsigned short int cutNum;	// 扫描包含的Cut个数
	
	unsigned short int elevIndex;	//!< 当前Cut在整个扫描中的序号
	unsigned short int cutIndex;	//!< 当前Cut的唯一序号

	unsigned short int weatherMode;	//!< 当前扫描的工作模式

	unsigned short int sType;	//!< 雷达扫描模式（RHI、PPI、VOL）
	unsigned short int dataForm; //!< 数据排列方式

	int antennaG;	//!< 天线增益

	unsigned short beamH;         //!< 法线方向上的波束宽度，以1/100度为计数单位【2】
	unsigned short beamL;         //!< 切线方向上的波束宽度，以1/100读为计数单位【2】
	unsigned char polarizations;  /*! 极化状况【1】
									 - 0=水平
									 - 1=垂直
									 - 2=双线偏振
									 - 3=其他待定
									 */
	unsigned short sideLobe;         //!<第一旁瓣电平，以0.01dB为计数单位【2】
	int power;                     //!< 雷达脉冲峰值功率，以瓦为单位【4】
	int waveLength;                //!< 波长，以0.1mm为计数单位【4】
	unsigned short lineA;          //!< 接收机动态范围，以0.01dB为计数单位【2】
	short int lineMinPower;       //!< 接收机最小可测功率，计数单位为0.01dBm【2】
	char clutterP;                 //!< 地杂波消除阈值，计数单位为dB【1】
	char clutterS;                 //!< 海杂波消除阈值，计数单位为dB【1】
	unsigned char velocityP;		/*! 速度处理方式【1】
									- 0=无速度处理
									- 1=PPP
									- 2=FFT
									*/
	unsigned char filterP;			/*! 地杂波消除方式【1】
									- 0=无地杂波消除
									- 1=地杂波扣除法
									- 2=滤波器处理
									- 3=地杂波+滤波器处理
									- 4=谱分析处理
									- 5=其他处理法
									*/
	unsigned char filterS;            //!< 海杂波消除方式【1】
	//待定
	unsigned char noiseT;            //!< 噪声消除阈值（0-255）【1】
	unsigned char intensityR;          /*! 强度估算距离订正方式【1】
									   - 1，对数
									   - 2, 线性
									   */

	unsigned short int azi;   //!< 做RHI时的所在方位角，计数单位为1/100度，做PPI和立体扫描时为65535【2】
	short int ele;		//!< 仰角，计数单位1/100度 
	short int rhiL;     //!< 做RHI时的最低仰角，计数单位为1/100度，做PPI和立体扫描时为对应的仰角【2】
	short int rhiH;     //!< 做RHI时的最高仰角，计数单位为1/100度，做其他扫描时为-32768【2】
	char Reserved[24];  //!< 保留【24】
}tagScanCutInfo;

#pragma pack()


//////////////////////////////////////////////////////////////////////////
};// end namespace DataFormat
};// end namespace GRS
};// end namespace CDYW

#endif



