
#ifndef IGRSFrame_h__
#define IGRSFrame_h__

#include <wx/aui/aui.h>
#include "../LibWRISFramework/Includes.h"
/*! \class IGRSFrame
	工作流接口
*/
class IGRSFrame
{
public:

	///// wxAuiToolbar
	//! 获取ID标识的工具栏，并获得工具栏按钮的ID
	virtual wxAuiToolBar* GetToolBar( CDYW::PluginSystem::IPluginToolBar &toolbar, unsigned long& id ) = 0;
	
	/*! 显示，隐藏工具栏
		\param toolbar 工具栏结构体
		\param bShow true，显示工具栏； false，隐藏工具栏
	*/
	virtual void ShowToolBar( CDYW::PluginSystem::IPluginToolBar &toolbar, bool bShow ) = 0;

	/*! 指定的工具栏是否显示
		\param toolbar 工具栏结构体
	*/
	virtual bool IsToolBarShow( CDYW::PluginSystem::IPluginToolBar &toolbar ) = 0;

	/*! Enable/Disable工具栏
		\param toolbar 工具栏结构
		\param bEnable ture,Enable Toolbar; false, Disable Toolbar
	*/
	virtual void EnableToolBar( CDYW::PluginSystem::IPluginToolBar &toolbar, bool bEnable ) = 0;

	/*! 决定制定的工具栏是否是Enable
		\param toolbar 工具栏结构
	*/
	virtual bool IsToolBarEnable( CDYW::PluginSystem::IPluginToolBar &toolbar ) = 0;


	/*! 启动批量工具栏操作
		此功能用于避免太多的Aui更新。本功能必须与 StopBatchOperToolBar 配对在同一线程中使用，
	*/
	virtual void StartBatchOperToolBar() = 0;

	/*! 终止批量工具栏操作
	*/
	virtual void StopBatchOperToolBar( CDYW::PluginSystem::IPluginToolBar &toolBar ) = 0;

	/*! Enable/Disable 工具栏上的按钮
		\param toolbar 工具栏结构
		\param src_id 按钮的在xml定义里的id
		\param bEnable ture,Enable Tool; false, Disable Tool
	*/
	virtual bool EnableToolBarTool( CDYW::PluginSystem::IPluginToolBar &toolbar, int src_id, bool bEnable ) = 0;

	/*! Toggle/UnToggle 工具栏上的按钮
		\param toolbar 工具栏结构
		\param src_id 按钮的在xml定义里的id
		\param bEnable ture,Enable Tool; false, Disable Tool
	*/
	virtual bool ToggleToolBarTool( CDYW::PluginSystem::IPluginToolBar &toolbar, int src_id, bool bToggle ) = 0;

	/*! 重计算工具栏大小
		\param pTB 工具栏指针
	*/
	virtual void RealizeToolBar( wxAuiToolBar* pTB ) = 0;


	virtual wxAuiManager& GetAuiManager() = 0;

	//! 显示软件运行时间信息
	virtual void DisplaySoftRunTime( bool bEnable ) = 0;

	//! 获取系统的状态栏
	virtual wxStatusBar& GetStatusBar() = 0;

	//! 获取应用程序的图标
	virtual wxIcon GetAppIcon() = 0;

	/*! 制定的面板是否显示
		\param panel 面板

		\return true,显示;false,隐藏
	*/
	virtual bool IsPanelShow( CDYW::PluginSystem::IPluginDlgPara &panel ) = 0;
};


/*! \brief GRSFrameID  {079FFFB9-01E0-4e07-B7EA-3FBBC42A719E}
*/
static const PL_GUID guid_I_GRSFrame =
{ 0x5d4a9885, 0xb0fb, 0x4216, { 0xa8, 0x78, 0xd3, 0x27, 0x3, 0x7, 0x51, 0x7d } };


#endif // IGRSFrame_h__





