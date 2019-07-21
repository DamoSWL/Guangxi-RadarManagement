
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
			\brief ��������ṩ�ĳ��ù��ܽӿ�

			���ӿ��ṩ�����й���;
				- ��־��Ϣ��¼
				- ����ܴ��ھ���Ļ�ȡ
				- ������·���Ļ�ȡ

			\ingroup GRSFrameworkInterface
		*/
		class ICommon
		{
		public:
			//! ��ȡ����ܴ��ڵľ��
			virtual PL_HWND GetMainFrameHwnd() = 0;

			//! ��ȡ ���ݳ�����·��
			virtual const char* GetWorkPath() = 0;

			//! ��ȡ��ʱ�ļ�����·��
			virtual const char* GetTempPath() = 0;

			//! ��ȡ��־�ļ�����·��
			virtual const char* GetLogPath() = 0; 

			//////////////////////////////////////////////////////////////////////////
			//! ��Ӽ�¼��Ϣ
			virtual void AddLog(unsigned int type, const char *szMsg) = 0;

			//! ��Ӹ�ʽ����¼��Ϣ
			virtual void AddLogF( unsigned int type,const char *format, ... ) = 0;

			/*! ��ʾ��ʾ��Ϣ
				\param szTip ��ʾ����
				\param time ��ʾʱ��,��λ������
				\param BeepAct �Ƿ�����,�˹���Ŀǰδʵ��
			*/
			virtual void SetTip( const char *szTip, int time = 3000, int BeepAct = 0 ) = 0;


			/*! ��֤�Ƿ���Ȩ��
				
				����ģ�����ͨ���˺���������Ҫ�ṩ���뱣���Ĺ��ܣ��ṩȨ����֤�� �ú����ᵯ����֤�Ի���Ҫ���û��������룻
				
				\return true,��֤ͨ��
				        false, ��֤ʧ��
			*/
			virtual bool VerifyRight(  ) = 0;
		};
	};
};


#endif
