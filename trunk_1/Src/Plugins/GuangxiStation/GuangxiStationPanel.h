/* \file GuangxiStationPanel.h
	\brief GuangxiStation普通面板定义
	\ingroup GuangxiStationGroup
	
	本文件定义了GuangxiStationPanel面板类。

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

#ifndef __HEADER_GuangxiStation_Panel_20171127
#define __HEADER_GuangxiStation_Panel_20171127

#include "GuangxiStation_gui.h"

using namespace CDYW::PluginSystem;

// {CC44A43F-D6F5-46d8-8F73-19A7B8AEED51}
static const PL_GUID g_guid_panel = { 0x9CE4032B, 0x637C, 0x4DEA, { 0xB4, 0x40, 0x2F, 0xBE, 0xF3, 0x79, 0xEE, 0xDD } };

///////////////////////////////////////////////////////////////////////////////
/// Class GuangxiStationPanel
///////////////////////////////////////////////////////////////////////////////
/*! \class GuangxiStationPanel
	\brief GuangxiStation参数设置对话框类
	\ingroup GuangxiStationGroup
*/
class GuangxiStationPanel : 
    public GuangxiStationPackPanel ,
	public IParaPageHelper< GuangxiStationPanel, wxWindow >
{
public:
	GuangxiStationPanel( wxWindow* parent);
	~GuangxiStationPanel();

	/* 重载自IDlgParapage*/
	void OnApply();
};

#endif
