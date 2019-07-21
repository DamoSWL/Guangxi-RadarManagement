
/*********************************************************/
/*! \file IEditorManager.h
	\brief 编辑器管理类

	\version 1.0
	\author 舒定波
	\date 2008年05月28日

	\b 修改记录
		- 1.0 舒定波 20080528

	\b 版权信息：
	<b>Copyright (c) 2008,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.</b>
	
	\ingroup GRSFrameworkInterface
*/
/*********************************************************/

#ifndef _HEADER_EDITORMANAGER_20080528
#define _HEADER_EDITORMANAGER_20080528

#include "IEditor.h"

/*! \class IEditorManager IEditorManager.h
	\brief 编辑器管理类

	\ingroup GRSFrameworkInterface
*/
class IEditorManager
{
public:
	//! 注册编辑器
	/*! 在 宿主插件中注册编辑器
		\param pEditor 编辑器接口指针
		\param bShow 显示、隐藏编辑器
	*/
	virtual bool RegEditor( IEditor *pEditor, bool bShow) = 0;
	
	//! 注销指定的编辑器
	/*! 在 宿主插件中注销编辑器
		\param editorID 编辑器的ID
	*/
	virtual bool UnregEditor( PL_IID editorID ) = 0;
	
	//! 注销指定插件的编辑器
	/*!
		\param pluginID 插件的ID
	*/
	virtual bool UnregPluginEditor( PL_PID pluginID ) = 0;
	
	//! 获取EditorManager的窗口标识符
	virtual PL_HWND GetManagerHwnd() = 0;

	//! 获取指定的编辑器的接口指针
	/*!
		\param editorID 编辑器的ID
	*/
	virtual IEditor *GetEditor( PL_IID editorID ) = 0;

	/*! 显示/隐藏编辑器
		\param editorID 编辑器ID
		\param bShow true,显示编辑器；false,隐藏编辑器
	*/
	virtual void ShowEditor( PL_IID editorID, bool bShow ) = 0;

	//! 指定的编辑器是否显示
	/*!
		\param editorID 编辑器的ID
	*/
	virtual bool IsEditorShow( PL_IID editorID ) = 0;

	//! 得到指定插件包含的编辑器的个数
	/*!
		\param pluginID 插件的ID
	*/
	virtual unsigned int GetPluginEditorsCount( PL_PID pluginID ) = 0;

	//! 获取编辑器的个数
	virtual unsigned int GetEditorsCount() = 0;

	//! 关闭指定的编辑器
	/*!
		\param editorID 编辑器的ID
		\param dontsave true:不调用编辑器的保存函数；false:调用编辑器的保存函数
	*/
	virtual bool Close( PL_IID editorID,bool dontsave = false) = 0;

	//! 保存指定的编辑器
	/*!
		\param editorID 编辑器的ID
	*/
	virtual bool Save( PL_IID editorID) = 0;
};

#endif

