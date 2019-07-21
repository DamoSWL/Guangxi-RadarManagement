
#include "ywsocket_udp.h"
#include "MSWsock.h"

using namespace CDYW::SOCKET;

//////////////////////////////////////////////////////////////////////////
///// class YWSocket_UDP
YWSocket_UDP::YWSocket_UDP()
{
	//init variable
	m_bCreateOK = false;
	m_NetType = 0;
	m_bManul = false;
	s_message	= "";
	m_socket	= -1;
	memset( &m_thrRec, 0, sizeof( pthread_t ) );

	m_iRecBuffLen	= 0;
	m_bIsReceiving = false;
	m_pRecvBuf = NULL;

	_recThrTimeout.tv_sec = 1;
	_recThrTimeout.tv_usec = 1000000;
	m_pThis = (void*)this;
}


//--------------
YWSocket_UDP::~YWSocket_UDP()
{
	Close( false );

	if( m_pRecvBuf ) {
		delete []m_pRecvBuf;
		m_pRecvBuf = NULL;
	}
}


bool YWSocket_UDP::Socket(int nAddressFormat,int nSocketType,int nProtocolType)
{
	m_socket = socket(nAddressFormat,nSocketType,nProtocolType);
	if ( m_socket != -1 ){
		s_message = "Socket Create OK!";

		// 设置socket为非阻塞模式
		u_long non_block = 1;
		ioctlsocket(m_socket, FIONBIO, &non_block);
		return true;
	}else{
		s_message = "Socket Create error.	Please check the sock has been init!";
		OnEvent( YW_EVENT_SOCKET_CREATE_ERROR );
		return false;
	}
}

bool YWSocket_UDP::Bind( unsigned int nSocketPort, const char* cIP)
{
	// / ADD
	string szIP(cIP);

	memset( &m_sockaddr, 0 , sizeof(sockaddr_in) );
	m_sockaddr.sin_family = AF_INET;
	if(szIP != "")
		m_sockaddr.sin_addr.s_addr = inet_addr( (char*)szIP.c_str() );	
	else 
		m_sockaddr.sin_addr.s_addr = INADDR_ANY;

	// // /
	m_port = nSocketPort;
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_port = htons((u_short)GetPort());
	if( bind( m_socket, (struct sockaddr *)&m_sockaddr, sizeof(m_sockaddr) ) < 0 )
	{
		s_message = "Bind error,Please check the port";
		OnEvent( YW_EVENT_SOCKET_BIND_ERROR );
		return false;
	}

	s_message = "Bind OK";

	return true;
}


bool YWSocket_UDP::Close( bool bMan )
{
	try
	{
		m_bManul = bMan;

		if (m_NetType==1)//若为组播，则发送退出组消息
		{
			/* send a DROP MEMBERSHIP message via setsockopt */
			if ((setsockopt(m_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, 
				(const char*) &mc_req, sizeof(mc_req))) < 0) {
				s_message = "Socket SetOpt: Drop MemberShip fail!\n";
				return false;
			}
		}

		// 停止数据接收线程
		if( IsReceiving() )
			StopReceive();

		// 关闭Socket
		if(m_socket >0)
		{
			shutdown(m_socket, 2);
			CloseSocket(m_socket);
			m_socket = -1;
			m_bCreateOK = false;

			if( m_bManul ) {
				OnEvent( YW_EVENT_DISCONNECT, NULL, 0);
				m_bManul = false;
			}
		}

		return true;
	}
	catch (...) 
	{
		return false;
	}
}


int YWSocket_UDP::Send(char * lpBuf, int nBufLen, const char *sendIP, unsigned short int sendPort )
{
	if( m_socket != -1 )
	{
		sockaddr_in destaddr;
		memset(&destaddr,0,sizeof(sockaddr_in));  
		destaddr.sin_family   =   AF_INET;  
		destaddr.sin_addr.s_addr = inet_addr( (char*)sendIP );
		destaddr.sin_port   = htons( sendPort );

		int err = sendto( m_socket, lpBuf, nBufLen,0,(sockaddr*)&destaddr,sizeof(destaddr));  
		//if(err == -1)  
		//{  
		//	CloseSocket(m_socket);  
		//	return -1;  
		//}
		return err;
	}

	return nBufLen;
}


void * YWSocket_UDP::thrReceive( void *pParam)
{
	YWSocket_UDP *pSock = (YWSocket_UDP *)pParam;

	return pSock->RecData();

	return NULL;
}

