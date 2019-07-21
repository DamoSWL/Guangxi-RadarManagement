
/*********************************************************/
/*! \file IPluginFramework.h
	\brief 通用雷达系统框架接口定义文件
	\ingroup PluginFWGroup

	\version 1.0
	\author	舒定波
	\date	2008年05月28日

	\b 修改记录：
	\arg \c version 1.1， lava_sdb，20080702

版权信息：
<b>Copyright (c) 2008,ChengDu YuanWang Science & Technology CO., LTD.
All rights reserved.</b>

	\ingroup LibGRSFrameworkInterface
*/
/*********************************************************/
#ifndef _Plugin_Framework_20080325
#define _Plugin_Framework_20080325

#include "Settings.h"
#include <time.h>
#include "PluginDef.h"
#include "IPluginInfo.h"
#include "IPlugin.h"


//! CDYW命名空间
namespace CDYW
{
//! 插件系统命名空间
namespace PluginSystem
{
//////////////////////////////////////////////////////////////////////////

class IPluginFramework;
class IPlugin;

// typedefs for plugins' function pointers
typedef void(*PluginSDKVersionProc)(int*,int*,int*);
typedef IPlugin*(*CreatePluginProc)();
typedef void(*FreePluginProc)(IPlugin*);

/*! 
	\param activePluginGuid 激活的插件的GUID
	\param bHandle 是否需要继续处理此事件:true,继续处理；false,不处理
	\param pData 传送的数据
	\param len 传送的数据的长度
	\return S_OK，函数执行成功；S_FALSE，函数执行失败
*/
typedef int (*FunSpyActived)( PL_PID activePluginGuid, bool &bHandle, unsigned char *pData, unsigned long len );
typedef FunSpyActived FunSpyActiving;
typedef FunSpyActived FunSpySuspend;
typedef FunSpyActived FunSpySuspending;

/*! 插件枚举回调函数
	\param PInfo 插件信息
	\param bStopEnum 设置为true，表示终止枚举过程
*/
typedef int (*EnumPluginProc)( void* pParame, IPluginInfo *pInfo,bool &bStopEnum );

/*! 
* \class IPluginFramework IPluginFrameWork.h
* \brief 插件框架接口类
* \ingroup LibGRSFrameworkInterface
*
* 此接口类定义了插件框架的相关功能函数.
*/
class IPluginFramework
{
public:

	//! 注册插件到GRS插件系统
	/*! 
		\param PID 插件的GUID
		\param pluginName 插件的名称
		\param createProc 插件的创建函数
		\param freeProc 插件的释放函数
		\param SDKVerProc 插件对应的SDK版本获取函数
	*/
	virtual void RegisterPlugin( PL_GUID PID,  // plugin's guid
		const char *pluginName,		// 插件名称
		CreatePluginProc createProc, // creation
		FreePluginProc freeProc, // destruction
		PluginSDKVersionProc SDKVerProc) = 0; // SDK version

	/*! 查询插件
		\param guid 查询的插件的GUID
		\return 插件的指针
	*/
	virtual IPlugin *QueryPlugin( PL_PID guid ) = 0;

	/*! 查询插件的功能接口
		\param pid  功能接口所属插件的插件GUID
		\param iid 查询的功能接口的GUID
		\return 功能接口指针
	*/
	virtual void *QueryInterfaceP( PL_PID pid, PL_IID iid ) = 0;

	//! 初始化 插件 框架
	virtual void PL_Framework_Initialize() = 0;

	//! 释放 插件 框架
	virtual void PL_Framework_Finally() = 0;

	/*! 从配置文件读取插件信息
		\param cConfigPath 插件信息文件
		\param bLoad 是否自动装载插件
		\return 返回信息，S_OK加载成功
	*/
	virtual int LoadPluginByConfig( char *cConfigPath, bool bLoad) = 0;

	/*! 加载插件
		\param cPluginPath 插件的存放路径
		\return 添加插件成功返true.
	*/
	virtual int LoadPlugin( char* cPluginPath ) = 0;

	/*! 加载插件
		\param cPluginPath 插件的路径
		\param bQuite 是否安静
		\return 插件是否加载成功
	*/
	virtual int LoadPlugin( char* cPluginPath, bool bQuite=false) = 0;

	//! 通知插件进行初始化操作
	virtual int LoadPlugin( IPluginInfo *pInfo, bool bQuite, bool saveChangeToConfig = true) = 0;

	virtual int SuspendPlugin( PL_PID pluginGUID, bool bQuite, bool saveChangeToConfig = true ) = 0;

