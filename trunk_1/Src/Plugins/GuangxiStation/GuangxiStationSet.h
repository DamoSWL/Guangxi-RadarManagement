/* \file GuangxiStationSet.h
	\brief GuangxiStation参数设置对话框定义
	\ingroup GuangxiStationGroup
	
	本文件定义了GuangxiStationSet参数设置对话框类。

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

#ifndef __HEADER_GuangxiStation_Set_20171127
#define __HEADER_GuangxiStation_Set_20171127

#include "GuangxiStation_gui.h"
#include "wxipctrl.hpp"

using namespace CDYW::PluginSystem;
static const PL_GUID g_guid_Set = { 0xAB390FC2, 0x516F, 0x4FD1, { 0xA2, 0xD2, 0xD6, 0xBD, 0x92, 0x09, 0x80, 0x3E } };

/*! \class GuangxiStationSet
	\brief 参数设置对话框类
	\ingroup GuangxiStationGroup
*/
class GuangxiStationSet : 
	public GuangxiStationPanelSet ,
	public IParaPageHelper< GuangxiStationSet, wxWindow >
{
public:
	GuangxiStationSet( wxWindow* parent);

	virtual ~GuangxiStationSet();

	/* 重载自IDlgParapage*/
	virtual void OnApply();
	virtual int OnCheckApply();

private:
	void init();
};

#endif