void *YWSocket_UDP::RecData()
{
	pthread_cleanup_push( CB_CleanReceiveThr, this );

	m_bIsReceiving = true;
	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	if( m_pRecvBuf ) {delete []m_pRecvBuf;m_pRecvBuf=NULL;}

	m_pRecvBuf = new unsigned char[ m_iRecBuffLen];
	
	//! 数据接收线程启动
	OnEvent(YW_EVENT_RECEIVE_THREAD_START);

	_recThrExitFlag = YW_EVENT_RECEIVE_THREAD_EXIT;
	int nReceived=0;
	for(;;)
	{
		pthread_testcancel();

		fd_set fds;
		FD_ZERO(&fds);
		FD_SET( m_socket,&fds);

		// filter the socket that have data recvice
		nReceived = select( m_socket + 1, &fds, NULL, NULL, &_recThrTimeout );
		if(m_socket == INVALID_SOCKET) break;
		if( nReceived == 0 )
		{	// select 超时
			OnEvent( YW_EVENT_RECEIVE_THREAD_TIMEOUT );
		}
		else if( nReceived == -1)
		{
			_recThrExitFlag = YW_EVENT_RECEIVE_ERROR;
			pthread_detach( pthread_self() );
			break;
		}
		else
		{
			if(FD_ISSET( m_socket, &fds ) )
			{
				pthread_testcancel();

				nReceived = recvfrom( m_socket,	(char*)m_pRecvBuf,m_iRecBuffLen,0,(SOCKADDR *)&SenderAddr, &SenderAddrSize );
	
				pthread_testcancel();
				if (nReceived == -1)
				{
					/*_recThrExitFlag = YW_EVENT_DROP_ERROR;
#ifdef WIN32
					DWORD dwErr = ::GetLastError();
					if(dwErr == WSAECONNRESET)
						continue;

					OnEvent( _recThrExitFlag, (void*)dwErr, sizeof( DWORD ) );
#endif
					pthread_detach( pthread_self() );
					break;*/
				}
				else if( nReceived == 0 ) 
				{
					/*_recThrExitFlag = YW_EVENT_DROP_REMOTE;
					pthread_detach( pthread_self() );
					break;*/
				}
				else
				{
					if(nReceived<(m_iRecBuffLen-1))//添加结束符，避免字符串乱码
					{
						m_pRecvBuf[nReceived]=0;
						m_pRecvBuf[nReceived+1]=0;
					}
					OnReceiveData( (char*)m_pRecvBuf, nReceived);
				}
			}
		}

		pthread_testcancel();
	}

	pthread_cleanup_pop( 1 ); // 无论如何都会调用 CB_CleanReceiveThr()函数，清理资源。

	return NULL;
}



#ifdef WIN32
void *YWSocket_UDP::CB_CleanReceiveThr( void *pSock )
#else
void YWSocket_UDP::CB_CleanReceiveThr( void *pSock )
#endif
{
	YWSocket_UDP *socket = (YWSocket_UDP *)pSock;
	socket->m_bIsReceiving = false;
	if( socket->_recThrExitFlag != YW_EVENT_RECEIVE_THREAD_EXIT && socket->m_socket != INVALID_SOCKET) 
	{
		// 表示是网络异常退出
		shutdown(socket->m_socket, 2);
		CloseSocket(socket->m_socket);
		socket->m_socket = -1;
		socket->m_bCreateOK = false;
		socket->OnEvent( socket->_recThrExitFlag );
	}

#ifdef WIN32
	return NULL;
#endif
}


//! Get the error message
/*!
\b 修改：
- lava_sdb 2003-11-20

\return The error string
*/
const char *YWSocket_UDP::GetError()
{
	return s_message.c_str();
}


