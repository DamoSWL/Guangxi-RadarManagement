/*! \file IPluginHost.h
	\brief 本文描述了宿主插件的接口
	\ingroup LibGRSFrameworkInterface
*/

#ifndef __INTERFACE_PLUGIN_HOST_20080813
#define __INTERFACE_PLUGIN_HOST_20080813

namespace CDYW
{
	namespace PluginSystem
	{

//////////////////////////////////////////////////////////////////////////
//////  宿主插件
//! 参数设置对话框模板
struct IPluginDlgPara;

/*! \typedef void (*PFUN_CreateDlgPara)( IPluginDlgPara &para, PL_HWND hPar )
\brief 参数设置对话框的创建函数指针
GRS使用此函数指针创建相应的参数设置对话框
*/
typedef void (*PFUN_CreateDlgPara)( IPluginDlgPara &para, PL_HWND hPar );

/*! \typedef void (*PFUN_DeleteDlgPara)( IPluginDlgPara &para )
\brief 参数设置对话框注销指针
GRS使用此函数删除相应的参数设置对话框
*/
typedef void (*PFUN_DeleteDlgPara)( IPluginDlgPara &para );

/*! \class IDlgParaPage
	\brief 参数设置对话框接口类，所有的参数设置对话框必须从此接口类派生

	本接口类提供了响应函数 OnApply(...)，来响应参数设置对话框上的“应用”按钮。
	\ingroup LibGRSFrameworkInterface
*/
class IDlgParaPage
{
public:
	virtual void OnApply() = 0;
	virtual int OnCheckApply()		//正常返回0
	{
		OnApply(); //兼容以前的代码
		return 0;
	}
	virtual void SelectPage(int Sel)
	{
		return;
	}
};

/*! \name PaneStyleGroup
	面板样式定义
*/
/*! @{ */

#define PANE_DIR_LEFT		0x00000001	// dock left
#define PANE_DIR_RIGHT		0x00000002	// dock right
#define PANE_DIR_TOP		0x00000004	// dock top
#define PANE_DIR_BOTTOM		0x00000008// dock bottom
#define PANE_DIR_CENTERR	0x00000010	// dock center H，水平居中
#define PANE_DIR_CENTERE	0x00000020	// dock center V，垂直居中
#define PANE_FIXED			0x00000040	// FIXED 样式
#define PANE_RESIZEABLE		0x00000080	// RESIZE 样式
#define PANE_DOCK			0x00000100	// DOCK
#define PANE_FLOAT			0x00000200	// 浮动
#define PANE_HIDE			0x00000400	// 隐藏
#define PANE_CAPTION_VISIBLE	0x00000800	// 显示标题栏
#define PANE_BUTTON_MAX		0x00001000	// 最大化按钮
#define PANE_BUTTON_MIN		0x00002000	// 最小化按钮
#define PANE_BUTTON_PIN		0x00004000	// PIN按钮
#define PANE_BUTTON_CLOSE	0x00008000	// 关闭按钮
#define PANE_GRIPPER_VISI	0x00010000	// 显示 GRIPPER
#define PANE_GRIPPER_TOP	0x00020000	// GRIPPER 顶部显示
#define PANE_MAXIMIZE		0x00040000	// 可以最大化
#define PANE_RESTORE		0x00080000	// 可以复原
#define PANE_DOCK_LEFT		0x00100000	// dock left
#define PANE_DOCK_RIGHT		0x00200000	// dock right
#define PANE_DOCK_TOP		0x00400000	// dock top
#define PANE_DOCK_BOTTOM	0x00800000	// dock bottom
#define PANE_FLOATABLE		0x01000000	// 可以浮动
#define PANE_MOVABLE		0x02000000	// 可以移动
#define PANE_BORDER_VISI	0x04000000	// 边框可视
#define PANE_CENTREPANE		0x08000000	// 作为中心面板

#define PANE_DOCK_ALL		PANE_DOCK_LEFT | PANE_DOCK_RIGHT | PANE_DOCK_TOP | PANE_DOCK_BOTTOM
#define PANE_BUTTON_ALL		PANE_BUTTON_MAX | PANE_BUTTON_MIN | PANE_BUTTON_PIN | PANE_BUTTON_CLOSE

//! 缺省样式简单版，不包括DOCK 和浮动
#define PANE_DEF_STYLE_SIMPLE PANE_DIR_RIGHT | \
	PANE_MOVABLE | \
	PANE_BUTTON_PIN | PANE_BUTTON_CLOSE  

#define PANE_DEF_STYLE PANE_DEF_STYLE_SIMPLE 

//! 缺省样式完全版，包含了DOCK和浮动
#define PANE_DEF_STYLE_ALL		PANE_DIR_RIGHT | \
	PANE_MOVABLE | PANE_FLOATABLE | \
	PANE_BUTTON_ALL | \
	PANE_DOCK_ALL

//! 打开某些PANE的样式
#define PANE_STYLE_ENABLE( style, flag  ) \
	style |= flag;

//! 关闭某些PANE的样式
#define PANE_STYLE_DISABLE( style, flag ) \
	style &= (~(flag));

//! 自动退出参数
#define QUITOPTION_NOCMD					-1	//无自动退出命令
#define QUITOPTION_EXITPROCESS				0	//安全退出进程
#define	QUITOPTION_EXITPROCESSANDSHUTDOWN	1	//安全退出进程并自动关闭计算机
#define QUITOPTION_EXITPROCESSANDRESTART	2	//安全退出进程并自动重启计算机
#define QUITOPTION_RESTARTPROCESS			3	//重启进程
#define QUITOPTION_DISABLECANCEL			100	//关机过程中禁止取消

/*! @} */

/*! \struct IPluginDlgPara
	\brief 参数设置对话框结构体，描述参数设置对话框的属性
	\see IHostPlugin::RegisterSettingDialog
	\ingroup LibGRSFrameworkInterface
*/
struct IPluginDlgPara
{
	IDlgParaPage *pPage; //!< \see IDlgParaPage 功能接口指针
	PL_PID pluginGUID; //!< 参数设置对话框所属的插件的GUID
	PL_PSD_ID selfGUID;  //!< 本身设置页的Guid
	PL_PSD_ID parGUID;   //!< 从属的分类的Guid：0，表示使用缺省的
	unsigned long parIndex;  //!< 在分类中排列的序号
	char cName[64];  //!< 选择项的名称
	bool bNewClass; //!< 是否用新的分类显示
	PL_HWND hWnd;      //!< 对话框的句柄
	PFUN_CreateDlgPara funCreate; //!< 创建设置对话框的函数指针
	PFUN_DeleteDlgPara funDelete; //!< 创建设置对话框的函数指针
	void *pthis;	//!< 窗体指针
	unsigned long lStyle;	//!< 面板的样式
};


/*! \struct IPluginPanel
	\brief 面板结构体，描述面板的相关属性
	\ingroup LibGRSFrameworkInterface
	\see IHostPlugin::RegisterPanel
*/
typedef IPluginDlgPara IPluginPanel;


/*! \struct IPluginMenu
\brief 菜单结构体，描述菜单相关属性

在GRS中使用XML来描述菜单结构，如下所示：
\code
const char g_MenuXML[] ="<?xml version=\"1.0\" encoding=\"gb2312\" ?>"
"<Plugin name=\"wxPlugin\">"
	"<A_Menu name=\"压缩(&amp;Y)\" pos=\"5\" >"
		"<B_Menu name=\"-\" separator=\"-1\" order=\"2\" />"

		"<LeafMenu name=\"-\" separator=\"-1\" />"
		"<LeafMenu name=\"压缩格式21\" id=\"11\" tooltip=\"压缩格式\" separator=\"0\" order=\"1\" />"
		"<LeafMenu name=\"-\" separator=\"-1\" />"

		"<B_Menu name=\"测试共有菜单\" order=\"0\" >"
			"<LeafMenu name=\"-\" separator=\"-1\" />"
			"<LeafMenu name=\"压缩格式23\" id=\"13\" tooltip=\"压缩格式\" separator=\"0\" order=\"0\" />"
			"<LeafMenu name=\"-\" separator=\"-1\" />"
		"</B_Menu>"

		"<LeafMenu name=\"-\" separator=\"-1\" />"
		"<LeafMenu name=\"压缩格式24\" id=\"14\" tooltip=\"压缩格式\" separator=\"0\" order=\"2\" />"
		"<LeafMenu name=\"-\" separator=\"-1\" />"

		"<B_Menu name=\"压缩2\" order=\"2\" >"
			"<LeafMenu name=\"-\" separator=\"-1\" />"
			"<LeafMenu name=\"压缩格式25\" id=\"15\" tooltip=\"压缩格式\" separator=\"0\" order=\"100\" />"
			"<LeafMenu name=\"-\" separator=\"-1\" />"
		"</B_Menu>"
	"</A_Menu>"
"</Plugin>";

\endcode
\b A_Menu 表示主菜单，只能有一个，由parGUID标识的主菜单已经存在时，加入，不存在则创建
\b B_Menu 表示子菜单，可以有多个，子菜单下还可以添加 LeafMenu 菜单项
\b LeafMenu 菜单项

\arg \c name 菜单项的名称
\arg \c id 菜单项的id，此id在插件中是唯一的，插件根据此id判断UI事件触发源
\arg \c tooltip 菜单项显示的 tooltip
\arg \c separator 菜单项的类型
\arg \c order 菜单项加载位置

- -1 分隔符类型菜单项
- 0 普通菜单项
- 1 check 类型菜单项
- 2 RADIO 类型菜单项

\see IHostPlugin::RegisterMenu
\ingroup LibGRSFrameworkInterface
*/
struct IPluginMenu
{
	PL_MENU_ID selfGUID; //!< 自己菜单的GUID
	PL_MENU_ID parGUID; //!< 主菜单的GUID	
	PL_PID pluginGUID; //!< 菜单所属插件的GUID
		
