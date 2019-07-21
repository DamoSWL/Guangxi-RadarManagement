// TCPServer.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "TCPServerPlugin.h"
#include "TCPServer.h"
CTCPServerPlugin* g_pMain=NULL;

int _iInsCnt;  //!< 实例的全局编号计数器
vector<ITCPInstance *> m_vecIns;  //!< 网络实例容器

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 TCPServer.h
CTCPServerPlugin::CTCPServerPlugin()
{
	g_pMain = this;
	_iInsCnt = 0;
	m_vecIns.clear();
	return;
}
CTCPServerPlugin::~CTCPServerPlugin()
{
	try{
		vector<ITCPInstance*>::iterator it,ite;
		it = m_vecIns.begin();
		ite = m_vecIns.end();

		while( it != ite ) 
		{
			ITCPInstance *pTmpIns = (*it);
			CTCPServer *pIns = (CTCPServer *)pTmpIns;
			delete pIns;
			pTmpIns = NULL;
			it++;
		}
	}catch(...)
	{
		return;
	}
}

//************************************
// Method:    GetInstance
// FullName:  CTCPServerPlugin::GetInstance
// Access:    public 
// Returns:   ITCPInstance *
// Qualifier: 获取TCP实例指针
//************************************
ITCPInstance * CTCPServerPlugin::GetInstance(const unsigned int RecePackSize,		/*接收包最大值*/
	const unsigned int SendPackSize,		/*发送包最大值*/
	const unsigned int ReceBuffSize,		/*接收缓冲区大小*/
	const unsigned int SendBuffSize)
{
	CTCPServer *pIns = new CTCPServer(RecePackSize,SendPackSize,ReceBuffSize,SendBuffSize );
	if( pIns ) {
		m_vecIns.push_back( (ITCPInstance*)pIns );
		return (ITCPInstance*)pIns;
	}
	return NULL;
}

//************************************
// Method:    ReleaseInstance
// FullName:  CTCPServerPlugin::ReleaseInstance
// Access:    public 
// Returns:   bool
// Qualifier: 释放TCP实例
// Parameter: ITCPInstance * pIns
//************************************
bool CTCPServerPlugin::ReleaseInstance( ITCPInstance *pIns )
{
	try{
		vector<ITCPInstance*>::iterator it,ite;
		it = m_vecIns.begin();
		ite = m_vecIns.end();

		while( it != ite ) {
			ITCPInstance *pTmpIns = (*it);

			if( pTmpIns && ( pTmpIns == pIns ) ) {
				m_vecIns.erase( it );

				it=m_vecIns.begin();
				ite = m_vecIns.end();
				continue;
			}

			it++;
		}

		CTCPServer *pTemIns = (CTCPServer *)pIns;
		delete pTemIns;
		pIns = NULL;		
	}catch(...)
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	return true;
}
