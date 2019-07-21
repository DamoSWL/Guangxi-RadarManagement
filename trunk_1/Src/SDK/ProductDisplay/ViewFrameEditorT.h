
#include "../../interface/WRISFramework/Includes.h"


#ifndef __HEADER_MultiViewFrameEditorT_2008820
#define __HEADER_MultiViewFrameEditorT_2008820

/*! \class MultiViewFrameEditorT
	\brief 多视图编辑器类框架层辅助模板类
	
	此模板类简化了Tab类产品框架层的创建。

	\ingroup ProductDisplayGroup
*/
template< class T >
class MultiViewFrameEditorT:
	public IEditor
{
public:
	MultiViewFrameEditorT()
	{
		memset( szName, 0, sizeof(szName) );

		pluginID = g_guid;
		guid = NewGUID();  // 创建GUID

		bChange = false;
	};

	//! 处理关闭消息
	void OnClose( bool bDestroy )
	{
		if( !bDestroy ) {
			wxWindow *pWin = (wxWindow *)hWnd;
			pWin->Show( false );
		}
	}

	void OnSave()
	{
		int i = 0;
	}

	PL_IID GetEditorID()
	{
		return guid;
	}

	void OnEditorManagerEvent( int iEvtType )
	{

	}
};


#endif // __HEADER_MultiViewFrameEditorT_2008820