	char *menuDefine; //!< 菜单项的XML定义
	IPluginCommandInfo menuCommandInfo;  //!< 菜单信息
};

/*! \struct IPluginToolBar
\brief 工具栏结构体，描述工具栏相关属性

GRS使用XML来描述工具栏结构，如下所示：

\code
<?xml version="1.0" encoding="gb2312" ?>
<Plugin name="wxPlugin">
<ToolBar>
<Button name="按钮 1" pic="NowCasting.bmp" id="21" tooltip="这是按钮 1" separator="0" />
<Button separator="1" />
<Button name="按钮 1" pic="Setting.bmp" id="22" tooltip="这是按钮 2" separator="0" />
</ToolBar>
</Plugin>
\endcode
每个\b ToolBar 表示一个工具栏，每个工具栏有多个 \b Button 按钮。

\b Button 按钮属性
\arg \c name 按钮的名称
\arg \c pic 按钮的图片
\arg \c id 按钮的id，此id在插件中是唯一的，插件根据此id判断UI事件触发源
\arg \c tooltip 按钮显示的 tooltip
\arg \c separator 按钮的类型
- 0 普通按钮
- 1 分隔符按钮

\see IHostPlugin::RegisterToolBar
\ingroup LibGRSFrameworkInterface
*/
struct IPluginToolBar
{
	bool bNewBar; //!< 是否作为新的工具条
	PL_PID pluginGUID; //!< 工具条所属插件的 GUID
	PL_TB_ID selfGUID; //!< 工具条的GUID
	PL_TB_ID parGUID; //!< 从属工具条的GUID
	char *barDefine; //!< 工具条项的XML定义
	unsigned long lSize;  //!< 定义的长度
	PL_HWND hWnd; //!< 工具条的句柄
	char name[64]; //!< 工具栏的名称
	vector< IPluginCommandInfo* > toolbarCommandInfo; //!< 插件的工具栏命令结构
};

/*! 自定义命令数据结构
	\ingroup LibGRSFrameworkInterface
*/
struct IPluginCommon
{
	PL_PID pluginGUID; //!< 命令段所属插件的GUID
	unsigned long ID_Start_Src;  //!< 起始ID
	unsigned long ID_End_Src;    //!< 终止ID
	unsigned long ID_Start_Map;  //!< 映射的消息ID
	char name[64]; //!< 自定义命令的名称
	vector< IPluginCommandInfo* > otherCommandInfo; //!< 插件的其他命令结构
};

#define RESFUNCODE_QUEREG		0xA51001	//查询注册状态
#define RESFUNCODE_MODBUGMON	0xA51002	//修改BugReport监控时间状态
#define RESFUNCODE_QUE64RUNTIME	0xA51003	//查询操作系统运行时间（64位，毫秒）
typedef struct _ReserveFun_
{
	unsigned int FunCode;	//功能码
	void *pPar;				//参数指针
}tagReserveFunPar;

#define REGSTATUS_TRY			1000	//试用状态
#define REGSTATUS_CIVILREG		1001	//民品已注册状态
#define REGSTATUS_ARMREG		1002	//军品已注册状态
#define REGSTATUS_NONEEDREG		1003	//不需要状态，开放状态

/*!	\class IHostPlugin IPluginFrameWork.h
	\brief 宿主插件类接口

	宿主插件是一类特殊的插件，宿主插件提供了UI相关的操作实现。
	一个GRS项目有且只能有一个宿主插件。
	\ingroup LibGRSFrameworkInterface
*/
class IHostPlugin
{
public:
	//! 保留功能接口，提供给后续功能扩展，后续如需添加新功能可使用该函数，避免每一次改接口都要重新编译所有插件
	virtual void ReserveFun(void *pPar) = 0;

