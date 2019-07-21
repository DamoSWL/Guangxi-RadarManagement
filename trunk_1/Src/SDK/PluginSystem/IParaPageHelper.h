
/*! \file IParaPageHelper.h
	\brief 本文件为参数设置页的建立提供了一个辅助类
	
	通过IParaPageHelper辅助类,用户可以方便的建立参数设置对话框页

	\author lava_sdb
*/

#ifndef _HELPER_DLGPARAPAGE_20080813
#define _HELPER_DLGPARAPAGE_20080813

#include "../interface/LibWRISFramework/IPluginHost.h"


//! CDYW命名空间
namespace CDYW
{
//! 插件系统命名空间
namespace PluginSystem
{
//////////////////////////////////////////////////////////////////////////

/*! \class MgrParaPage
	\brief 参数设置对话框辅助类
	
	插件要使用的参数设置对话框和普通面板都必须从此类派生

	\ingroup PluginSDK
	\ingroup GRSFrameworkGroup
	\see wxPanelGRS
*/
template< class T , class TBase>
class IParaPageHelper:
	public IDlgParaPage
{
public:
	//! 创建页
	static void CreatePage( IPluginDlgPara &para, PL_HWND hPar)
	{
		T *pT = new T( (TBase *)hPar );
		para.hWnd = (TBase *)pT;
		para.pPage = (IDlgParaPage*)pT;
		para.pthis = pT;
	};

	//! 删除页
	static void DeletePage( IPluginDlgPara &para )
	{
		TBase *pTB = (TBase *)para.hWnd;
		//pTB->Destroy();
		T *pT = (T*)para.pthis;
		delete pT;
	}
};

//////////////////////////////////////////////////////////////////////////
};
};
#endif
