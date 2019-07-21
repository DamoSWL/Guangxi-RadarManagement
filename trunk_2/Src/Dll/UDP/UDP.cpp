// UDP.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "UDP.h"
#include "UDPDataReceive.h"

int _iInsCnt;  //!< 实例的全局编号计数器
vector<ISocketNet *> m_vecIns;  //!< 网络实例容器
CUDP::CUDP()
{
	_iInsCnt = 0;
	return;
}

CUDP::~CUDP()
{
	try{
		vector<ISocketNet*>::iterator it,ite;
		it = m_vecIns.begin();
		ite = m_vecIns.end();

		while( it != ite ) 
		{
			ISocketNet *pTmpIns = (*it);
			UDPDataReceive *pUdp = (UDPDataReceive *)pTmpIns;
			delete pUdp;
			pTmpIns = NULL;
			it++;
		}
	}catch(...)
	{
		return;
	}
}

ISocketNet * CUDP::GetInstance()
{
	UDPDataReceive *pUdp = new UDPDataReceive( _iInsCnt++ );
	if( pUdp ) {
		m_vecIns.push_back( (ISocketNet*)pUdp );
		return (ISocketNet*)pUdp;
	}
	return NULL;
}

bool CUDP::ReleaseInstance( ISocketNet *pIns )
{
	try{
		vector<ISocketNet*>::iterator it,ite;
		it = m_vecIns.begin();
		ite = m_vecIns.end();

		while( it != ite ) {
			ISocketNet *pTmpIns = (*it);

			if( pTmpIns && ( pTmpIns == pIns ) ) {
				m_vecIns.erase( it );

				it=m_vecIns.begin();
				ite = m_vecIns.end();
				continue;
			}

			it++;
		}

		UDPDataReceive *pUdp = (UDPDataReceive *)pIns;
		delete pUdp;
		pIns = NULL;		
	}catch(...)
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	return true;
}