	//! 触发程序自动退出，返回false主程序不允许自动退出
	virtual bool AutoQuit(unsigned int Option=QUITOPTION_EXITPROCESS) = 0;

	//! 当前是否是Debug模式
	virtual bool IsDebugMode() = 0;

	//! 是否宽架初始化完成
	virtual bool IsAllInitFinished() = 0;

	//! 运行指定的命令
	virtual bool InvokPluginCommand(unsigned int iCommandID) = 0;

	//! 根据命令ID 获取命令ID 所属的插件的信息
	virtual const IPluginInfo *GetPluginInformation( unsigned int iCommandID ) = 0;

	//! 得到命令信息
	virtual const IPluginCommandInfo *GetPluginCommandInfo( unsigned int iCommandID ) = 0;

	//! 获取命令信息
	virtual const IPluginCommandInfo *GetPluginCommandInfoBySrc(
		PL_GUID guid,
		unsigned long iCommandIDSrc
		) = 0;

	//! 得到参数设置对话框
	virtual PL_PAGE* GetOptionSheet() = 0;

	//! 注册参数设置对话框
	virtual void RegisterSettingDialog( IPluginDlgPara para ) = 0;

	//! 注册菜单的函数
	virtual bool RegisterMenu( IPluginMenu menu) = 0;

