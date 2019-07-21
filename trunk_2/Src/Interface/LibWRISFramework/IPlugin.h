

/*! \file IPlugin.h
	\brief 本文件定义了插件的接口类

	本文定义了插件接口类，所有的插件都必须提供IPlugin接口（从IPlugin派生）。
*/
#ifndef __INTERFACE_IPLUGIN_20080813
#define __INTERFACE_IPLUGIN_20080813
/************************************************************************/
/*    Interface IPlugin                                                 */
/************************************************************************/

//! CDYW命名空间
namespace CDYW
{
//! 插件系统命名空间
namespace PluginSystem
{

//////////////////////////////////////////////////////////////////////////
class IPluginFramework;

//! 计时器功能结构体
/*! 
	\ingroup LibGRSFrameworkInterface
*/
typedef struct _TimeFirePoint
{
	PL_PID PID; //!< 插件的GUID
	int iType; /*! 时间事件触发类型
				- 0 在指定时间间隔触发，每？秒触发
				- 1 在每天？时间触发
				- 2 在每周星期几什么时间触发
				- 3 在每月第几天什么时间触发
				- 4 一次性触发 
				*/
	unsigned long timeID;  //!< 计时器事件的ID
	bool bFire; //!< 是否在 插件装载完毕后触发一次
	tm time;    //!< 时间
}tagTimeFirePoint;
#define CANCEL_SHUTDOWN		0xFFFF0001
#define QUERY_IFCANSHUTDOWN	0xFFFF0002		//查询各插件，询问是否能退出，*pData指向bool型，"true"允许退出，"false"不能退出
#define RETSHUTDOWN_ENABLE		0			//*tagBroadcastData:pData=0允许关闭
#define RETSHUTDOWN_DISENABLE	1			//*tagBroadcastData:pData=1不允许关闭
typedef struct _BroadcastData
{
	unsigned int Type;
	char *pData;
}tagBroadcastData;
/*! \class IPlugin
	\brief 插件接口，所有的GRS插件都必须实现此接口
	\ingroup LibGRSFrameworkInterface
*/
class IPlugin
{
public:
	//! 工具条/菜单 消息处理函数
	/*!
	\param wNotifyCode 消息码
	\param wID 命令的 ID
	\param hWndCtl 控件的ID
	\return true,命令处理；false,不能识别的命令；
	*/
	virtual bool OnCommandHandle(
		PL_WORD wNotifyCode,
		PL_WORD wID,
		void * hWndCtl
		) = 0;

	/*! 及时器事件处理函数
	*/
	virtual bool OnTimerHandle(
		tagTimeFirePoint *pFS,
		void *,
		unsigned long /*lSize*/
		) = 0;

	/*!广播事件处理函数，
	\param pluginGUID 发送广播事件的插件的标识符
	\param eventID 广播事件的ID
	\param pData 广播事件附加的数据
	\param llen 广播事件附加的数据的长度
	*/
	virtual void OnBroadcastEvent(
		PL_PID pluginGUID,
		unsigned int eventID,
		unsigned char *pData,
		unsigned int llen
		) = 0;

	//全局数据广播
	virtual void BroadcastData(tagBroadcastData Data) = 0;
	//////////////////////////////////////////////////////////////////////////

	//! 初始化插件，此函数由Framework调用
	/*! 插件在此函数中获取各个需要的功能接口
	\param pFrame	插件框架接口指针
	\param PID 插件的GUID
	\return 插件的GUID
	*/
	virtual int Initialize( IN IPluginFramework *pFrame, IN PL_PID &PID) = 0 ;


	//! 初始化完毕后调用, 插件在此函数中进行UI注册操作（菜单、工具栏、设置面板等）
	virtual void OnInitialized() = 0;

	//! 当插件的所有UI初始化结束后调用
	virtual void OnActived() = 0;

	//! 在程序退出OnDestroy之前调用
	virtual void OnExit() = 0;

	//! 在结束插件时调用
	virtual void OnDestroy() = 0;

	//! 获取插件的GUID
	virtual PL_PID GetGUID() = 0;

	//! 获取插件名称
	virtual const char *GetName() = 0;

	//! 获取插件功能说明
	virtual const char *GetNote() = 0;

	//! 获取插件版本信息
	virtual const char *GetVersion() = 0;

	//! 获取作者
	virtual const char *GetAuthor() = 0;

	//! 获取插件标识号,用于区别具有特定功能的插件
	virtual unsigned long GetPluginType() = 0;

	/*! 查询接口
	\param guid 查询的功能接口的GUID
	\return 功能接口的指针
	*/
	virtual void *QueryInterfaceP( PL_IID guid ) = 0;

	/*! 获取指定插件的依赖插件列表
		\param cnt 插件列表的个数
		\param ppszDepNames 插件列表名称
		\return 插件列表的指针
	*/
	virtual PL_GUID *GetDependenceList( int &cnt, const char ***ppszDepNames ) = 0;

	//设置插件LOG信息输出选项
	virtual void SetLogOption(bool EnableMessge,bool EnableError,bool EnableWarnning,bool EnableDebug) = 0;	

	virtual void ReservedFun1(void *pPar) = 0;//预留功能接口1
	virtual void ReservedFun2(void *pPar) = 0;//预留功能接口2
	virtual void ReservedFun3(void *pPar) = 0;//预留功能接口3
	virtual void ReservedFun4(void *pPar) = 0;//预留功能接口4
	virtual void ReservedFun5(void *pPar) = 0;//预留功能接口5
};

}; // end namespace PluginSystem
}; // end namespace CDYW

#endif
