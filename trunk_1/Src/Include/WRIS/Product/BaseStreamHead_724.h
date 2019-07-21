/*! \file basestreamhead_xx2.h 
	\brief 本文件记录了xx2雷达基数据l流格式相关结构

	\ingroup GRSGroup
	
	\version 1.0
	\author lava_sdb
	\date 2008/09/06

	\b modify log:
	\arg \c version 1.00,lava_sdb,2008/09/06 创建
	\arg \c version 1.01,lava_sdb,2008/10/29  更新了命名空间 

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GRSInterface
*/


#ifndef _HEADER_XX2BaseDataHeader_20080906
#define _HEADER_XX2BaseDataHeader_20080906

#include "basestreamhead_14.h"

using namespace CDYW::GRS::DataFormat::SXX6;

namespace CDYW
{
namespace GRS
{
namespace DataFormat
{
namespace CXX2
{
//////////////////////////////////////////////////////////////////////////
#pragma pack(1)		// one byte alignment

struct RADARDATA_XX2
{
	unsigned char dataType; // 数据类型:0，扫描数据头； 1，径向数据

	union{
		RADARDATAHEADER  radarHeader; // 雷达数据头【140】
		LineDataBlock  radialData[5];  // 径向数据 【4149】
	};
};

#pragma pack()


//////////////////////////////////////////////////////////////////////////
};// end namespace XX2
};// end namespace DataFormat
};// end namespace GRS
};// end namespace CDYW

#endif



