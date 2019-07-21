
/*! \file IPluginInfo.h

	\ingroup LibGRSFrameworkInterface
*/


#ifndef __INTERFACE_PLUGIN_INFO_20080814
#define __INTERFACE_PLUGIN_INFO_20080814

#include "IPlugin.h"
#include "PluginDef.h"
#include <vector>
#include <string>

using namespace std;

#define STATUS_UNLOAD 0  //!< 插件没有加载或加载出现错误，不可操作
#define STATUS_SUSPEND 2 //!< 插件处于挂起状态
#define STATUS_REDAY 3  //!< 插件正常激活，处于就绪状态，软件卸载时依据此状态，插件在接受函数OnDestroy函数调用后切换到此状态
#define STATUS_WORKING 4 //!< 插件正常激活，处于正常工作状态

namespace CDYW
{
namespace PluginSystem
{
//////////////////////////////////////////////////////////////////////////

/*! \class IPluginInfo
	\brief 插件信息数据结构
	
	本接口类用于描述插件的基本信息

	\ingroup LibGRSFrameworkInterface
*/
class IPluginInfo
{
protected:
	// typedefs for plugins' function pointers
	typedef void(*PluginSDKVersionProc)(int*,int*,int*);
	typedef IPlugin*(*CreatePluginProc)();
	typedef void(*FreePluginProc)(IPlugin*);
public:
	PL_GUID m_guid;
	PL_HINSTANCE m_hDLLInstance; //!< 插件 DLL 的实例句柄
	IPlugin* m_pHost;    //!< 宿主指针( 插件指针 )
	struct{
		unsigned int m_status:4;	/*!< 插件当前状态。
							- 0，插件有误
							- 1，插件挂起，未加载；
							- 2，插件加载；
							*/
		unsigned int EnableMessge:1;
		unsigned int EnableError:1;
		unsigned int EnableWarnning:1;
		unsigned int EnableDebug:1;
		unsigned int Spa:24;	
	};
	string m_strName; //!< 插件的名称
	string m_strExName; //!< 插件的外部名称
	string m_strPath;	//!< 插件的路径名
	string m_strVersion; //!< 插件的版本号
	bool m_bAutoLoad; //!< 是否自动加载
	unsigned int m_icntDepends;		//!< 需要依赖插件的个数

	CreatePluginProc createProc;
	FreePluginProc freeProc;
	PluginSDKVersionProc versionProc;
};

//////////////////////////////////////////////////////////////////////////
}; // end PluginSystem
}; // end CDYW

#endif
