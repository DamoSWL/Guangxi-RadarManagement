/* \file IGuangxiStation IGuangxiStation.h
	\brief GuangxiStation对外接口定义文件
	\ingroup PluginGroup
	
	本接口为其它插件提供...功能。

	\version 1.0
	\author 
	\date 20171127

	\b 修改记录：
	\arg \c version 1.0， <author>，20171127 创建

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GuangxiStationInterface
*/
#ifndef _INTERFACE_GuangxiStation_20171127
#define _INTERFACE_GuangxiStation_20171127

//! 插件的ID
static const PL_GUID g_PL_GuangxiStation = { 0x16BD8B65, 0x8D31, 0x4EB0, { 0xB2, 0xA8, 0x5C, 0x97, 0x5D, 0xCF, 0x36, 0xE5 } };

//! 插件的功能接口ID 
static const PL_GUID g_I_GuangxiStation = { 0x58676E21, 0x0144, 0x47D7, { 0xA3, 0x9E, 0x49, 0x4D, 0xF5, 0x80, 0x20, 0xEE } };

//! GuangxiStation插件d的中文名称
static const char *g_Name_GuangxiStation = "GuangxiStation";

/*! \class IGuangxiStation
	\brief GuangxiStation接口类

	\ingroup GuangxiStationInterface
*/
class IGuangxiStation
{
public:

};

#endif

