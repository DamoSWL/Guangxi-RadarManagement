/************************************************************************
CSingleFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __SOURCE_RCS_FILE_090210
#define __SOURCE_RCS_FILE_090210 

#include "rvw_pro.h "

using namespace CDYW::GRS::DataFormat;
using namespace CDYW::GRS::DataFormat::XFL;

#include <string>
using namespace std;
#pragma pack(4)

//! RCS产品头
typedef struct __RCS_HEADER{
	int height;		//!< RCS 产品的高度，单位 RangeBin
	int width;		//!< RCS 产品的宽度，单位 RangeBin
	float sDis;		//!< 起始距离，单位 米
	float sAzi;		//!< 起始方位，单位 度
	float eDis;		//!< 终止距离
	float eAzi;     //!< 终止方位
	float ele;		
}tagVCut;
#pragma pack()

//! RCS数据解析类
/*!
 * 此类用于解析RCS数据及其类似格式的数据
  
 * \author lava_sdb
 * \ingroup group_UT
*/
class RCSPro:
	public RVWPro
{
public:
	__RCS_HEADER m_RCSHeader;
	unsigned char *m_pVData;
public:
	RCSPro(void);
	~RCSPro(void);

	int Open(string szPath);	
	int OpenBuff( char *pBuff, int iLen);

};

#endif
