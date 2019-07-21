/************************************************************************
CIVAPFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef _IVAP_FILE_080619
#define _IVAP_FILE_080619

#include "rvw_pro.h"


#pragma pack(4)

//! IVAP产品头
typedef struct __IVAP_HEADER { 
	int m_RadNum; //!< PPI所包含的径向个数
	int m_VbNum;//!<该层PPI每个径向的速度距离库数。
	int cut ;//!<扫描层
	int AzSpace; //!<方位角间距
	int RangeNum; //!<距离上的等级数
}tagIVAPHead;

#pragma pack()


//! 本类用于解析IVAP产品
/*!
*	此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
\速度方位显示（IVAP）

* \author lava_sdb
* \ingroup group_UT
*/

class IVAPPro:
	public RVWPro
{

public:
	unsigned char *m_IVAPData;
	tagIVAPHead m_IVAPHeader;
	//unsigned char *m_pVData;//20170922修改：因产品数据部分修改，不再保存速度数据，，鼠标取值不再支持
	float *m_pIVAP;


public:
	IVAPPro(void);
	~IVAPPro(void);

	int OpenBuff( char *pBuff, int iLen);
	void Close();

};

#endif