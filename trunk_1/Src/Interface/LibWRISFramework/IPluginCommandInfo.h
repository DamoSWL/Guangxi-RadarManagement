
/*********************************************************/
/*! \file IPluginCommandInfo.h
	\brief 文件定义了 插件命令接口类

	本文件定义了插件命令接口类，此类用于实现插件的菜单、工具条按钮等命令

	Copyright (c) 2008,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.

	\version 1.0
	\author 舒定波
	\date 2008年08月13日

	\b 修改记录

	\ingroup LibGRSFrameworkInterface
*/
/*********************************************************/

#ifndef _INTERFACE_PLUGININFO_20080813
#define _INTERFACE_PLUGININFO_20080813


#include "PluginDef.h"
#include <vector>
#include <string>

using namespace std;


//! CDYW命名空间
namespace CDYW
{
	//! 插件系统命名空间
	namespace PluginSystem
	{
//////////////////////////////////////////////////////////////////////////
/*! \class IPluginCommandInfo
	\brief 插件命令类

	此类用于记录插件支持的各个命令。GRS通过此类来记录插件与GRS中的消息映射。

	\ingroup LibGRSFrameworkInterface
*/
class IPluginCommandInfo
{
public:
	vector< IPluginCommandInfo* > m_vecSubMenu;
	bool m_bIsSubMenu;
	unsigned int m_iSeparator;		//!< 命令是否是间隔符

	unsigned int m_iCommandID;		//!< 主框架分配的命令ID
	unsigned int m_iCommandIDSrc;	//!< 本身的命令ID
	char m_strMenuString[64];		//!< 命令名称
	char m_strHelpString[64];
	char m_strToolTip[64];
	char m_strPicString[64]; //!< 图片名字
	char m_strMenuName[64];
};

//////////////////////////////////////////////////////////////////////////
	}; // end namespace PluginSystem
}; // end namespace CDYW

#endif



