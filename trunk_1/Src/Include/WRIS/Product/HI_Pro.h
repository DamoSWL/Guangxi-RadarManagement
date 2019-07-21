/************************************************************************
CHIFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef __HI_FILE_120606 
#define __HI_FILE_120606  

#include "rvw_pro.h"
#include "time.h"
#include <vector>
#include <string>
using namespace std;

#pragma pack(4)
//!冰雹指数产品文件头
typedef struct _HIHEAD
{
	int StormNum;   //!<一个体扫内风暴体的个数
	float Area;//风暴分量面积阈值
	float H0; //环境融化层的高度
	float Hm20; //-20℃环境温度的地面以上高度

}tagHIHead;

typedef struct _SCFEATURE
{
	int StormCellIndex;//风暴体编号
	float Az;//方位
	float Range;//距离
	float XSC;     ///风暴体质量权重中心所在的X坐标，km
	float YSC;     ///风暴体质量权重中心所在的Y坐标，km
	int POH;//降冰雹的概率
	float SHI;//强冰雹指数
	int POSH;//强冰雹概率
	float MEHS;//冰雹最大尺寸预报

	float H_POH;//计算冰雹高度
	float DH_POH;//计算冰雹高度差


}tagSCFEATURE;
#pragma pack()

//! HI 产品解析类
/*!
	本类用于解析 HI 产品

* \author 樊兆妮
* \ingroup group_UT
*/
class HIPro:
	public RVWPro
{
public:
	HIPro(void);
	~HIPro(void);

	unsigned char *m_pDataT;
	vector<bool> m_bVisible;
	
	int Open(string szPath );	
	int OpenBuff( char *pBuff,int iLen);

	inline float Deg2Float(short data)
	{
		char *pData = (char *)&data;
		return (float)(pData[0] *0.02197265625 + pData[1] * 2.8125);
	}


	//! 获取风暴体个数
	unsigned int GetStormCount( );

	
	//! 获取指定风暴体的特征量
	tagSCFEATURE *GetStormFeature(unsigned int iIndex);
	tagHIHead m_HIHeader;

};


#endif
