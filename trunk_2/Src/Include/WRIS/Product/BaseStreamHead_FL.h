/*! \file FLBaseDataHead.h 
	\brief 
	\ingroup GRSGroup
	
	\version 1.0
	\author lava_sdb
	\date 2008/09/06

	\b modify log:
	\arg \c version 1.00,lava_sdb,2008/09/06 Create
	\arg \c version 1.01,lava_sdb,2008/10/29 增加了命名空间

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GRSInterface
*/

//实时数据战址段
#ifndef _HEADER_FLBaseDataHead_20080906
#define _HEADER_FLBaseDataHead_20080906

#include  "radialhead.h"

namespace CDYW
{
namespace GRS
{
namespace DataFormat
{
namespace XFL
{
#pragma pack(1)
/*!
	FL雷达的径向基数据流格式
*/
typedef struct _BASE_DATA_STREAM_FL
{
	long FOOS;			//!< 数据包头标记
	unsigned long id;	//!< 包的ID
	BASE_DATA_HEAD header;			//!< 雷达基数据流的径向头
	unsigned char ref[ 512 * 8 ];	//!< 反射率数据
	unsigned char vel[ 512 * 8 ];	//!< 速度数据
	unsigned char wid[ 512 * 8 ];	//!< 谱宽数据
	long FOOE;			//!< 数据包尾标记
}tagBaseDataStreamFL;

#pragma pack()
//////////////////////////////////////////////////////////////////////////
};// end namespace FLX
};// end namespace DataFormat
}; // end namespace GRS
}; // end namespace CDYW

#endif

