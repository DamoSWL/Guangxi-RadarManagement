/* \file GuangXiToolBar.h
	\brief GuangXi工具条定义
	\ingroup GuangXiGroup
	
	本文件定义了GuangXi工具条。

	\version 1.0
	\author 
	\date 20171117

	\b 修改记录：
	\arg \c version 1.0， <author>，20171117 创建

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GuangXiGroup
*/
#ifndef __HEADER_MY_TB_20171117
#define __HEADER_MY_TB_20171117

//! 工具栏定义
const char g_ToolBarXML[] ="<?xml version=\"1.0\" encoding=\"gb2312\" ?>"
	"<Plugin name=\"wxPlugin\">"
	"<ToolBar>"
	"<Button name=\"测试1\" pic=\"Pic1.bmp\" id=\"21\" tooltip=\"测试1\" separator=\"0\" />"
	"<Button separator=\"1\" />"
	"<Button name=\"测试2\" pic=\"Pic2.bmp\" id=\"22\" tooltip=\"测试2\" separator=\"0\" />"
	"</ToolBar>"
	"</Plugin>";

static const PL_TB_ID g_guid_toolbar = { 0xA4956E4A, 0x46F9, 0x49E6, { 0xBA, 0xA5, 0xE9, 0x2F, 0xBB, 0xAC, 0xC5, 0x4A } };

#endif


