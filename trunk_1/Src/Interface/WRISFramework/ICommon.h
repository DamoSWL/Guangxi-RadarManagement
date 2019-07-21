
#ifndef _INTERFACE_ICOMMON_20080407
#define _INTERFACE_ICOMMON_20080407

#include < LibWRISFramework/PluginDef.h >
#include <string>

#define LOG_TYPE_MESSAGE 0
#define LOG_TYPE_ERROR  1
#define LOG_TYPE_WARNING 2
#define LOG_TYPE_DEBUG  3

namespace CDYW
{
	namespace Plugin
	{
		/*! \class ICommon
			\brief 宿主插件提供的常用功能接口

			本接口提供了下列功能;
				- 日志信息记录
				- 主框架窗口句柄的获取
				- 程序工作路径的获取

			\ingroup GRSFrameworkInterface
		*/
		class ICommon
		{
		public:
			//! 获取主框架窗口的句柄
			virtual PL_HWND GetMainFrameHwnd() = 0;

			//! 获取 数据程序工作路径
			virtual const char* GetWorkPath() = 0;

			//! 获取临时文件保存路径
			virtual const char* GetTempPath() = 0;

			//! 获取日志文件保存路径
			virtual const char* GetLogPath() = 0; 

			//////////////////////////////////////////////////////////////////////////
			//! 添加记录信息
			virtual void AddLog(unsigned int type, const char *szMsg) = 0;

			//! 添加格式化记录信息
			virtual void AddLogF( unsigned int type,const char *format, ... ) = 0;

			/*! 显示提示信息
				\param szTip 提示内容
				\param time 显示时间,单位：毫秒
				\param BeepAct 是否响铃,此功能目前未实现
			*/
			virtual void SetTip( const char *szTip, int time = 3000, int BeepAct = 0 ) = 0;


			/*! 验证是否有权限
				
				其他模块可以通过此函数，对需要提供密码保护的功能，提供权限验证。 该函数会弹出验证对话框，要求用户输入密码；
				
				\return true,验证通过
				        false, 验证失败
			*/
			virtual bool VerifyRight(  ) = 0;
		};
	};
};


#endif