	//! 注册工具条的函数
	virtual bool RegisterToolBar( IPluginToolBar bar) = 0;

	//! 注册面板
	virtual bool RegisterPanel( IPluginDlgPara panel ) = 0;

	//! 注册快捷键
	/*!
		\param PID 插件ID
		\param cfgPath 插件的快捷键配置文件路径
	*/
	virtual bool RegisterShortcut( PL_PID PID, const char *cfgPath ) = 0;

	//! 注册命令
	/*!
	\return 返回的首命令的映射ID
	*/
	virtual unsigned long RegisterCommon( IPluginCommon &comm) = 0;


	//! Yes/No模式对话框，询问用户是否继续某类型操作
	virtual bool AskDialog( unsigned int askID, char *strAsk=NULL ) = 0;

	//! Enable 指定 id 的命令
	/*!
	\param toolbar 工具条结构
	\param id 命令ID
	\param bEnable true, Enable; false,Disable;
	*/
	virtual bool EnableToolBarCommand(IPluginToolBar &toolbar, unsigned long id, bool bEnable) = 0;
	virtual bool EnableToolBarCommand(PL_TB_ID toolbar_guid, unsigned long id, bool bEnable) = 0;

	//! Check 指定 id 的命令
	/*!
	\param toolbar 工具条结构
	\param id 命令ID
	\param bCheck true, Check; false,UnCheck;
	*/
	virtual bool CheckToolBarCommand(IPluginToolBar &toolbar, unsigned long id, bool bCheck) = 0;
	virtual bool CheckToolBarCommand(PL_TB_ID toolbar_guid, unsigned long id, bool bCheck) = 0;
	//! Enable 指定 id 的命令
	/*!
	\param menu 菜单结构
	\param id 命令ID
	\param bEnable true, Enable; false,Disable;
	*/
	virtual bool EnableMenuCommand( IPluginMenu &menu, unsigned long id, bool bEnable) = 0;
	virtual bool EnableMenuCommand( PL_MENU_ID menu_guid, unsigned long id, bool bEnable) = 0;

	//! Check 指定 id 的命令
	/*!
	\param menu 菜单结构
	\param id 命令ID
	\param bCheck true, Check; false,UnCheck;
	*/
	virtual bool CheckMenuCommand( IPluginMenu &menu, unsigned long id, bool bCheck) = 0;
	virtual bool CheckMenuCommand( PL_MENU_ID menu_guid, unsigned long id, bool bCheck) = 0;

	/************************************************************************/
	/*      Editor Manager                                                 */
	/************************************************************************/

	/* 添加编辑器到 tabView
		\param hWnd 要添加的编辑器的句柄
		\param szName 要添加的编辑器的名称
		\return true，添加成功；false，添加失败
	*/
	//bool AddEditor( PL_HWND hWnd, const char* szName );

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	/*! 
		\brief 显示参数设置对话框或其它功能面板 
		\param panel 要显示的对话框或功能面板(Panel)的结构体
		\param bShow true:显示；false:隐藏
	*/
	virtual void ShowPanel( IPluginDlgPara &panel, bool bShow=true) = 0;

	virtual void ShowTheSetDlg( PL_GUID selfID,int PageSel=0 ) = 0;

	virtual IPluginDlgPara GetPanlePara( PL_GUID ID) = 0;
	virtual IPluginDlgPara GetDldPara( PL_GUID ID) = 0;

	//解析菜单项配置文件接口
	// szmenu 菜单配置参数
	// filepath 配置文件路径
	virtual bool InitMenu( char *Szmenu , const char *Szfilepath ) = 0;

};


/*! GUID比较函数对象

*/
class EquGUID
{
	PL_GUID _guid;
public:
	EquGUID() {};
	EquGUID( PL_GUID guid ){ _guid = guid; }

	bool operator()( const PL_GUID &guid )
	{
		return !memcmp( &guid, &_guid, sizeof( PL_GUID) );		
	}
};


//////////////////////////////////////////////////////////////////////////
}; // end namespace PluginSystem
}; // end namespace CDYW

#endif
