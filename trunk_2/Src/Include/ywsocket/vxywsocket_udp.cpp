
#include "vxywsocket_udp.h"
#include "taskLib.h"

using namespace CDYW::SOCKET;


//////////////////////////////////////////////////////////////////////////
///// class YWSocket_UDP
YWSocket_UDP::YWSocket_UDP()
{
	//init variable
	m_bCreateOK = false;
	m_bManul = false;
	s_message	= "";
	m_socket	= -1;
	//memset( &m_thrRec, 0, sizeof( pthread_t ) );

	m_iBuffLen	= 0;
	m_bIsReceiving = false;
	m_pRecvBuf = NULL;

	m_iVxLeavePackgeSize = 0;

	_recThrTimeout.tv_sec = 1;
	_recThrTimeout.tv_usec = 1000000;
	m_tidReceive = 0;
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
		if(err == -1)  
		{
			return -1;
		}
	}

	return nBufLen;
}

void * YWSocket_UDP::thrReceive( void *pParam)
{
	YWSocket_UDP *pSock = (YWSocket_UDP *)pParam;

	if(pSock->m_iVxLeavePackgeSize == 0 )
	{
		return pSock->RecDataT();
	}else{
		return pSock->RecData();
	}
	return NULL;
}


void *YWSocket_UDP::RecDataT()
{
	pthread_cleanup_push( CB_CleanReceiveThr, this );

	int iLen = m_iBuffLen; // 缓冲区还剩余的字节数
	m_bIsReceiving = true;
	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	if( m_pRecvBuf ) {
		delete []m_pRecvBuf;
	}
	m_pRecvBuf = new unsigned char[ m_iBuffLen * 2 ];
	unsigned char *pW = m_pRecvBuf;

	//! 数据接收线程启动
	OnEvent(YW_EVENT_RECEIVE_THREAD_START);

	_recThrExitFlag = YW_EVENT_RECEIVE_THREAD_EXIT;
	int nReceived = 0;
	fd_set fds;

	for(;;)
	{
		//pthread_testcancel();

		FD_ZERO(&fds);
		FD_SET( m_socket,&fds);

		nReceived = select( m_socket + 1, &fds, NULL, NULL, &_recThrTimeout );
		if( nReceived == 0 )
		{	// select 超时
			OnEvent( YW_EVENT_RECEIVE_THREAD_TIMEOUT );
		}else if( nReceived == -1)
		{
			_recThrExitFlag = YW_EVENT_RECEIVE_ERROR;
			//pthread_detach( pthread_self() );
			break;
		}
		else
		{
			if(FD_ISSET( m_socket, &fds ) )
			{
				//pthread_testcancel();

				nReceived = recvfrom( m_socket,
					(char*)pW,
					m_iBuffLen,
					0,
					(SOCKADDR *)&SenderAddr, 
					&SenderAddrSize );

				//pthread_testcancel();

				if (nReceived == ERROR)
				{
					//_recThrExitFlag = YW_EVENT_DROP_ERROR;
					//pthread_detach( pthread_self() );
					//break;
					printf( "UDP Receive Error.\n" );
				}else {
					/// 做判断
					OnReceiveData( (char*)m_pRecvBuf, m_iBuffLen);
					iLen = m_iBuffLen;
					pW = m_pRecvBuf;
				}
			}
		}

		//pthread_testcancel();
	}

	//pthread_cleanup_pop( 1 ); // 无论如何都会调用 CB_CleanReceiveThr()函数，清理资源。

	return NULL;
}


