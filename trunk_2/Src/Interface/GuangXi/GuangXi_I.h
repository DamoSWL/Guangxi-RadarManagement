/* \file IGuangXi IGuangXi.h
	\brief GuangXi对外接口定义文件
	\ingroup PluginGroup
	
	本接口为其它插件提供...功能。

	\version 1.0
	\author 
	\date 20171117

	\b 修改记录：
	\arg \c version 1.0， <author>，20171117 创建

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GuangXiInterface
*/
#ifndef _INTERFACE_GuangXi_20171117
#define _INTERFACE_GuangXi_20171117

//! 插件的ID
static const PL_GUID g_PL_GuangXi = { 0x16C4B2CF, 0x17BF, 0x4ED7, { 0x85, 0xAF, 0x68, 0x1E, 0x3B, 0x4D, 0x04, 0x11 } };

//! 插件的功能接口ID 
static const PL_GUID g_I_GuangXi = { 0xBCB62C5E, 0xC685, 0x401C, { 0x88, 0xD8, 0x41, 0xB1, 0x86, 0x5E, 0x47, 0x6F } };

//! GuangXi插件d的中文名称
static const char *g_Name_GuangXi = "GuangXi";

/*! \class IGuangXi
	\brief GuangXi接口类

	\ingroup GuangXiInterface
*/
class IGuangXi
{
public:

};

#endif

