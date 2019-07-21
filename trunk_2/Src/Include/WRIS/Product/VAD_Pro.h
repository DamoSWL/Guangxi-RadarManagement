/************************************************************************
CVADFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef _VAD_FILE_080619
#define _VAD_FILE_080619

#include "rvw_pro.h"


#pragma pack(4)

//! VAD产品头
typedef struct __VAD_HEADER { 
	float YangJiao; //!< 速度方位显示所分析的仰角，度
	float HorRan;   //!< 速度方位显示所分析的水平距离,km
	float SlatRan;   //!< 速度方位显示所分析的径向距离，km
	float VerRan;   //!< 速度方位显示所分析的垂直距离，km
	float a0;       //!< 拟合速度曲线的对称性，m/s
	float pVh;      //!< 分析区域的平均风速，m/s
	float pWindD;   //!< 分析区域的平均风向，弧度
	float RMS;      //!< 拟合速度的均方根误差，m/s
	float iRadNum; //!< PPI所包含的径向个数
	float minV;     //!< 最小速度 m/s
	float maxV;     //!< 最大速度 m/s
}tagVADHead;

#pragma pack()


//! 本类用于解析VAD产品
/*!
*	此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
\速度方位显示（VAD）

* \author lava_sdb
* \ingroup group_UT
*/

class VADPro:
	public RVWPro
{

public:
	tagVADHead m_VADHeader;

	float *m_pVADData;
	int m_iNumOfRadial ;
	float *m_pDataR;
	float *m_pDataV;
	float *m_pVAD;


public:
	VADPro(void);
	~VADPro(void);

	int Open(string szPath);
	int OpenBuff( char *pBuff, int iLen);
	void Close();

};

#endif