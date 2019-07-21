// TCPClient.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "TCPClientPlugin.h"
#include "TCPClient.h"

CTCPClientPlugin* g_pMain = NULL;

int _iInsCnt;  //!< ʵ����ȫ�ֱ�ż�����
vector<ITCPClientInstance *> m_vecIns;  //!< ����ʵ������
// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� TCPClient.h
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
	const unsigned int RecePackSize, /*���հ����ֵ*/ 
	const unsigned int SendPackSize, /*���Ͱ����ֵ*/ 
	const unsigned int ReceBuffSize, /*���ջ�������С*/ 
	const unsigned int SendBuffSize /*���ͻ�������С*/ )
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
