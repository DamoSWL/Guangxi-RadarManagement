/*! \file productid.h 
	\brief 本文定义了各个产品的ID宏

	\ingroup GRSGroup
	
	\version 1.0
	\author lava_sdb
	\date 2008/09/06

	\b modify log:
	\arg \c version 1.00,lava_sdb,2008/09/06 Create
	\arg \c version 1.01,lava_sdb,2008/10/29 更新了产品ID列表
	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GRSInterface
*/



#ifndef __HEADER_ProductID_200896
#define __HEADER_ProductID_200896

#define PID_PPI			0	//!< PPI原始数据
#define PID_RHI			1	//!< RHI原始数据
#define PID_VOL			2	//!< 体扫原始数据
#define PID_REF			3	//!< 基本反射率
#define PID_VEL 		4	//!< 基本速度
#define PID_WID 		5	//!< 基本谱宽
#define PID_ET  		6	//!< 回波顶高
#define PID_EB  		7	//!< 回波底高
#define PID_CAPPI 		8	//!< 水平切割
#define PID_RCS 		9	//!< 反射率垂直切割
#define PID_MAXREF 		10	//!< 最大回波强度
#define PID_MAX			11  //!< MAX
//#define PID_R_M	12  //!< R 120 km
//#define PID_RC	13	//!< 立体显示 R
//#define PID_V_L	14
//#define PID_V_M	15
//#define PID_VC	16
//#define PID_W_L	17
//#define PID_W_M	18
#define PID_USP 		12    //!< 用户可选降水
#define PID_CATCH 		13  //!< 区域降水总量
#define PID_M 			14      //!< 中尺度气旋识别
#define PID_TVS 		15    //!< 龙卷涡旋识别
#define PID_IVAP 		16   //!< 风场反演产品
#define PID_MCAPPI 		17 //!< 反射率多层CAPPI
#define PID_ICAPPI 		18 //!< 离子CAPPI
#define PID_GFD			19    //!< 阵风锋识别
#define PID_HCUTC 		20  //!< 区域降水
#define PID_ETC  		21   //!< 回波顶高等值线
#define PID_EBC  		22   //!< 回波底高等值线
#define PID_CRC 		23    //!< 垂直最大回波等值线
#define PID_VAD 		24		//速度方位显示产品
#define PID_VIL 		25		//垂直液态水含量
#define PID_LRA 		26		//分层组合反射率平均值
#define PID_LRM 		27		//分层组合反射率最大值
#define PID_VCS 		28		//速度垂直切割
#define PID_SCS 		29		//谱宽垂直切割
//#define PID_MAXV 30
//#define PID_MAXW 31
#define PID_THI 		30  //!< 定点扫描
#define PID_RPI 		31  //!< 方位扇扫
#define PID_VWP 		32	//垂直风廓线产品
#define PID_LTA 		33  //!< 分层组合湍流平均值
#define PID_LTM 		34  //!< 分层组合湍流最大值
#define PID_SWP 		35  //!< 强天气概率
#define PID_OHP 		36  //!< 1小时降水量1
#define PID_THP 		37  //!< 三小时降水量
#define PID_STP 		38  //!<  风暴总降水量
#define PID_RZ  		39  //!< 雨强
#define PID_HPF 		40  //!< 冰雹潜势预测
#define PID_VES 		41  //!< 径向速度仰角切变
#define PID_CS  		42  //!< 组合切变
#define PID_ARD 		43  //!< 方位涡度
#define PID_RVD 		44  //!< 径向速度散度
#define PID_RHI_R 		45	//
#define PID_RHI_VW 		46
#define PID_RHI_RVW 	47
#define PID_SS 			48   //!< 风暴结构
#define PID_SCTI 		49 //!<风暴识别与追踪预报
#define PID_SEI			50  //!< 暴雨回波识别
#define PID_DDPD 		51 //!< 下击暴流识别
#define PID_UnR 		52  //!< 未滤波反射率
#define PID_RFC 		53  //!< 大气波导
#define PID_NHP 		54  //!< N小时降水量
#define PID_HI 			55   //!< 冰雹指数

#define PID_PZDR 		56 //!< 差分反射率因子
#define PID_PPDP 		57 //!< 差分相位
#define PID_PKDP 		58 //!< 差分相移因子
#define PID_PRHV 		59	//!< 零延迟相关系数

#define PID_Bound 		60 //!< 风暴轮廓追踪
#define PID_LightE 		61 //!< 雷电预警

#define PID_MVCAPPI 	62 //!< v多层CAPPI
#define PID_MWCAPPI		63 //!< w多层CAPPI
#define PID_MPCAPPI		64 //!< pi多层CAPPI
#define PID_HCL			65	//粒子产品
#define PID_PCS			66		//粒子垂直切割
#define PID_CORZ		67		//衰减订正以后的反射率因子
#define PID_VSP			70		//单体扫降雨量

#define PID_NWRTD		106		//反射率三维格点拼图（格式同RTDPZ）
#define PID_VOLRTD		107		//基于体扫的反射率三维格点（格式同RTDPZ）
#define PID_CAPPIPZ		108		//CAPPI拼图
#define PID_CRPZ		109		//CR拼图
#define PID_RTD			110		//反射率三维格点
#define PID_RTDPZ		111		//反射率三维格点拼图
#define PID_VILPZ		112		//基于三维格点的垂直累积液态含水量
#define PID_RCSPZ		113		//基于三维格点的反射率垂直切割
#define PID_STIPZ		114		//基于三维格点的风暴识别、追踪与预报
#define PID_SSPZ		115		//基于三维格点的风暴结构
#define PID_OHPPZ		116		//基于三维格点拼图的一小时降雨产品
#define PID_THPPZ		117		//基于三维格点拼图的三小时降雨产品
#define PID_STPPZ		118		//基于三维格点拼图的降水累积产品
#define PID_RZPZ		119		//基于三维格点拼图的雨强产品
#define PID_HPFPZ		120		//基于三维格点拼图的冰雹潜势预测产品
#define PID_ETPZ		121		//基于三维格点拼图的回波顶高产品
#define PID_EBPZ		122		//基于三维格点拼图的回波底高产品

#define PID_WMPE		123		//人影作业参数估算

#define PID_NWCAPPI		124		//基于单站三维格点的组网CAPPI产品
#define PID_NWCR		125		//基于单站三维格点的组网CR产品
#define PID_NWET		126		//基于单站三维格点的组网ET产品
#define PID_NWEB		127		//基于单站三维格点的组网EB产品

#define PID_WF		130		//山火识别
#define PID_SDS		131		//沙尘暴识别

#define PID_PE		132		//人工增雨
#define PID_HS		133		//人工防雹
#endif // __HEADER_ProductID_200896

