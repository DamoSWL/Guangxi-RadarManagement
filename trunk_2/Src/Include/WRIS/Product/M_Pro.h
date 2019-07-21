/************************************************************************
CMFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef _M_FILE_120312
#define _M_FILE_120312

#include "rvw_pro.h"
#include <vector>
using namespace std;
//! M产品头
typedef struct _MHEAD {
	int TPV; //二维特征中型矢量的最小数目
	int TFM; //二维特征高度值上限
	int Num; //所识别出的中气旋及三维相关切变的个数
}tagMHead;


//!中尺度气旋或三维相关切变属性 attribute
typedef struct _DEM3ATTRI
{
	int STORMID;//风暴单体标识号:离它最近的风暴单体标识号
	int FEATURE;//特征：中气旋或三维相关切变   0,表示中气旋；1,表示三维相关切变。
	int Dem2FeNum;//二维特征量个数
	float AZ;//雷达到特征中心的方位
	float RAN;//雷达到特征中心的距离
	float X;//X坐标
	float Y;//Y坐标
	float BASE;//特征底的高度
	float TOP;//特征顶的高度
	float RAD;//沿径向方向的尺度
	float AZDIA;//沿方位角方向的尺度
	float HighSARmax;//最大切变的高度
	float AZSARmax;//最大切变方位
	float RsSARmax;//最大切变距雷达中心的距离
}tagDem3Attri;
/*
tagFileHeader + tagMHead + 中尺度气旋0 + 中尺度气旋1 + ......；
*/

//! 基数据及其类似数据解析类
/*!
 *	此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
	\中尺度气旋（M）

 * \author 樊兆妮
 * \ingroup group_UT
*/
class CMPro:
	public RVWPro
{

public:
	tagMHead m_MHeader;
	vector<bool> m_bVisible;

	unsigned char *m_pMData;
	unsigned int m_DataLen ;

public:
	CMPro(void);
	~CMPro(void);

	int Open(string szPath);
	int OpenBuff( char *pBuff, int iLen);
	//!获得中气旋或三维切变的个数
	unsigned int GetMNum();
	//!获得指定的中气旋或三维切变的指针
	tagDem3Attri *GetDem3Attri(int iIndex);
};


#endif