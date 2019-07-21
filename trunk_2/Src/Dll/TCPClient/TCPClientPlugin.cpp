// TCPClient.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "TCPClientPlugin.h"
#include "TCPClient.h"

CTCPClientPlugin* g_pMain = NULL;

int _iInsCnt;  //!< 实例的全局编号计数器
vector<ITCPClientInstance *> m_vecIns;  //!< 网络实例容器
// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 TCPClient.h
CTCPClientPlugin::CTCPClientPlugin()
{
	g_pMain = this;
	_iInsCnt = 0;
	m_vecIns.clear();
	return;
}

CTCPClientPlugin::~CTCPClientPlugin()
{
	try{
		vector<ITCPClientInstance*>::iterator it,ite;
		it = m_vecIns.begin();
		ite = m_vecIns.end();

		while( it != ite ) 
		{
			ITCPClientInstance *pTmpIns = (*it);
			CTCPClient *pIns = (CTCPClient *)pTmpIns;
			delete pIns;
			pTmpIns = NULL;
			it++;
		}
	}catch(...)
	{
		return;
	}
}

ITCPClientInstance * CTCPClientPlugin::GetInstance( 
	const unsigned int RecePackSize, /*接收包最大值*/ 
	const unsigned int SendPackSize, /*发送包最大值*/ 
	const unsigned int ReceBuffSize, /*接收缓冲区大小*/ 
	const unsigned int SendBuffSize /*发送缓冲区大小*/ )
{
	CTCPClient *pIns = new CTCPClient(RecePackSize,SendPackSize,ReceBuffSize,SendBuffSize );
	if( pIns ) {
		m_vecIns.push_back( (ITCPClientInstance*)pIns );
		return (ITCPClientInstance*)pIns;
	}
	return NULL;
}

void CTCPClientPlugin::ReleaseInstance( ITCPClientInstance *pInstance )
{
	try{
		vector<ITCPClientInstance*>::iterator it,ite;
		it = m_vecIns.begin();
		ite = m_vecIns.end();

		while( it != ite ) {
			ITCPClientInstance *pTmpIns = (*it);

			if( pTmpIns && ( pTmpIns == pInstance ) ) {
				m_vecIns.erase( it );

				it=m_vecIns.begin();
				ite = m_vecIns.end();
				continue;
			}

			it++;
		}

		CTCPClient *pTemIns = (CTCPClient *)pInstance;
		delete pTemIns;
		pInstance = NULL;		
	}catch(...)
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	return;
}

int CTCPClientPlugin::GetInstanceSize()
{
	return m_vecIns.size();
}
