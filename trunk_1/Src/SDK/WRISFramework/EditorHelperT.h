
/*! \file EditorHelperT.h 
	\brief 定义了编辑器模板辅助类
	\ingroup GRSFrameworkGroup
	
	\version 1.0
	\author lava_sdb
	\date 2008/08/20

	\b modify log:
	\arg \c version 1.0,lava_sdb,2008/08/20 Create

	\bug 
	
	<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

*/


#include "../../interface/WRISFramework/Includes.h"

#ifndef __HEADER_EditorT_2008820
#define __HEADER_EditorT_2008820

/*! \class EditorHelperT
	\brief 本模板类用于辅助创建编辑器
	
	本模板类为方便用户创建编辑器而生成，本模板类包含两个模板参数：
	- T Host类
	- winType 编辑器包含的控件类型，控件必须从wxWindow派生

	\see CEditor

	\ingroup GRSFrameworkGroup
*/
template< class T, class winType >
class EditorHelperT:
	public IEditor
{
public:
	EditorHelperT()
	{
		memset( szName, 0, sizeof(szName) );

		pluginID = guid_NULL;
		guid = guid_NULL;
	};

	/*! \brief 处理关闭消息

		处理关闭消息，重载自IEditor。本函数由框架调用。
		
		\param bDestroy 表示是否是销毁编辑器
		
	*/
	void OnClose( bool bDestroy )
	{
		if( !bDestroy ) {
			winType *pWin = (winType *)hWnd;
			pWin->Show( false );
		}else {
			winType *pWin = (winType *)hWnd;
			delete pWin;
		}
	}

	void OnSave()
	{

	}

	void OnEditorManagerEvent( int iEvtType )
	{

	}
};


#endif // __HEADER_EditorT_2008820
