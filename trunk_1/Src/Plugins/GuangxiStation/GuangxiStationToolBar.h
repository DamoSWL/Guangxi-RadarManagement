/* \file GuangxiStationToolBar.h
	\brief GuangxiStation工具条定义
	\ingroup GuangxiStationGroup
	
	本文件定义了GuangxiStation工具条。

	\version 1.0
	\author 
	\date 20171127

	\b 修改记录：
	\arg \c version 1.0， <author>，20171127 创建

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GuangxiStationGroup
*/
#ifndef __HEADER_MY_TB_20171127
#define __HEADER_MY_TB_20171127

//! 工具栏定义
const char g_ToolBarXML[] ="<?xml version=\"1.0\" encoding=\"gb2312\" ?>"
	"<Plugin name=\"wxPlugin\">"
	"<ToolBar>"
	"<Button name=\"测试1\" pic=\"Pic1.bmp\" id=\"21\" tooltip=\"测试1\" separator=\"0\" />"
	"<Button separator=\"1\" />"
	"<Button name=\"测试2\" pic=\"Pic2.bmp\" id=\"22\" tooltip=\"测试2\" separator=\"0\" />"
	"</ToolBar>"
	"</Plugin>";

static const PL_TB_ID g_guid_toolbar = { 0x1564DBFD, 0xA856, 0x4ECA, { 0xB9, 0x8E, 0xFB, 0x63, 0xD0, 0xD2, 0x88, 0x52 } };

#endif


