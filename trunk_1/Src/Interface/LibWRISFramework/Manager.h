/*********************************************************/
/*! \file Manager.h
	\brief 通用雷达系统框架-管理器类

\version 1.0
\author 舒定波
\date 2008年05月28日


\b 修改记录
	

版权信息：
	Copyright (c) 2008,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.

	\ingroup LibGRSFrameworkInterface
*/
/*********************************************************/

#ifndef _HEADER_MANAGER_20080528
#define _HEADER_MANAGER_20080528

#include "Settings.h"
#include "IPluginFramework.h"
#include "assert.h"

class IEditorManager;
class ConfigManager;
//class LogManager;
//class MacrosManager;
//class PersonalityManager;
//class UserVariableManager;
//class ScriptingManager;
class FileManager;

/*! \class Manager Manager.h
	\brief 管理类
	\ingroup PluginFWGroup
*/
class DLLIMPORT Manager
{
	static bool appShuttingDown;
public:
	//static IPluginFramework *Get();
	static Manager *Get();
	static void Free();
	static void Shutdown();
	static bool IsAppShuttingDown();

	/** Functions returning pointers to the respective sub-manager instances.
	* During application startup as well as during runtime, these functions will always return a valid pointer.
	* During application shutdown, these functions will continue to return a valid pointer until the requested manager shuts down.
	* From that point, the below functions will return null. If there is any chance that your code might execute
	* during application shutdown, you MUST check for a null pointer.
	* The one notable exception to this rule is ConfigManager, which has the same lifetime as Manager itself.
	*
	* The order of destruction is:
	* ----------------------------
	*   ToolsManager,       TemplateManager, PluginManager,
	*   ScriptingManager,   ProjectManager,  EditorManager,
	*   PersonalityManager, MacrosManager,   UserVariableManager,
	*   LogManager
	*   The ConfigManager is destroyed immediately before the applicaton terminates, so it can be
	*   considered being omnipresent.
	*
	* For plugin developers, this means that most managers (except for the ones you probably don't use anyway)
	* will be available throughout the entire lifetime of your plugins.
	*/
	IEditorManager* GetEditorManager() const;
	//LogManager* GetLogManager() const;
	CDYW::PluginSystem::IPluginFramework* GetPluginManager() const;
	//MacrosManager* GetMacrosManager() const;
	//PersonalityManager* GetPersonalityManager() const;
	//UserVariableManager* GetUserVariableManager() const;
	//ScriptingManager* GetScriptingManager() const;
	ConfigManager* GetConfigManager(const wxString &name_space) const;
	//FileManager* GetFileManager() const;
};

#endif

