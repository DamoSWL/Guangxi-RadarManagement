/*! \file udpdatareceive.cpp 
\brief 
\ingroup DataReceiveGroup

\version 1.0
\author lava_sdb
\date 2008/11/09

\b modify log:
\arg \c version 1.0,lava_sdb,2008/11/09 Create

\bug 

<b>Copyright (c) 2008-2010,ChengDu YuanWang Science & Technology CO., LTD.
All rights reserved.</b>

\ingroup DataReceiveInterface
*/


#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif


#include "udpdatareceive.h"


UDPDataReceive::UDPDataReceive( int iNumIns )
{
	_iGlobalNum = iNumIns;
	memset( &m_handle, 0, sizeof(tagSTHandle) );

	memset( &_muHandle, 0, sizeof( pthread_mutex_t) );
	pthread_mutex_init( &_muHandle, NULL );
}

UDPDataReceive::~UDPDataReceive()
{
	StopReceive();
}

//! 启动数据接收
bool UDPDataReceive::StartReceive(const int Type,const char *LocalIP,const unsigned short LocalPort,
	const char *DstIP,const unsigned short DstPort,
	const unsigned int RevBuffLen,const unsigned int SendBuffLen,const unsigned int TimeOutMS)
{
	if( IsReceiving() )
		Close( true );

	if (Type==0)
		return Create( true, LocalIP, LocalPort, RevBuffLen,SendBuffLen );
	else if (Type==1)
		return Create(true, LocalIP, LocalPort, DstIP, DstPort, RevBuffLen,SendBuffLen );
	else
		return CreateEx(Type,true, LocalIP, LocalPort, DstIP, DstPort, RevBuffLen,SendBuffLen );
}


//! 停止数据接收
bool UDPDataReceive::StopReceive()
{
	if( IsReceiving() )
	{
		Close( true );
	}

	return true;
}

bool UDPDataReceive::IsWorking()
{
	return IsReceiving();
}


void UDPDataReceive::RegHandle( void *pHandle, FunDataRecv sigRecv, FunDataEvent sigEvent )
{
	CDYW::Lock lock( &_muHandle );
	if( pHandle )
	{
		m_handle.pHandle = pHandle;

		if(sigRecv)
			m_handle.sigRecv = sigRecv;

		if( sigEvent )
			m_handle.sigEvent = sigEvent;
	}
}

void UDPDataReceive::UnregHandle( void* pHandle)
{
	CDYW::Lock lock( &_muHandle );
	if( pHandle == m_handle.pHandle )
	{
		m_handle.pHandle = NULL;
		m_handle.sigRecv = NULL;
		m_handle.sigEvent = NULL;
	}
}


//! 发送数据到网络连接的另一端
bool UDPDataReceive::SendData( const char *pData, unsigned int uiLen,const char *DstIP/*=NULL*/,unsigned short DstPort/*=0*/ )
{
	return Send( (char*)pData, uiLen, DstIP, DstPort);
}

//////////////////////////////////////////////////////////////////////////
//// YWSocketClient

int UDPDataReceive::OnReceiveData( const char *pData, unsigned int len )
{
	CDYW::Lock lock( &_muHandle );

	if( m_handle.sigRecv )
		return (*m_handle.sigRecv)( m_handle.pHandle, pData, len );

	return 0;
}


void UDPDataReceive::OnEvent(int EventType, void * pData, unsigned int len)
{
	CDYW::Lock lock( &_muHandle );
	if( m_handle.sigEvent ) {
		(*m_handle.sigEvent)( m_handle.pHandle, EventType, (char*)pData, len );
	}
}

const char * UDPDataReceive::GetMessage()
{
	return GetError();
}


