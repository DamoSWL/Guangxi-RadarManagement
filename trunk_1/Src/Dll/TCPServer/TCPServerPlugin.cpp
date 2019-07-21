// TCPServer.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "TCPServerPlugin.h"
#include "TCPServer.h"
CTCPServerPlugin* g_pMain=NULL;

int _iInsCnt;  //!< ʵ����ȫ�ֱ�ż�����
vector<ITCPInstance *> m_vecIns;  //!< ����ʵ������

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� TCPServer.h
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
// Qualifier: ��ȡTCPʵ��ָ��
//************************************
ITCPInstance * CTCPServerPlugin::GetInstance(const unsigned int RecePackSize,		/*���հ����ֵ*/
	const unsigned int SendPackSize,		/*���Ͱ����ֵ*/
	const unsigned int ReceBuffSize,		/*���ջ�������С*/
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
// Qualifier: �ͷ�TCPʵ��
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