bool YWSocket_UDP::Create( bool bStartRec, const char* szBindIP, unsigned int nBindPort, unsigned int ReceBufLen/*=8192*/,unsigned int SendBufLen/*=8192*/, long timeout/* = 1000000*/)
{
	if(m_socket>0)
	{
		s_message = "Socket has been create!";
		return false;
	}

	// Create the Socket
	if(!Socket( AF_INET, SOCK_DGRAM, 0))
	{
		s_message = "Socket Create fail!\n";
		return false;
	}

	m_iRecBuffLen = ReceBufLen;

	// 设置接收缓冲区
	int recBuf=0,reclen=sizeof(int);
	getsockopt( m_socket,SOL_SOCKET ,SO_RCVBUF ,(char*)&recBuf, (int*)&reclen);	//Windows系统默认8192的缓冲区大小
	if(recBuf<ReceBufLen)
	{
		recBuf = ReceBufLen;
		setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&recBuf, sizeof(int));
	}


	// 设置发送缓冲区
	int sndBuf=0,sndlen = sizeof(int);
	getsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, (int*)&sndlen);	
	if(sndBuf<SendBufLen)
	{
		sndBuf = SendBufLen;
		setsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, sizeof(int));
	}


	DWORD   dwBytesReturned   =   0;	  
	BOOL     bNewBehavior   =   FALSE;   
	WSAIoctl(m_socket, SIO_UDP_CONNRESET,&bNewBehavior,sizeof(bNewBehavior),NULL,0,&dwBytesReturned,NULL,NULL);

	if(!Bind( nBindPort, szBindIP))
	{
		s_message = "Server Bind fail!\n";
		CloseSocket(m_socket);
		m_socket = -1;
		return false;
	}

	// Connect to the Host
	int timeoutT   =   100; 
	::setsockopt( m_socket,SOL_SOCKET, SO_SNDTIMEO, (char*)&timeoutT, sizeof(int) ); //发送  
	::setsockopt( m_socket,SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutT, sizeof(int) ); //接收

	OnEvent( YW_EVENT_UDP_CREATE_OK );
	//////////////////////////////////////////////////////////////////////////
	_recThrTimeout.tv_sec = timeout/1000000;
	_recThrTimeout.tv_usec = timeout%1000000;
	
	if( bStartRec )
		pthread_create( &m_thrRec, NULL, thrReceive , m_pThis);

	m_bCreateOK = true;

	return true;
}

