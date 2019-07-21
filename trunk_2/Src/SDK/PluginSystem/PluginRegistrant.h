
/*! \file PluginRegistrant.h
	\brief 插件实例化辅助类
*/

#ifndef __HELPER_PLUGININSTANCE_20080813
#define __HELPER_PLUGININSTANCE_20080813

#include "../interface/LibWRISFramework/Includes.h"

namespace CDYW
{
namespace PluginSystem
{
//////////////////////////////////////////////////////////////////////////

/*!  \class PluginRegistrant

	\brief 插件注册辅助类.

* 在GRS中使用这个类注册你的新插件。你所做的仅仅是创建一个PluginRegistrant类的实例。

* \par
* Example code to use in one of your plugin's source files (supposedly called "MyPlugin"):
* \code
* namespace
* {
*     PluginRegistrant<MyPlugin> registration("MyPlugin");
* }
* \endcode

* \ingroup PluginFWGroup
*/
template<class T> 
class PluginRegistrant
{
public:
	/*! 
		\param plugName 插件的名称
		\param guid 插件的ID.
	
	*/
	PluginRegistrant( const char *plugName, const PL_GUID guid)
	{
		Manager::Get()->GetPluginManager()->RegisterPlugin( guid, // plugin's name
			plugName,
			&CreatePlugin, // creation
			&FreePlugin, // destruction
			&SDKVersion); // SDK version
	}

	static IPlugin* CreatePlugin()
	{
		return new T;
	}

	static void FreePlugin( IPlugin* plugin)
	{
		T * pT = ( T* )plugin;
		delete pT;
	}

	static void SDKVersion(int* major, int* minor, int* release)
	{
		if (major) *major = PLUGIN_SDK_VERSION_MAJOR;
		if (minor) *minor = PLUGIN_SDK_VERSION_MINOR;
		if (release) *release = PLUGIN_SDK_VERSION_RELEASE;
	}
};

//////////////////////////////////////////////////////////////////////////
};
};

#endif

