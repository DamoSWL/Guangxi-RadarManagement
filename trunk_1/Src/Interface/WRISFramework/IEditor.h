/*********************************************************/
/*! \file IEditor.h
	\brief 编辑器接口类

	\version 1.0
	\author 舒定波
	\date 2008年06月30日

	\b 修改记录
		- 1.0 舒定波 20080630

	<b>Copyright (c) 2008,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>

	\ingroup GRSFrameworkInterface
*/
/*********************************************************/

#ifndef _INTERFACE_EDITOR_20080630
#define _INTERFACE_EDITOR_20080630


#define EVT_Editor_Close 1 // 关闭事件
#define EVT_Editor_DisFocus 2 // 失去焦点（不是Notebook的当前显示的Tab）
#define EVT_Editor_Focus 3 // 获得焦点（为NoteBook当前选择Tab）


/*! \class IEditor IEditor.h
	\brief 编辑器接口类，所有编辑器必须由此接口类派生
	如果插件的要包含编辑器，其包含的编辑器必须由本接口类派生

	\ingroup GRSFrameworkInterface
*/
class IEditor
{
public:
	PL_PID pluginID;  //!< 编辑器所属插件的ID
	PL_IID guid;      //!< 编辑器的ID
	char szName[100]; //!< 编辑器的名称
	PL_HWND hWnd;  //!< 编辑器的指针
	bool bChange;  //!< 编辑器内容是否改变。true,改变;false,没有改变
public:
	IEditor() {
		bChange = false;
		pluginID = guid_NULL;
		guid = guid_NULL;
		memset( szName, 0, sizeof(szName));
		hWnd = NULL;
	}

	/*! 处理关闭消息
		\param bDestroy 是否注销插件是调用的此函数
	*/
	virtual void OnClose( bool bDestroy = false ) = 0;
	virtual void OnSave() = 0;

	// Change事件发生，例如当前选择的Tab发生了变化等
	virtual void OnEditorManagerEvent( int iEvtType ) = 0;  // 
};



#endif

