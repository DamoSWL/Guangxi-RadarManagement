/* \file GuangXiPlug.cpp
	\brief GuangXi 插件实现文件
	\ingroup GuangXiGroup
	
	本文件定义了GuangXi插件的实体。

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

// PluginTest.cpp : Defines the entry point for the DLL application.
//

#ifdef WX_PRECOMP
	#include "wx_pch.h"
#endif

#include "GuangXiMain.h"

static const PL_PID g_guid = g_PL_GuangXi;

namespace GuangXi
{
	//! 插件测试类实例化
	PluginRegistrant< GuangXiPlug > plugin( 
			g_Name_GuangXi, 
			g_PL_GuangXi 
	);
}

