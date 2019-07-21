/* \file GuangxiStationPlug.cpp
	\brief GuangxiStation 插件实现文件
	\ingroup GuangxiStationGroup
	
	本文件定义了GuangxiStation插件的实体。

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

// PluginTest.cpp : Defines the entry point for the DLL application.
//

#ifdef WX_PRECOMP
	#include "wx_pch.h"
#endif

#include "GuangxiStationMain.h"

static const PL_PID g_guid = g_PL_GuangxiStation;

namespace GuangxiStation
{
	//! 插件测试类实例化
	PluginRegistrant< GuangxiStationPlug > plugin( 
			g_Name_GuangxiStation, 
			g_PL_GuangxiStation 
	);
}

