/*! \file CPluginInfo.h
	\brief 本文件定义了插件信息接口类的实现类
	
*/


#ifndef _HEADER_CPLUGININFO_20080814
#define _HEADER_CPLUGININFO_20080814

#include "../interface/LibWRISFramework/IPluginInfo.h"

namespace CDYW
{
namespace PluginSystem
{
//////////////////////////////////////////////////////////////////////////

/*! \class CPluginInfo
	
	插件信息类，本类记录插件相关信息，如插件的状态，实例等

*/
	class CPluginInfo:
		public IPluginInfo
	{
	public:
		CPluginInfo()
		{
			createProc = NULL;
			freeProc = NULL;
			versionProc = NULL;

			//////////////////////////////////////////////////////////////////////////
			m_hDLLInstance = NULL;
			m_pHost = NULL;
			m_status = STATUS_UNLOAD;
			m_bAutoLoad = false;
			memset( &m_guid, 0, sizeof( PL_GUID ) );
		};
	
		CPluginInfo(CPluginInfo const& AddinInfo)
		{
			m_pHost = AddinInfo.m_pHost;
			m_strName = AddinInfo.m_strName;
			m_strExName = AddinInfo.m_strExName;
			m_strPath = AddinInfo.m_strPath;
			m_strVersion = AddinInfo.m_strVersion;
			m_hDLLInstance = AddinInfo.m_hDLLInstance;
			m_status = AddinInfo.m_status;
			m_bAutoLoad = AddinInfo.m_bAutoLoad;
			m_guid = AddinInfo.m_guid;
			createProc = AddinInfo.createProc;
			freeProc = AddinInfo.freeProc;
			versionProc = AddinInfo.versionProc;
		};
	
		void operator =(CPluginInfo const& AddinInfo)
		{
			m_pHost = AddinInfo.m_pHost ;
			m_strName = AddinInfo.m_strName;
			m_strExName = AddinInfo.m_strExName;
			m_strPath = AddinInfo.m_strPath;
			m_strVersion = AddinInfo.m_strVersion;
			m_hDLLInstance = AddinInfo.m_hDLLInstance;
			m_status = AddinInfo.m_status;
			m_bAutoLoad = AddinInfo.m_bAutoLoad;
			m_guid = AddinInfo.m_guid;
			createProc = AddinInfo.createProc;
			freeProc = AddinInfo.freeProc;
			versionProc = AddinInfo.versionProc;
		};
	
		bool operator ==(CPluginInfo const& AddinInfo)
		{
			if( memcmp( &m_guid, &AddinInfo.m_guid, sizeof(PL_GUID) ) == 0 )
				return true;
	
			return false;
		}
	
		bool operator ==( PL_GUID guid)
		{
			if( memcmp( &m_guid, &guid, sizeof(PL_GUID) ) == 0 )
				return true;
	
			return false;
		}
	};

//////////////////////////////////////////////////////////////////////////
}; // end PluginSystem
}; //CDYW

#endif
