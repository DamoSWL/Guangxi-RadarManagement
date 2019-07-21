/* \file GuangXiPanel.h
	\brief GuangXi普通面板定义
	\ingroup GuangXiGroup
	
	本文件定义了GuangXiPanel面板类。

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

#ifndef __HEADER_GuangXi_Panel_20171117
#define __HEADER_GuangXi_Panel_20171117

#include "GuangXi_gui.h"

using namespace CDYW::PluginSystem;

// {CC44A43F-D6F5-46d8-8F73-19A7B8AEED51}
static const PL_GUID g_guid_panel = { 0xB1DB30B7, 0xA633, 0x483D, { 0x9E, 0x3B, 0xC1, 0x9D, 0x2C, 0x06, 0x59, 0x6E } };

///////////////////////////////////////////////////////////////////////////////
/// Class GuangXiPanel
///////////////////////////////////////////////////////////////////////////////
/*! \class GuangXiPanel
	\brief GuangXi参数设置对话框类
	\ingroup GuangXiGroup
*/
class GuangXiPanel : 
    public GuangXiPackPanel ,
	public IParaPageHelper< GuangXiPanel, wxWindow >
{
public:
	GuangXiPanel( wxWindow* parent);
	~GuangXiPanel();

	/* 重载自IDlgParapage*/
	void OnApply();
};

#endif
