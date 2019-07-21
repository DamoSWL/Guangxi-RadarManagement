/* \file GuangXiEditor.h
	\brief GuangXi编辑器定义
	\ingroup GuangXiGroup
	
	本文件定义了GuangXiEditor编辑器类。

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
#ifndef __HEADER_GuangXi_EDITOR_20171117
#define __HEADER_GuangXi_EDITOR_20171117


using namespace CDYW::PluginSystem;
static const PL_GUID g_guid_Editor = { 0x53419766, 0x4952, 0x47C5, { 0xA4, 0x85, 0xBD, 0xBF, 0x39, 0x66, 0x70, 0xF2 } };

/*! \class GuangXiEditor
	\brief GuangXi编辑器。
\ingroup GuangXiGroup
*/
class GuangXiEditor:
	public IEditor
{
public:

	GuangXiEditor()
	{
		memset( szName, 0, sizeof(szName) );
		strcpy( szName, "Test Editor");

		pluginID = g_PL_GuangXi;
		guid = guid_NULL;
	}

	virtual ~GuangXiEditor() {};

	//! 处理关闭消息
	void OnClose( bool bDestroy )
	{
		if( !bDestroy ) {
			//wxMessageBox(wxT("Hide Editor"));
			wxWindow *pWin = (wxWindow *)hWnd;
			pWin->Show( false );
		}else {
			//wxMessageBox(wxT(" Destroy Editor"));
			wxWindow *pWin = (wxWindow *)hWnd;
			//pWin->Destroy();
			delete pWin;
		}

	}

	void OnSave()
	{

	}
	
	//! 处理编辑器事件
	void OnEditorManagerEvent( int iEvtType )
	{

	}
};

#endif
