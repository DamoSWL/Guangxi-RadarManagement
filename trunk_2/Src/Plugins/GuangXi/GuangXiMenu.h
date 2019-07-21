/* \file WRSOCcontrtolMenu.h
	\brief WRSOCcontrtol菜单定义
	\ingroup WRSOCcontrtolGroup
	
	本文件定义了WRSOCcontrtol菜单内容。

	\version 1.0
	\author 
	\date 20170904

	\b 修改记录：
	\arg \c version 1.0， <author>，20170904 创建

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup WRSOCcontrtolGroup
*/


#ifndef __HEADER_GuangXi_Menu_20171117
#define __HEADER_GuangXi_Menu_20171117

/*
\b A_Menu 表示主菜单，只能有一个，由parGUID标识的主菜单已经存在时，加入，不存在则创建
\b B_Menu 表示子菜单，可以有多个，子菜单下还可以添加 LeafMenu 菜单项
\b LeafMenu 菜单项

\arg \c name 菜单项的名称
\arg \c id 菜单项的id，此id在插件中是唯一的，插件根据此id判断UI事件触发源
\arg \c tooltip 菜单项显示的 tooltip
\arg \c separator 菜单项的类型
\arg \c order 菜单项加载位置

- -1 分隔符类型菜单项
- 0 普通菜单项
- 1 check 类型菜单项
- 2 RADIO 类型菜单项

快捷键XML描述格式：(&amp;T)
*/
//! 菜单定义
const char g_MenuXML[] ="<?xml version=\"1.0\" encoding=\"gb2312\" ?>"
"<Plugin name=\"wxPlugin\">"

	"<LeafMenu name=\"参数设置\" id=\"11\" separator=\"0\" order=\"4\" />"
	"<LeafMenu name=\"-\" separator=\"-1\" order=\"3\" />"

"</Plugin>";

static const PL_MENU_ID g_guid_menu = { 0x32A822ED, 0x4C1F, 0x4AC5, { 0x92, 0x70, 0xA0, 0xD0, 0xF8, 0x6B, 0x8B, 0x9D } };

#endif



