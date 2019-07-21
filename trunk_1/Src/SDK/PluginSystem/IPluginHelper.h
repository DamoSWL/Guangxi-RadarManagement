/*! \file IPluginHelper.h
	\brief 文件存放了一些插件相关的辅助类
*/


#ifndef __HEADER_IPluginHelper_20080813
#define __HEADER_IPluginHelper_20080813

#include "../interface/LibWRISFramework/Includes.h"
typedef struct LOGCTL
{
	union{
		unsigned int Temp;
		struct{
			unsigned int EnableMessge:1;
			unsigned int EnableError:1;
			unsigned int EnableWarnning:1;
			unsigned int EnableDebug:1;
			unsigned int Spa:28;	
		};
	};	
}tagLogCtl;
//! CDYW命名空间
namespace CDYW
{
//! 插件系统命名空间
namespace PluginSystem
{
//////////////////////////////////////////////////////////////////////////

/*! \class IPluginHelper
	\brief 本类为辅助类，使用本类可以更方便的创建插件。
	
	本类为一些不常使用的功能函数实现了缺省操作。 用户的插件类从此类派生时，只需重载需要的功能函数而不是所有的功能函数。
	\b 派生类必须包含 \a PL_PID 类型的变量 \a g_guid，此变量是插件的 ID。
*/
template< class T >
class IPluginHelper:
	public IPlugin
{
protected:
	IPluginFramework *m_IFrame; //!< 插件框架指针
	PL_GUID m_guid;
	tagLogCtl m_LogCtl;	//LOG日志输出控制
	CDYW::Plugin::ICommon *m_IComm;
public:

	// 构造函数
	IPluginHelper():
	  m_IFrame( NULL )
	{
		m_IComm = NULL;
		m_LogCtl.Temp = 0xFFFFFFFF;
	}
	
	//! 工具条/菜单 消息处理函数
	bool OnCommandHandle(
		PL_WORD wNotifyCode,
		PL_WORD wID,
		void * hWndCtl)
	{
		return true;
	}

	//! 及时器事件处理函数
	bool OnTimerHandle(
		tagTimeFirePoint *pFS,
		void *,
		unsigned long /*lSize*/)
	{
		return true;
	}

	//!广播事件处理函数，
	void OnBroadcastEvent(
		PL_PID pluginGUID,
		unsigned int eventID,
		unsigned char *pData,
		unsigned int llen
		)
	{
		return;
	}
	//数据广播函数
	void BroadcastData(tagBroadcastData Data)
	{
		if(Data.Type==QUERY_IFCANSHUTDOWN)
			*Data.pData = RETSHUTDOWN_ENABLE;
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	int Initialize( IN IPluginFramework *pFrame, IN PL_PID &PID)
	{
		m_IFrame = pFrame;
		m_IComm = (CDYW::Plugin::ICommon *)m_IFrame->QueryInterfaceP( guid_Frame, guid_I_Common );
		//if( IsEqualID( PID, m_guid) )
		//	return S_OK;
		return S_OK;
	}

	//! 初始化完毕后调用, 插件在此函数中进行UI注册操作（菜单、工具栏、设置面板等）
	void OnInitialized() 
	{
		return;
	}

	//在程序退出,OnDestroy函数执行之前调用,该函数由框架退出线程调用，
	//重载OnExit函数以后可在该函数中处理各插件自定义线程结束操作，请避免在该函数中操作UI
	void OnExit() 
	{
		return;
	}

	//! 当插件的所有UI初始化结束后调用
	void OnActived() 
	{
		return;
	}

	//! 在结束插件时调用
	void OnDestroy() 
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////

	//! 获取插件功能说明
	const char *GetNote() 
	{
		return "";
	}


	//! 获取作者
	const char *GetAuthor() 
	{
		return "Anonymous";
	}

	//! 获取插件标识号,用于区别具有特定功能的插件
	unsigned long GetPluginType() 
	{
		return 999;
	}

	//! 查询接口
	void *QueryInterfaceP( PL_IID guid ) 
	{
		return NULL;
	}

	//! 获取指定插件的依赖插件列表
	PL_GUID *GetDependenceList( int &cnt, const char ***ppszDepNames ) 
	{
		cnt=0;
		if( ppszDepNames != NULL )
		{
			*ppszDepNames = NULL;
		}
		return NULL;
	}
	//主宽架程序调用单独设置各插件LOG输出控制
	void SetLogOption(bool EnableMessge,bool EnableError,bool EnableWarnning,bool EnableDebug)
	{
		m_LogCtl.EnableMessge = EnableMessge;
		m_LogCtl.EnableError = EnableError;
		m_LogCtl.EnableWarnning = EnableWarnning;		
		m_LogCtl.EnableDebug = EnableDebug;
	}
	void AddLog(unsigned int type, const char *szMsg)
	{
		if(type==LOG_TYPE_ERROR && m_LogCtl.EnableError) 
			m_IComm->AddLog(type,szMsg);
		else if(type==LOG_TYPE_MESSAGE && m_LogCtl.EnableMessge) 
			m_IComm->AddLog(type,szMsg);
		else if(type==LOG_TYPE_WARNING && m_LogCtl.EnableWarnning) 
			m_IComm->AddLog(type,szMsg);
		else if(type==LOG_TYPE_DEBUG && m_LogCtl.EnableDebug) 
			m_IComm->AddLog(type,szMsg);

	}
	void AddLogF( unsigned int type,const char *format, ... )
	{
		bool flag=false;
		if(type==LOG_TYPE_ERROR && m_LogCtl.EnableError) flag = true;
		else if(type==LOG_TYPE_MESSAGE && m_LogCtl.EnableMessge) flag = true;
		else if(type==LOG_TYPE_WARNING && m_LogCtl.EnableWarnning) flag = true;
		else if(type==LOG_TYPE_DEBUG && m_LogCtl.EnableDebug) flag = true;

		if(flag)
		{
			va_list args;
			va_start(args,   format);
			char _mess[500+10]={0};
			memset(_mess,0,sizeof(_mess));
			int ret=_vsprintf_p(_mess,   500,format,   args);	
			va_end(args);
			if(ret==-1)
				sprintf(_mess,"无效的LOG输出信息");
			m_IComm->AddLog(type,_mess);
		}
	}

	void ReservedFun1(void *pPar)//预留功能接口1
	{

	}
	void ReservedFun2(void *pPar)//预留功能接口2
	{
	}
	void ReservedFun3(void *pPar)//预留功能接口3
	{

	}
	void ReservedFun4(void *pPar)//预留功能接口4
	{

	}
	void ReservedFun5(void *pPar)//预留功能接口5
	{

	}
};

//////////////////////////////////////////////////////////////////////////
};
};
#endif
