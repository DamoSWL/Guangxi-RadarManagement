/* \file GuangXiSet.h
	\brief GuangXi参数设置对话框定义
	\ingroup GuangXiGroup
	
	本文件定义了GuangXiSet参数设置对话框类。

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

#ifndef __HEADER_GuangXi_Set_20171117
#define __HEADER_GuangXi_Set_20171117

#include "GuangXi_gui.h"

using namespace CDYW::PluginSystem;
static const PL_GUID g_guid_Set = { 0x85E681AF, 0x196E, 0x4DA2, { 0x92, 0xA7, 0xC1, 0xB9, 0x60, 0xDD, 0x18, 0x0C } };


/*! \class GuangXiSet
	\brief 参数设置对话框类
	\ingroup GuangXiGroup
*/


class GuangXiSet : 
	public GuangXiPanelSet ,
	public IParaPageHelper< GuangXiSet, wxWindow >
{
public:
	GuangXiSet( wxWindow* parent);

	virtual ~GuangXiSet();

	/* 重载自IDlgParapage*/
	void OnApply();
	int OnCheckApply();

private:
	void init();
	void getDir(wxCommandEvent& event);
	void getFtpDir(wxCommandEvent& event);
	void delFtpDir(wxCommandEvent& event);
	void getFtpType(wxCommandEvent& event);
	void onSelectedItem(wxListEvent& event);

private:
	int m_JishuButtonID;
	int m_ErchiButtonID;
	int m_ZhuangtaiButtonID;
	int m_BaojingButtonID;
	int m_PingtuID;

	int m_FtpAddButtonID;
	int m_FtpDelButtonID;
	int m_FtpTypeButtonID;

	int m_selectedItemIndex;

	DECLARE_EVENT_TABLE()
};

#endif