bool YWSocket_UDP::Create( bool bStartRec, const char* szLocalIP, unsigned int nLocalPort, const char* szMCIP, unsigned int nMCPort, unsigned int ReceBufLen/*=8192*/,unsigned int SendBufLen/*=8192*/, long timeout /*= 3000000*/ )
{
	if(m_socket>0)
	{
		s_message = "Socket has been create!";
		return false;
	}

	// Create the Socket
	if(!Socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP))
	{
		s_message = "Socket Create fail!\n";
		return false;
	}

	m_iRecBuffLen = ReceBufLen;
	// 设置接收缓冲区
	int recBuf=0,reclen=sizeof(int);
	getsockopt( m_socket,SOL_SOCKET ,SO_RCVBUF ,(char*)&recBuf, (int*)&reclen);	//Windows系统默认8192的缓冲区大小
	if(recBuf<=ReceBufLen)
	{
		recBuf = ReceBufLen;
		setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&recBuf, sizeof(int));
	}

	// 设置发送缓冲区
	int sndBuf=0,sndlen = sizeof(int);
	getsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, (int*)&sndlen);	
	if(sndBuf<=SendBufLen)
	{
		sndBuf = SendBufLen;
		setsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, sizeof(int));
	}

	int flag_on = 1;              /* socket option flag */
	/* set reuse port to on to allow multiple binds per host */
	if ((setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag_on,
		sizeof(flag_on))) < 0) 
	{
			s_message = "Socket SetOpt: Reuse port fail!\n";
			return false;
	}

	DWORD   dwBytesReturned   =   0;	  
	BOOL     bNewBehavior   =   FALSE;   
	WSAIoctl(m_socket, SIO_UDP_CONNRESET,&bNewBehavior,sizeof(bNewBehavior),NULL,0,&dwBytesReturned,NULL,NULL);

	if(!Bind( nMCPort, szLocalIP))
	{
		s_message = "Server Bind fail!\n";
		CloseSocket(m_socket);
		m_socket = -1;
		return false;
	}

	// Connect to the Host
	int timeoutT   =   100; 
	::setsockopt( m_socket,SOL_SOCKET, SO_SNDTIMEO, (char*)&timeoutT, sizeof(int) ); //发送  
	::setsockopt( m_socket,SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutT, sizeof(int) ); //接收

	/* construct an IGMP join request structure */
	mc_req.imr_multiaddr.s_addr = inet_addr(szMCIP);
	mc_req.imr_interface.s_addr = inet_addr(szLocalIP);/*htonl(INADDR_ANY)*/;

	/* send an ADD MEMBERSHIP message via setsockopt */
	if ((setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
		(char*) &mc_req, sizeof(mc_req))) < 0)
	{
			s_message = "Socket SetOpt: Add MemberShip fail!\n";
			return false;
	}

	OnEvent( YW_EVENT_UDP_CREATE_OK );
	//////////////////////////////////////////////////////////////////////////
	_recThrTimeout.tv_sec = timeout/1000000;
	_recThrTimeout.tv_usec = timeout%1000000;

	if( bStartRec )
		pthread_create( &m_thrRec, NULL, thrReceive , m_pThis);

	m_bCreateOK = true;
	m_NetType = 1;//组播标记

	return true;
}
bool YWSocket_UDP::CreateEx( int NetType,bool bStartRec, const char* szLocalIP, unsigned int nLocalPort, const char* szDstIP, unsigned int nDstPort, unsigned int ReceBufLen/*=8192*/,unsigned int SendBufLen/*=8192*/, long timeout /*= 3000000*/ )
{
	if(m_socket>0)
	{
		s_message = "Socket has been create!";
		return false;
	}

	// Create the Socket
	if(!Socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP))
	{
		s_message = "Socket Create fail!\n";
		return false;
	}

	m_iRecBuffLen = ReceBufLen;
	// 设置接收缓冲区
	int recBuf=0,reclen=sizeof(int);
	getsockopt( m_socket,SOL_SOCKET ,SO_RCVBUF ,(char*)&recBuf, (int*)&reclen);	//Windows系统默认8192的缓冲区大小
	if(recBuf<=ReceBufLen)
	{
		recBuf = ReceBufLen;
		setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&recBuf, sizeof(int));
	}

	// 设置发送缓冲区
	int sndBuf=0,sndlen = sizeof(int);
	getsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, (int*)&sndlen);	
	if(sndBuf<=SendBufLen)
	{
		sndBuf = SendBufLen;
		setsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, sizeof(int));
	}

	int flag_on = 1;              /* socket option flag */
	/* set reuse port to on to allow multiple binds per host */
	if ((setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag_on,
		sizeof(flag_on))) < 0) 
	{
		s_message = "Socket SetOpt: Reuse port fail!\n";
		return false;
	}

	DWORD   dwBytesReturned   =   0;	  
	BOOL     bNewBehavior   =   FALSE;   
	WSAIoctl(m_socket, SIO_UDP_CONNRESET,&bNewBehavior,sizeof(bNewBehavior),NULL,0,&dwBytesReturned,NULL,NULL);

	int nPort = nLocalPort;
	if(NetType==1)//组播
		nPort = nDstPort;
	if(!Bind( nPort, szLocalIP))
	{
		s_message = "Server Bind fail!\n";
		CloseSocket(m_socket);
		m_socket = -1;
		return false;
	}

	// Connect to the Host
	int timeoutT   =   100; 
	::setsockopt( m_socket,SOL_SOCKET, SO_SNDTIMEO, (char*)&timeoutT, sizeof(int) ); //发送  
	::setsockopt( m_socket,SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutT, sizeof(int) ); //接收

	if(NetType==1)//组播
	{
		/* construct an IGMP join request structure */
		mc_req.imr_multiaddr.s_addr = inet_addr(szDstIP);
		mc_req.imr_interface.s_addr = inet_addr(szLocalIP);/*htonl(INADDR_ANY)*/;

		/* send an ADD MEMBERSHIP message via setsockopt */
		if ((setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
			(char*) &mc_req, sizeof(mc_req))) < 0)
		{
			s_message = "Socket SetOpt: Add MemberShip fail!\n";
			return false;
		}
	}
	else if(NetType==2)//广播
	{
		BOOL bBroadcast=TRUE;
		if(setsockopt(m_socket,SOL_SOCKET,SO_BROADCAST,(const char*)&bBroadcast,sizeof(BOOL))<0)
		{
			s_message = "Socket SetOpt: Set Broadcast fail!\n";
			return false;
		}
	}
	

	OnEvent( YW_EVENT_UDP_CREATE_OK );
	//////////////////////////////////////////////////////////////////////////
	_recThrTimeout.tv_sec = timeout/1000000;
	_recThrTimeout.tv_usec = timeout%1000000;

	if( bStartRec )
		pthread_create( &m_thrRec, NULL, thrReceive , m_pThis);

	m_bCreateOK = true;
	m_NetType = NetType;//网络类型标记

	return true;
}


void YWSocket_UDP::StartReceive()
{
	if( m_bCreateOK && !m_bIsReceiving ) {
		pthread_create( &m_thrRec, NULL, thrReceive , m_pThis);
	}
}

void YWSocket_UDP::StopReceive()
{
	if( m_bCreateOK )
	{
		if( m_bIsReceiving ) 
		{
			//快速退出select
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET( m_socket,&fds);
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;

			pthread_cancel( m_thrRec );
			pthread_join( m_thrRec, NULL);
			m_bIsReceiving = false;
			OnEvent(YW_EVENT_RECEIVE_THREAD_STOP);
		}
	}
}