void *YWSocket_UDP::RecData()
{
	//pthread_cleanup_push( CB_CleanReceiveThr, this );

	int iMainPackgeSize = m_iBuffLen - m_iVxLeavePackgeSize;
	int iLeavePackgeSize = m_iVxLeavePackgeSize;

	int iLen = m_iBuffLen; // 缓冲区还剩余的字节数

	m_bIsReceiving = true;
	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	if( m_pRecvBuf ) {
		delete []m_pRecvBuf;
	}
	m_pRecvBuf = new unsigned char[ m_iBuffLen * 2 ];
	unsigned char *pW = m_pRecvBuf;

	//! 数据接收线程启动
	OnEvent(YW_EVENT_RECEIVE_THREAD_START);

	_recThrExitFlag = YW_EVENT_RECEIVE_THREAD_EXIT;
	int nReceived=0;
	for(;;)
	{
		//pthread_testcancel();

		fd_set fds;
		FD_ZERO(&fds);

		FD_SET( m_socket,&fds);

		// filter the socket that have data recvice
		nReceived = select( m_socket + 1, &fds, NULL, NULL, &_recThrTimeout );

		if( nReceived == 0 )
		{	// select 超时
			OnEvent( YW_EVENT_RECEIVE_THREAD_TIMEOUT );
		}
		else if( nReceived == -1)
		{
			_recThrExitFlag = YW_EVENT_RECEIVE_ERROR;
			//pthread_detach( pthread_self() );
			break;
		}
		else
		{
			if(FD_ISSET( m_socket, &fds ) )
			{
				//pthread_testcancel();

				nReceived = recvfrom( m_socket,
					(char*)pW,
					m_iBuffLen,
					0,
					(SOCKADDR *)&SenderAddr, 
					&SenderAddrSize );

				//pthread_testcancel();
				if (nReceived == -1)
				{
					_recThrExitFlag = YW_EVENT_DROP_ERROR;
#ifdef WIN32
					DWORD dwErr = ::GetLastError();
					OnEvent( _recThrExitFlag );
#endif
					//pthread_detach( pthread_self() );
					break;
				}
				else if( nReceived == 0 ) {
					_recThrExitFlag = YW_EVENT_DROP_REMOTE;
					//pthread_detach( pthread_self() );
					break;
				}
				else {
					/// 做判断
					if( nReceived > iMainPackgeSize)
					{// 大包
						if( nReceived > iLen )
						{ 
							// 丢了小包
							memmove( m_pRecvBuf, pW, nReceived );
							iLen = m_iBuffLen - nReceived;
							pW = m_pRecvBuf + nReceived;
							OnEvent( YW_EVENT_RECEIVE_LOST_PACKGE, (void*)iLen );
						}else{
							pW += nReceived;
							iLen = m_iBuffLen - nReceived;
						}
					}else{
						if( iLen == nReceived )
						{
							// 数据包接收正确
							// 接收正确
							OnReceiveData( (char*)m_pRecvBuf, m_iBuffLen);
							char *pDataType = (char *)m_pRecvBuf;
							unsigned char *pEleInd = (unsigned char*)(m_pRecvBuf+ 84);
							int *pCutInd = (int*)(m_pRecvBuf+ 85);
							if( *pDataType == 0 ) {
								tagRecePackgeInfo info;
								info.iBuffSize = m_iBuffLen;
								info.iReceSize = nReceived;
								info.iReverd0 = *pEleInd;
								info.iReverd1 = *pCutInd;
								OnEvent( YW_EVENT_RECEIVE_PACKGE_INFO, &info, sizeof(tagRecePackgeInfo) );
							}
						}else{
							// 丢了大包,放弃此小包
							int iLost = iLen;
							OnEvent( YW_EVENT_RECEIVE_LOST_PACKGE, (void*)iLost );
						}

						iLen = m_iBuffLen;
						pW = m_pRecvBuf;
					}
				}
			}
		}

		//pthread_testcancel();
	}

	//pthread_cleanup_pop( 1 ); // 无论如何都会调用 CB_CleanReceiveThr()函数，清理资源。

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
	if( socket->_recThrExitFlag != YW_EVENT_RECEIVE_THREAD_EXIT ) {
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


bool YWSocket_UDP::Create( bool bStartRec, const char *szBindIP, unsigned int nBindPort, unsigned int tcpBufLen,unsigned int prio,char *taskname,long timeout = 1000000)
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

	//if( tcpBufLen > 0 )
	{
		// 设置接收缓冲区
		int recBuf,reclen=sizeof(int);
		getsockopt( m_socket,SOL_SOCKET ,SO_RCVBUF ,(char*)&recBuf, (int*)&reclen);
		if( recBuf < tcpBufLen )
		{
			recBuf = tcpBufLen;
			setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&recBuf, sizeof(int));
		}

		// 设置发送缓冲区
		int sndBuf,sndlen = sizeof(int);
		getsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, (int*)&sndlen);	
		if( sndBuf < tcpBufLen )
		{
			sndBuf = tcpBufLen;
			setsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, sizeof(int));
		}

		m_iBuffLen = tcpBufLen;
	}

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
	//	thread_create( &m_thrRec, NULL, thrReceive , this);
	m_tidReceive = taskSpawn(taskname,prio,0,64*1024,(FUNCPTR)thrReceive,(int)this,0,0,0,0,0,0,0,0,0);
	m_bCreateOK = true;

	return true;
}
void YWSocket_UDP::StartReceive(char *taskname , int prio)
{
	if( m_bCreateOK && !m_bIsReceiving ) {
		m_tidReceive = taskSpawn(taskname,prio,0,64*1024,(FUNCPTR)thrReceive,(int)this,0,0,0,0,0,0,0,0,0);
	//	pthread_create( &m_thrRec, NULL, thrReceive , this);
	}
}

//void YWSocket_UDP::StopReceive()
//{
//	if( m_bCreateOK )
//	{
//		if( m_bIsReceiving ) {
//			pthread_cancel( m_thrRec );
//			pthread_join( m_thrRec, NULL);
//			m_bIsReceiving = false;
//			OnEvent(YW_EVENT_RECEIVE_THREAD_STOP);
//		}
//	}
//}
//

void YWSocket_UDP::StopReceive()
{
	if( m_bCreateOK )
	{
		if( m_bIsReceiving ) {
			taskDelete(m_tidReceive);
			m_tidReceive = 0;
			m_bIsReceiving = false;
			OnEvent(YW_EVENT_RECEIVE_THREAD_STOP);
		}
	}
}


