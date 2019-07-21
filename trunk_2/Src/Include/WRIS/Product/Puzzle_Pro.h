/************************************************************************
CSingleFile
  
  This File using phrase source file                                                                  
************************************************************************/  

#ifndef __SOURCE_PUZZLE_FILE_20160729
#define __SOURCE_PUZZLE_FILE_20160729 

//#include "rvw_pro.h "
#include "../../../include/WRIS/product/STIPuzzle_Pro.h"
#include <WRIS/Product/PuzzleHead.h>
using namespace CDYW::GRS::DataFormat;
using namespace CDYW::GRS::DataFormat::XFL;

#include <string>
using namespace std;

#define minHeight 0//RTD默认最低切割高度，m
#define maxHeight 25000//RTD限制最大切割高度，m




enum PROJECTION { WebMer=1, WGS84=2, EqualDis=3 };

//! 拼图数据解析类
/*!
 * 此类用于解析RCS数据及其类似格式的数据
  
*/
class PuzzlePro:
	public RVWPro
{
public:
	tagCRHead *m_CRPZHeader;
	tagCAPPIHead *m_CAPPIPZHeader;
	tagVILPZHead *m_VILPZHeader;
	unsigned char *m_pVData;
	tagOHPPZHead *m_OHPPZHeader;
	tagETEBPZHead *m_ETEBPZHeader;
public:
	PuzzlePro(void);
	~PuzzlePro(void);

	int Open(string szPath);	
	int OpenBuff( char *pBuff, int iLen);
};

#endif
