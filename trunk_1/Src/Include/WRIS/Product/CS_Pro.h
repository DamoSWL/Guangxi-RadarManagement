/************************************************************************
CRACSFile

This File using phrase source file                                                                  
************************************************************************/  

#ifndef _RACS_FILE_080617
#define _RACS_FILE_080617

#include "rvw_pro.h"


//! RACS产品头
typedef struct _RACSHEAD {
	union{
		float fEle;		//大船历史数据为浮点数
		int   Ele;     //!< 计算径向散度的仰角编号
	};
	float LenOfWin;  //!< 计算径向散度的窗口长度，单位km
	float AziOfWin;  //!< 计算方位涡度的窗口长度，单位度
	int   CalNum;    //!< 一个径向上组合切变的个数
	float MaxRACS;	 //!< 该PPI最大的径向散度
	float MaxAzi;    //!< 最大组合切变所在的方位角，单位度
	float MaxRange;  //!< 最大组合切变所在的斜距，单位km
}tagRACSHead;


//! 本类用于解析RACS产品
/*!
*	此类用于解析及数据及其类似格式的数据，目前支持的数据类型有：
	\径向速度的组合切变（RACS）

	* \author lava_sdb
	* \ingroup group_UT
*/
class CSPro:
	public RVWPro
{

public:
	tagRACSHead m_RACSHeader;

	float *m_pRACSData;
	int iLen ;

public:
	CSPro(void);
	~CSPro(void);

	int Open(string szPath);
	int OpenBuff( char *pBuff, int iLen);
	void Close();

};




#endif