	/*! 释放插件
		\param pluginGUID 要释放的插件的GUID
		\return true,移除成功
	*/
	virtual bool SuspendPlugin( PL_PID pluginGUID ) = 0;

	virtual void SuspendAllPlugin() = 0;

	//向所有插件广播OnExit事件
	virtual void BroadcastOnExitToAllPlugin() = 0;

	//向所有插件广播OnExit事件
	virtual void BroadcastDataToAllPlugin(tagBroadcastData Data) = 0;

	virtual int RegisterHostPlugin( IPluginInfo *PInfo ) = 0;
	virtual bool IsPluginActived( PL_PID pluginGUID ) = 0;
	virtual bool IsPluginActived( IPluginInfo *pInfo ) = 0;


	/*! 安装插件
		\param cPluginPath 插件的路径名
		\param askForConfirmation 是否需要用户确认

		\return true,安装成功，或取消安装
	*/
	virtual bool InstallPlugin( char *cPluginPath, bool askForConfirmation = true) = 0;
	
	/*! 卸载插件
		\param pInfo 插件信息
		\param removeFiles 移除插件相关文件

		\return true,卸载成功，或取消卸载
	*/
	virtual bool UninstallPlugin( PL_PID pluginGUID, bool removeFiles = true) = 0;


	/*! 获取指定插件的依赖插件列表
		\param pluginGUID 查询插件的GUID
		\param cnt 插件列表的个数
		\param ppszDepNames 依赖插件的名字列表
		\return 插件列表的指针
	*/
	virtual PL_PID *GetDependenceList( PL_PID pluginGUID, int &cnt, const char ***ppszDepNames ) = 0;

	/*!插件把事件广播到其它插件
		\param pluginGUID 发送广播事件的插件的标识符
		\param eventID 广播事件的ID
		\param pData 广播事件附加的数据
		\param llen 广播事件附加的数据的长度
	*/
	virtual void BroadcastEvent(
		PL_PID pluginGUID,
		unsigned int eventID,
		unsigned char *pData,
		unsigned int llen
	) = 0;


	/* 监控函数一组 */
	/*! 注册监控函数
		\param pluginGUID 插件的GUID
		\param pFunActiving 插件激活前触发事件
		\param pFunActived 插件激活后触发事件
		\param pFunSuspending 插件挂起前触发事件
		\param pFunSuspend 插件挂起后触发事件
	*/
	virtual void RegisterSpy(
		PL_PID pluginGUID,
		FunSpyActiving pFunActiving,
		FunSpyActived pFunActived,
		FunSpySuspending pFunSuspending,
		FunSpySuspend pFunSuspend
	) = 0;

	/*! 注册某插件为Host插件
		\param pluginGUID HostPlugin的ID
		\return
				- S_OK 注册成功
				- S_FALSE 注册失败
	*/
	virtual int RegisterHostPlugin( PL_PID pluginGUID ) = 0;

	virtual int RegisterTimer(tagTimeFirePoint tmFP) = 0;

	/***************************************************
		1. 插件信息查询相关函数
	***************************************************/

	//!根据插件的GUID获取插件的信息
	virtual IPluginInfo *GetPluginInfoByGUID( PL_PID guid ) = 0;

	//! 获取插件数量
	virtual unsigned int GetPluginCount() = 0;

	/*! 插件枚举插件
		\param lpEnumProc 插件枚举回调函数，用户通过设置回调函数中的bStopEnum的值为true，可以终端枚举
		\param pParame, 传递给 lpEnumProc 的第一个参数
		\return 
	*/
	virtual bool EnumPlugin( EnumPluginProc lpEnumProc, void* pParame ) = 0;

	/*! 全局变量相关函数 */
	
	/*! 注册全局变量
		\param key 键
		\param val 值
		\return false 注册失败，变量已经存在
		        true   注册陈宫
	*/
	virtual bool RegVariant( const char *key, const char *val ) = 0;

	/*! \brief 改变指定的变量
	
		当指定的变量不存在时，会创建一个键值对
	*/
	virtual bool ChangeVariant( const char *key, const char *val ) = 0;

	//! 获取指定的变量
	virtual const char *GetVariant( const char *key ) = 0;

	//! 获取Int类型的全局变量
    virtual bool GetVariantInt( const char *key, int &val ) = 0;
};

typedef IPlugin* (*PLUGIN_INITFUNC)( );
typedef void (*PLUGIN_SUSPENDFUNC)( IPlugin* pPlugin);
typedef IPlugin* (*PLUGIN_ACTIVEFUNC)( IPluginFramework *pFrame, PL_PID &PID) ;


//////////////////////////////////////////////////////////////////////////
}; // end namespace PluginSystem
}; // end namespace CDYW
#endif

