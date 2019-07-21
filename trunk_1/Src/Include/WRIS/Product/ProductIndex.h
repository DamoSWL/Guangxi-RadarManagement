/*! \file ProductIndex.h 
	\brief 产品参数索引文件

	\ingroup GRSGroup
	
	\version 1.0
	\author lava_sdb
	\date 2008/09/06

	\b modify log:
	\arg \c version 1.0,lava_sdb,2008/09/06 Create

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GRSInterface
*/



#ifndef __HEADER_ProductIndex_200896
#define __HEADER_ProductIndex_200896

#define SIZE_PRO_NAME 100

namespace CDYW
{
namespace GRS
{
//////////////////////////////////////////////////////////////////////////

#pragma pack(1)

/*! 产品索引文件索引结构体
*/
typedef struct _INDEX_PRODUCT
{
	char name[100];	//!< 产品名称
	float param0;   //!< 产品参数0
	float param1;   //!< 产品参数1
	float param2;   //!< 产品参数2
	float param3;   //!< 产品参数3
	float param4;   //!< 产品参数4
	float param5;   //!< 产品参数5
}tagProIndex;

#pragma pack()


//////////////////////////////////////////////////////////////////////////
}; // end namespace GRS
}; // end namespace CDYW

#endif // __HEADER_ProductIndex_200896


